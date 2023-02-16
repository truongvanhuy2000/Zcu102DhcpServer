#ifndef DHCP_SERVER
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

int start_application(struct netif *netif);
void dhcpListener();
#endif
