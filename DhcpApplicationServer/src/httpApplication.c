#include "httpApplication.h"
#include "stringx.h"

int sock;
struct sockaddr_in remote;

int checkHeader(httpHeader *header);
int createSocket(char *destination);
void closeSocket();
int sendHttpRequest(char *packet);
httpResponse *waitHttpResponse();
char *httpGet(httpHeader *customHeader);
char *httpPost(httpHeader *customHeader, char *body);
int createSocket();
void closeSocket();

void httpApplicationPost()
{
    char *httpPostPacket;
    httpResponse *response;
    httpHeader customHeader = {
        .httpPath = defaultHttpPath,
        .hostName = defaultHostName,
        .connectionType = defaultConnectionType,
        .otherHeader = defaultOtherHeader,
        .contentLength = 0};
    if (!createSocket("192.168.1.25"))
    {
        return;
    }
    httpPostPacket = httpPost(&customHeader, "sonuvabich");

    if (!sendHttpRequest(httpPostPacket))
    {
        xil_printf("send fail");
    }
    free(httpPostPacket);

    response = waitHttpResponse();
    close(sock);
    if (!response)
    {
        xil_printf("no response back");
        return;
    }
    xil_printf(response->statusCode);
    free(response);
}

void httpApplicationGet()
{
    char *httpGetPacket;
    httpResponse *response;
    httpHeader customHeader = {
        .httpPath = defaultHttpPath,
        .hostName = defaultHostName,
        .connectionType = defaultConnectionType,
        .otherHeader = defaultOtherHeader,
        .contentLength = 0};
    if (!createSocket("192.168.1.25"))
    {
        return;
    }
    httpGetPacket = httpGet(&customHeader);
    if (!sendHttpRequest(httpGetPacket))
    {
        xil_printf("send fail");
    }
    free(httpGetPacket);
    response = waitHttpResponse();
    close(sock);
    if (!response)
    {
        xil_printf("no response back");
        return;
    }
    xil_printf(response->statusCode);
    free(response);
}

int sendHttpRequest(char *packet)
{
    int totalSent = 0;
    int sent = 0;
    while (totalSent < strlen(packet))
    {
        sent = send(sock, packet + totalSent, strlen(packet) - totalSent, 0);
        if (sent == -1)
        {
            printf("Can't send headers");
            return 0;
        }
        totalSent += sent;
    }
    return 1;
}
httpResponse *waitHttpResponse()
{
    char *response;

    httpResponse *hresp = (struct httpResponse *)malloc(sizeof(httpResponse));
    if(!hresp)
    {
    	xil_printf("malloc error");
    	return NULL;
    }
    char BUF[1024];
    int recived_len = 0;
    int totalRecv = 0;
	do
	{
		recived_len = recv(sock, BUF + totalRecv, 1024 - totalRecv, 0);
		totalRecv += recived_len;
	} while (recived_len != 0);
    /* Reallocate response */
    response = (char *)malloc(totalRecv + 1);
    if(!response)
    {
    	xil_printf("malloc error");
    	return NULL;
    }
    memcpy(response, BUF, totalRecv);

    int offSet;
    char *status_line = get_until(response, "\r\n");
    offSet = strlen(status_line);

    char *headers = get_until(response + offSet, "\r\n\r\n");
    headers = str_replace("\r\n", "", headers);

    char *body = strstr(response, "\r\n\r\n");
    body = str_replace("\r\n\r\n", "", body);

    hresp->body = body;
    hresp->other = headers;
    hresp->statusCode = status_line;

    free(response);
    return hresp;
}
char *httpGet(httpHeader *customHeader)
{
    char *httpHeader = (char *)malloc(1024 * sizeof(char));
    if (!httpHeader)
    {
        xil_printf("cant allocate memory for http header\n");
        return NULL;
    }
    if (!checkHeader(customHeader))
    {
        return NULL;
    }

    if (customHeader->httpPath != NULL)
    {
        sprintf(httpHeader, "GET /%s HTTP/1.1\r\nHost:%s\r\nConnection:%s\r\n%s\r\n\r\n",
                customHeader->httpPath,
                customHeader->hostName,
                customHeader->connectionType,
                customHeader->otherHeader);
    }
    // reallocation new size for the http header to save memory
    httpHeader = realloc(httpHeader, strlen(httpHeader) + 1);

    return httpHeader;
}

char *httpPost(httpHeader *customHeader, char *body)
{
    char *httpHeader = (char *)malloc(1024 * sizeof(char));
    if (!httpHeader)
    {
        xil_printf("cant allocate memory for http header\n");
        return NULL;
    }
    if (!checkHeader(customHeader))
    {
        return NULL;
    }
    customHeader->contentLength = strlen(body);

    if (customHeader->httpPath != NULL)
    {
        sprintf(httpHeader, "POST /%s HTTP/1.1\r\nHost:%s\r\nConnection:%s\r\nContent-Length:%d\r\n%s\r\n\r\n",
                customHeader->httpPath,
                customHeader->hostName,
                customHeader->connectionType,
                customHeader->contentLength,
                customHeader->otherHeader);
    }
    if (body != NULL)
    {
        sprintf(httpHeader, "%s%s", httpHeader, body);
    }
    // reallocation new size for the http header to save memory
    httpHeader = realloc(httpHeader, strlen(httpHeader) + 1);

    return httpHeader;
}
int checkHeader(httpHeader *header)
{
    if (!header->httpPath)
    {
        return 0;
    }
    if (!header->hostName)
    {
        header->hostName = "";
    }
    if (!header->connectionType)
    {
        header->connectionType = "keep-alive"; // we leave keep alive as default
    }
    if (!header->otherHeader)
    {
        header->otherHeader = "";
    }
    return 1;
}
int createSocket(char *destination)
{

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Can't create TCP socket");
        return 0;
    }
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr(destination);
    remote.sin_port = htons(HTTP_SOCKET);

    if (connect(sock, (struct sockaddr *)&remote, sizeof(remote)) < 0)
    {
        printf("Could not connect");
        return 0;
    }

    return 1;
}
void closeSocket()
{
    close(sock);
}
