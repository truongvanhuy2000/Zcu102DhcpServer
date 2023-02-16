#ifndef DHCP_SERVER
#define DHCP_SERVER

#include "lwipopts.h"
#include "xlwipconfig.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "xil_printf.h"
#include <sleep.h>
#include "dhcp.h"
#include "options.h"

#define MAX_BYTE 1024
#define SERVER_SOCKET 67
#define CLIENT_SOCKET 68
#define CLIENT_ADDRESS "255.255.255.255"

#define DHCP_PACKET_SIZE 236

int start_application(struct netif *netif);
void dhcpListener();
#endif
