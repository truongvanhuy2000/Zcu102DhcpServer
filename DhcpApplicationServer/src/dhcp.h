#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>
#include "options.h"
enum ports
{
    BOOTPS = 67,
    BOOTPC = 68
};

enum op_types
{
    BOOTREQUEST = 1,
    BOOTREPLY = 2,
};

enum hardware_address_types
{
    ETHERNET = 0x01,
    ETHERNET_LEN = 0x06,
};

/* DHCP message */

enum
{
    DHCP_HEADER_SIZE = 236 // without size of options
};

struct dhcp_message
{
    uint8_t op;    // message op code, message type
    uint8_t htype; // hardware address type
    uint8_t hlen;  // hardware address length
    uint8_t hops;  // incremented by relay agents

    uint32_t xid; // transaction ID

    uint16_t secs;  // seconds since address acquisition or renewal
    uint16_t flags; // flags

    uint32_t ciaddr; // client IP address
    uint32_t yiaddr; // 'your' client IP address
    uint32_t siaddr; // IP address of the next server to use in bootstrap
    uint32_t giaddr; // relay agent IP address

    uint8_t chaddr[16]; // client hardware address

    uint8_t sname[64]; // server host name

    uint8_t file[128]; // boot file name

    uint8_t magicCookie[4]; //use to identify the packet

    uint8_t options[308]; // optional parameters field
};

typedef struct dhcp_message dhcp_message;

/*
 * Internal representation of a DHCP message,
 * with options parsed into a list...
 */
struct dhcp_option
{
    uint8_t id;        // option id
    uint8_t len;       // option length
    uint8_t data[256]; // option data
};
typedef struct dhcp_option dhcp_option;

struct dhcp_option_list_
{
    dhcp_option dhcp_option;
    struct dhcp_option_list_ *next_option;
};
typedef struct dhcp_option_list_ dhcp_option_list;

struct dhcp_msg
{
    dhcp_message hdr;
    dhcp_option_list *opts;
};

typedef struct dhcp_msg dhcp_msg;

#endif
