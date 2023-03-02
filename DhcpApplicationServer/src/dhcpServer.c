#include "dhcpServer.h"
#include <string.h>
#include "linkList.h"

const char dhcpMagicCookie[4] = {0x63, 0x82, 0x53, 0x63};
const char offerIp[4] = {192, 168, 1, 25};

uint32_t infLeaseTime = 0xFFFFFFFF;

int sock;
struct sockaddr_in server_sock;
struct netif *serverNetif;

int fillDhcpPacket(dhcp_msg *request, dhcp_msg *reply, uint8_t type, const uint8_t *address);
void answerClientRequest(dhcp_option *option, dhcp_msg *reply);
void sendDhcpReply(dhcp_message *dhcp_msg);
int discoverPacketHandler(dhcp_msg *request, dhcp_msg *reply);
int requestPacketHandler(dhcp_msg *request, dhcp_msg *reply);

// Start the dhcp application
int start_application(struct netif *netif)
{
    int enable_broadcast = 1;
    serverNetif = netif;

    xil_printf("DHCP server is starting...\n");
    // Create the socket

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        xil_printf("Error in creating Socket\r\n");
        return 0;
    }
    // used to enable or disable broadcast transmission on a socket
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable_broadcast, sizeof(enable_broadcast)) < 0)
    {
        xil_printf("Set broadcast in socket error\r\n");
        return 0;
    }
    memset(&server_sock, 0, sizeof(server_sock));

    server_sock.sin_family = AF_INET;
    server_sock.sin_port = htons(SERVER_SOCKET);
    server_sock.sin_addr.s_addr = htonl(IPADDR_ANY);
    // server_sock.sin_addr.s_addr = netif->ip_addr.addr;

    if (bind(sock, (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0)
    {
        xil_printf("Bind Server Address with Socket error Error\r\n");
        return 0;
    }
    xil_printf("DHCP Server is listening on %d", (int)server_sock.sin_port);

    return 1;
}

int closeApplication()
{
    if (close(sock) < 0)
    {
        xil_printf("close socket error\n\n");
        return 0;
    }
    return 1;
}

// This function will be used to listen to incoming packet and indentify it
// if you want to listen to multiple client, just create another thread for this
int dhcpListener()
{

    struct sockaddr_in client_sock;
    size_t socklen = sizeof(client_sock);
    int len;
    int returnStatus;
    dhcp_msg requestMsg;
    dhcp_msg replyMsg;
    while (1)
    {
        memset(&requestMsg.hdr, 0, sizeof(requestMsg.hdr));
        requestMsg.opts = NULL;
        memset(&client_sock, 0, socklen);
        len = recvfrom(sock, &requestMsg.hdr, sizeof(requestMsg.hdr), MSG_DONTWAIT, (struct sockaddr *)&client_sock, &socklen);
        // 236 is the minium length of a dhcp data packet
        if (checkValidIpv4Addr(client_sock.sin_addr.s_addr))
        {
            break;
        }
        if (len < DHCP_PACKET_SIZE)
            continue;
        if (requestMsg.hdr.op != BOOTREQUEST)
            continue;
        if (requestMsg.hdr.hlen < 1 || requestMsg.hdr.hlen > 16)
            continue;

        // the magic cookie used to define the packet, the magic cookie for this is "DHCP"
        if (len - DHCP_PACKET_SIZE < 4 || memcmp(requestMsg.hdr.magicCookie, dhcpMagicCookie, sizeof(dhcpMagicCookie) != 0))
        {
            continue;
        }
        parseDhcpOptionList(&requestMsg);

        // Seach for message type option to know what kind of packet we are dealing with
        dhcp_option *typeOption = seachForOption(requestMsg.opts, DHCP_MESSAGE_TYPE);
        if (typeOption == NULL)
        {
            continue;
        }
        // we only serve 2 type of request this time but you can add more in the future
        switch (typeOption->data[0])
        {
        case DHCP_DISCOVER:
            if (discoverPacketHandler(&requestMsg, &replyMsg))
            {
                returnStatus = 1;
            }
            break;
        case DHCP_REQUEST:
            if (requestPacketHandler(&requestMsg, &replyMsg))
            {
                returnStatus = 2;
            }
            break;
        case DHCP_DECLINE:
            break;
        case DHCP_RELEASE:
            break;
        case DHCP_INFORM:
            break;
        default:
            xil_printf("dhcp packet with invalid DHCP option\n");
            break;
        }
        if (returnStatus)
        {
            sendDhcpReply(&(replyMsg.hdr));
            xil_printf("Send Ip to Client\n");
        }
        // clean the option linked list to prevent memory leak
        deleteOptionList(requestMsg.opts);
        deleteOptionList(replyMsg.opts);
        return returnStatus;
    }
}
int checkValidIpv4Addr(int addr)
{
    int validAddr = inet_addr("192.168.1.1");
    if ((validAddr & 0xFF) != (addr & 0xFF))
    {
        return 0;
    }
    if (((validAddr >> 8) & 0xFF) != ((addr >> 4) & 0xFF))
    {
        return 0;
    }
    if (((validAddr >> 16) & 0xFF) != ((addr >> 16) & 0xFF))
    {
        return 0;
    }
    // if(((validAddr >> 24) & 0xFF) == ((addr >> 24) & 0xFF))
    // {
    //     return 0;
    // }
    return 1;
}
// this one will be used to fill in the dhcp packet. Usually being used when sending
// packet
int fillDhcpPacket(dhcp_msg *request, dhcp_msg *reply, uint8_t type, const uint8_t *address)
{
    dhcp_option_list *typeOption = malloc(sizeof(dhcp_option_list));
    if (!memset(&reply->hdr, 0, sizeof(reply->hdr)))
    {
        return 0;
    }
    // fill in the required parametter to match client
    reply->hdr.op = BOOTREPLY;
    reply->hdr.htype = request->hdr.htype;
    reply->hdr.hlen = request->hdr.hlen;
    reply->hdr.xid = request->hdr.xid;
    reply->hdr.flags = request->hdr.flags;
    reply->hdr.giaddr = request->hdr.giaddr;

    memcpy(reply->hdr.chaddr, request->hdr.chaddr, request->hdr.hlen);
    memcpy(reply->hdr.magicCookie, dhcpMagicCookie, sizeof(dhcpMagicCookie));

    // Message type option
    typeOption->dhcp_option = createOption(DHCP_MESSAGE_TYPE, 1, &type);
    typeOption->next_option = NULL;

    // the first option of the reply is what kind of dhcp packet this is
    reply->opts = typeOption;

    // fill in client adress
    if (address != NULL)
    {
        reply->hdr.yiaddr = *((uint32_t *)address);
    }
    return 1;
}

// This function will answer client parametter request
// We will only answer 2 request this time, but you can add new one in the future
void answerClientRequest(dhcp_option *option, dhcp_msg *reply)
{
    int count = 0;
    while (count < option->len)
    {
        dhcp_option newOption;
        switch (option->data[count])
        {
        // respond to subnet mask request
        case SUBNET_MASK:
            newOption = createOption(SUBNET_MASK, 4, &(serverNetif->netmask.addr));
            appendOptionToList(reply, &newOption);
            break;
        // respond to gateway request
        case ROUTER:
            newOption = createOption(ROUTER, 4, &(serverNetif->gw.addr));
            appendOptionToList(reply, &newOption);
            break;
        default:
            break;
        }
        count++;
    }
}

// send the dhcp reply to client, we will send a broadcast message
// what if multiple client listen at the same time? i dont fk know
void sendDhcpReply(dhcp_message *dhcp_msg)
{
    struct sockaddr_in dest;
    int size = sizeof(*dhcp_msg);
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = INADDR_BROADCAST; // use broadcast address
    dest.sin_port = htons(CLIENT_SOCKET);    // use a specific port
    // client_sock->sin_addr.s_addr = dhcp_msg->yiaddr;
    if (sendto(sock, dhcp_msg, size, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
    {
        xil_printf("error bitch \n");
    }
}

// this one will handling the discovery request
int discoverPacketHandler(dhcp_msg *request, dhcp_msg *reply)
{
    dhcp_option *parametterRequestOption;
    // fill in the the packet
    if (!fillDhcpPacket(request, reply, DHCP_OFFER, offerIp))
    {
        return 0;
    }
    // Seach for parametter request option
    parametterRequestOption = seachForOption(request->opts, PARAMETER_REQUEST_LIST);
    if (parametterRequestOption)
    {
        answerClientRequest(parametterRequestOption, reply);
    }
    dhcp_option option = createOption(IP_ADDRESS_LEASE_TIME, 4, &infLeaseTime);
    if (!appendOptionToList(reply, &option))
    {
        return 0;
    }
    serializeOptionList(reply);
    return 1;
}

// this one will handle the request packet
int requestPacketHandler(dhcp_msg *request, dhcp_msg *reply)
{
    dhcp_option *parametterRequestOption;
    if (!fillDhcpPacket(request, reply, DHCP_ACK, offerIp))
    {
        return 0;
    }
    parametterRequestOption = seachForOption(request->opts, PARAMETER_REQUEST_LIST);
    if (parametterRequestOption)
    {
        answerClientRequest(parametterRequestOption, reply);
    }
    // set the lease time to be infinity because we only serve one client this time
    dhcp_option option = createOption(IP_ADDRESS_LEASE_TIME, 4, &infLeaseTime);
    if (!appendOptionToList(reply, &option))
    {
        return 0;
    }
    serializeOptionList(reply);
    return 1;
}
