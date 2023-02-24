#ifndef HTTP_APPLICATION_
#define HTTP_APPLICATION_

#include "lwipopts.h"
#include "xlwipconfig.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "xil_printf.h"

#define HTTP_SOCKET 80

#define httpRespondOK "HTTP/1.1 200 OK\r\n"
#define defaultHttpPath "index.html"
#define defaultHostName "192.168.1.25"
#define defaultConnectionType "keep-alive"
#define defaultOtherHeader "Cache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nOrigin: http://192.168.0.100\r\nContent-Type: application/x-www-form-urlencoded\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nReferer: http://192.168.0.100/index.cgi\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: vi,en-GB;q=0.9,en;q=0.8"

// you need to fill this header before sending anything
// if you dont need any of this, remember to init it as ""
// dont put \r\n at the end of any of these
typedef struct httpHeader
{
    char *httpPath; // must have
    char *hostName;
    int contentLength; // dont have to init this, just give it a value 0
    char *connectionType;

    char *otherHeader; // you can expand this more if you want
} httpHeader;

typedef struct httpResponse
{
    char *statusCode;
    char *other;
    char *body;
} httpResponse;

#endif
