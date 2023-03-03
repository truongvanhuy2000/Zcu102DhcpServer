#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "xil_printf.h"
#include "xparameters.h"
#include "networkInit.h"
#include "dhcpServer.h"
#include "httpApplication.h"

#define THREAD_STACKSIZE 1024

static TaskHandle_t xemacInputHandle;
static TaskHandle_t DhcpHandler;
static TaskHandle_t httpPostHandler;

static TaskHandle_t startTaskHandle;
static void initTask(void *pvParameters);
static void xemacInputTask(void *pvParameters);
static void dhcpTask(void *pvParameters);
static void httpPostTask(void *pvParameters);

int main()
{
	xTaskCreate(xemacInputTask, (const char *)"xemac", THREAD_STACKSIZE,
				NULL, 1, &xemacInputHandle);
	xTaskCreate(dhcpTask, (const char *)"dhcp", THREAD_STACKSIZE,
				NULL, 1, &DhcpHandler);
	xTaskCreate(httpPostTask, (const char *)"httpPost", THREAD_STACKSIZE,
				NULL, 1, &httpPostHandler);

	xTaskCreate(initTask, (const char *)"init", THREAD_STACKSIZE,
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
	start_application(&server_netif);
	while (1)
	{
		if (dhcpListener() == 2)
		{
			closeApplication();
			vTaskResume(httpPostHandler);
			vTaskSuspend(DhcpHandler);
		}
	}
}
static void initTask(void *pvParameters)
{
	if (!networkInit())
	{
		print("TCP/IP configuration error");
		return;
	}

	vTaskDelete(startTaskHandle);
}

static void httpPostTask(void *pvParameters)
{

	vTaskSuspend(httpPostHandler);
	while (1)
	{
		httpApplicationPost();
		vTaskDelay(500);
	}
}
