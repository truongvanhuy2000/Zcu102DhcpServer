#include "tcpIpController.h"
struct netif server_netif;
static void print_ip(char *msg, ip_addr_t *ip)
{
    xil_printf(msg);
    xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
               ip4_addr3(ip), ip4_addr4(ip));
}

static void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
    print_ip("Board IP:       ", ip);
    print_ip("Netmask :       ", mask);
    print_ip("Gateway :       ", gw);
}
static void assign_default_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
    int err;

    xil_printf("Configuring default IP %s \r\n", DEFAULT_IP_ADDRESS);

    err = inet_aton(DEFAULT_IP_ADDRESS, ip);
    if (!err)
        xil_printf("Invalid default IP address: %d\r\n", err);

    err = inet_aton(DEFAULT_IP_MASK, mask);
    if (!err)
        xil_printf("Invalid default IP MASK: %d\r\n", err);

    err = inet_aton(DEFAULT_GW_ADDRESS, gw);
    if (!err)
        xil_printf("Invalid default gateway address: %d\r\n", err);
}
void xemacNetifInput(void)
{
    xemacif_input(&server_netif);
}
int tcpIpControllerInit()
{
    struct netif *netif;

    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = {
        0x00, 0x0a, 0x35, 0x00, 0x01, 0x02};
    netif = &server_netif;
    xil_printf("-----lwIP RAW Mode TCP Client Application-----\r\n");
    /* initialize lwIP */
    lwip_init();

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address,
                   PLATFORM_EMAC_BASEADDR))
    {
        xil_printf("Error adding N/W interface\r\n");
        return 0;
    }
    netif_set_default(netif);
    netif_set_up(netif);

    assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
    print_ip_settings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
    xil_printf("\r\n");
    /* start the application*/
    if (!start_application(netif))
    {
        return 0;
    }
    xil_printf("\r\n");
    return 1;
}
