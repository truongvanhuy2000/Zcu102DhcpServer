#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "xil_printf.h"
#include "xparameters.h"
#include "tcpIpController.h"
#include "dhcpServer.h"
static TaskHandle_t xemacInputHandle;
static TaskHandle_t DhcpHandler;

static TaskHandle_t startTaskHandle;
static void initTask(void *pvParameters);
static void xemacInputTask(void *pvParameters);
static void dhcpTask(void *pvParameters);

int main()
{
	xTaskCreate(xemacInputTask, (const char *)"xemac", 500,
				NULL, 1, &xemacInputHandle);
	xTaskCreate(dhcpTask, (const char *)"dhcp", 500,
				NULL, 1, &DhcpHandler);
	xTaskCreate(initTask, (const char *)"init", 1000,
				NULL, 2, &startTaskHandle);
	vTaskStartScheduler();
	while (1)
		;
	return 0;
}

void xemacInputTask(void *pvParameters)
{
	while (1)
	{
		xemacNetifInput();
	}
}
void dhcpTask(void *pvParameters)
{
	while (1)
	{
		dhcpListener();
	}
}
static void initTask(void *pvParameters)
{
	if (!tcpIpControllerInit())
	{
		print("TCP/IP configuration error");
		return;
	}
	vTaskDelete(startTaskHandle);
}
