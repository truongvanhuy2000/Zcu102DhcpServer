#include "dhcpServer.h"
#include <string.h>

#define MAX_BYTE 1024
#define SERVER_SOCKET 67
#define CLIENT_SOCKET 68
#define CLIENT_ADDRESS "255.255.255.255"

#define DHCP_PACKET_SIZE 236

const char magicCookie[4] = {0x63, 0x82, 0x53, 0x63};
const char offerIp[4] = {192, 168, 1, 25};

int sock;
struct sockaddr_in server_sock;

int start_application(struct netif *netif)
{
    int enable_broadcast = 1;
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
    //  server_sock.sin_addr.s_addr = netif->ip_addr.addr;

    if (bind(sock, (struct sockaddr *)&server_sock, sizeof(server_sock)) < 0)
    {
        xil_printf("Bind Server Address with Socket error Error\r\n");
        return 0;
    }
    //    while(1)
    //    {
    //    	sendDhcpReply("asdasdas");
    //    	sleep(1);
    //    }
    xil_printf("DHCP Server is listening on %d", (int)server_sock.sin_port);
    return 1;
}
void deleteOptionList(dhcp_option_list *dhcpOptionList)
{
    dhcp_option_list *ptr;
    dhcp_option_list *temp;

    ptr = dhcpOptionList;
    while (ptr != NULL)
    {
        temp = ptr->next_option;
        free(ptr);
        ptr = temp;
    }
}
int parseDhcpOption(dhcp_option *option, uint8_t *optionPtr)
{
    int optionLen;
    option->id = *optionPtr;
    option->len = *(optionPtr + 1);
    memcpy(option->data, optionPtr + 2, option->len);
    optionLen = option->len + 2;

    return optionLen;
}

int parseDhcpOptionList(dhcp_msg *msg, int len)
{
    dhcp_option_list *optionListPtr;
    uint8_t *optionArrayPtr;
    int nextOptionPos;

    optionArrayPtr = msg->hdr.options;

    // extract magic cookie
    if (len < 4 || memcmp(optionArrayPtr, magicCookie, sizeof(magicCookie) != 0))
    {
        return 0;
    }
    optionArrayPtr += 4;
    msg->opts = malloc(sizeof(dhcp_option_list));
    optionListPtr = msg->opts;
    while (*optionArrayPtr != END)
    {
        nextOptionPos = parseDhcpOption(&optionListPtr->dhcp_option, optionArrayPtr);
        optionListPtr->next_option = malloc(sizeof(dhcp_option_list));
        optionListPtr = optionListPtr->next_option;
        optionArrayPtr += nextOptionPos;
    }
    optionListPtr->next_option = NULL;
    return 1;
}

dhcp_option *seachForOption(dhcp_option_list *optionList, int optionCode)
{
    while (optionList->next_option != NULL)
    {
        if (optionList->dhcp_option.id == optionCode)
        {
            return &(optionList->dhcp_option);
        }
    }
    return NULL;
}

void init_reply(dhcp_msg *request, dhcp_msg *reply)
{
    memset(&reply->hdr, 0, sizeof(reply->hdr));

    reply->hdr.op = BOOTREPLY;

    reply->hdr.htype = request->hdr.htype;
    reply->hdr.hlen = request->hdr.hlen;

    reply->hdr.xid = request->hdr.xid;
    reply->hdr.flags = request->hdr.flags;

    reply->hdr.giaddr = request->hdr.giaddr;

    memcpy(reply->hdr.chaddr, request->hdr.chaddr, request->hdr.hlen);
}
int appendOptionToArray(uint8_t *optionArrPtr, dhcp_option *option)
{
    int optionLen;
    optionArrPtr[0] = option->id;
    optionArrPtr[1] = option->len;
    memcpy(optionArrPtr + 2, option->data, option->len);
    optionLen = option->len + 2;
    return optionLen;
}
void serializeOptionList(dhcp_msg *reply)
{
    uint8_t *optionArrPtr;
    dhcp_option_list *optionPtr;
    int optionLen;

    optionPtr = reply->opts;
    optionArrPtr = reply->hdr.options;
    memcpy(reply->hdr.options, magicCookie, sizeof(magicCookie));
    optionArrPtr += 4;
    do
    {
        optionLen = appendOptionToArray(optionArrPtr, &(optionPtr->dhcp_option));
        optionArrPtr += optionLen;
        optionPtr = optionPtr->next_option;
    } while (optionPtr != NULL);
    *optionArrPtr = END;
}
// this one will be used to fill in the dhcp packet. Usually being used when sending
// packet
void fillDhcpPacket(dhcp_msg *request, dhcp_msg *reply, uint8_t type, const uint8_t *address)
{
    dhcp_option_list *typeOption = malloc(sizeof(dhcp_option_list));
    typeOption->dhcp_option.data[0] = type;
    typeOption->dhcp_option.id = DHCP_MESSAGE_TYPE;
    typeOption->dhcp_option.len = 1;
    typeOption->next_option = NULL;
    reply->opts = typeOption;

    if (address != NULL)
    {
        reply->hdr.yiaddr = *((uint32_t *)address);
    }
    serializeOptionList(reply);
}
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
    fillDhcpPacket(request, reply, DHCP_OFFER, offerIp);
    return 1;
}
void requestPacketHandler(dhcp_msg *request, dhcp_msg *reply)
{
    fillDhcpPacket(request, reply, DHCP_ACK, offerIp);
}
// This function will be used to listen to incoming packet and indentify it
void dhcpListener()
{
    xil_printf("Wait for DHCP Discovery Request");
    struct sockaddr_in client_sock;
    size_t socklen = sizeof(client_sock);
    size_t len;

    dhcp_msg requestMsg;
    dhcp_msg replyMsg;
    while (1)
    {
        memset(&requestMsg.hdr, 0, sizeof(requestMsg.hdr));
        len = recvfrom(sock, &requestMsg.hdr, sizeof(requestMsg.hdr), MSG_DONTWAIT, (struct sockaddr *)&client_sock, &socklen);
        // 236 is the minium length of a dhcp data packet
        if (len < DHCP_PACKET_SIZE)
            continue;
        if (requestMsg.hdr.op != BOOTREQUEST)
            continue;
        if (requestMsg.hdr.hlen < 1 || requestMsg.hdr.hlen > 16)
            continue;

        parseDhcpOptionList(&requestMsg, len - DHCP_PACKET_SIZE);

        dhcp_option *typeOption = seachForOption(requestMsg.opts, DHCP_MESSAGE_TYPE);
        if (typeOption == NULL)
        {
            continue;
        }

        init_reply(&requestMsg, &replyMsg);

        switch (typeOption->data[0])
        {
        case DHCP_DISCOVER:
            discoverPacketHandler(&requestMsg, &replyMsg);
            break;
        case DHCP_REQUEST:
            requestPacketHandler(&requestMsg, &replyMsg);
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
        sendDhcpReply(&(replyMsg.hdr));
        deleteOptionList(replyMsg.opts);
        deleteOptionList(requestMsg.opts);
    }
}
