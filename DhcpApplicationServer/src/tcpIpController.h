#ifndef TCP_IP_CONTROLLER
#define TCP_IP_CONTROLLER
#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "lwipopts.h"
#include "xil_printf.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/init.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "dhcpServer.h"

#define DEFAULT_IP_ADDRESS "192.168.1.10"
#define DEFAULT_IP_MASK "255.255.255.0"
#define DEFAULT_GW_ADDRESS "192.168.1.1"
#define PLATFORM_EMAC_BASEADDR XPAR_XEMACPS_0_BASEADDR
void xemacNetifInput(void);
int tcpIpControllerInit();
#endif
