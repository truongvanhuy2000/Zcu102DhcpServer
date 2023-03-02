#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>
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
enum dhcp_msg_type
{
    DHCP_DISCOVER = 1,
    DHCP_OFFER = 2,
    DHCP_REQUEST = 3,
    DHCP_DECLINE = 4,
    DHCP_ACK = 5,
    DHCP_NAK = 6,
    DHCP_RELEASE = 7,
    DHCP_INFORM = 8,
};

enum
{

    /* RFC 1497 Vendor Extensions */

    PAD = 0,
    END = 255,

    SUBNET_MASK = 1,
    TIME_OFFSET = 2,
    ROUTER = 3,
    TIME_SERVER = 4,
    NAME_SERVER = 5,
    DOMAIN_NAME_SERVER = 6,
    LOG_SERVER = 7,
    COOKIE_SERVER = 8,
    LPR_SERVER = 9,
    IMPRESS_SERVER = 10,
    RESOURCE_LOCATION_SERVER = 11,
    HOST_NAME = 12,
    BOOT_FILE_SIZE = 13,
    MERIT_DUMP_FILE = 14,
    DOMAIN_NAME = 15,
    SWAP_SERVER = 16,
    ROOT_PATH = 17,
    EXTENSIONS_PATH = 18,

    /* IP Layer Parameters per Host */

    IP_FORWARDING = 19,
    NON_LOCAL_SOURCE_ROUTING = 20,
    POLICY_FILTER = 21,
    MAXIMUM_DATAGRAM_REASSEMBLY_SIZE = 22,
    DEFAULT_IP_TIME_TO_LIVE = 23,
    PATH_MTU_AGING_TIMEOUT = 24,
    PATH_MTU_PLATEAU_TABLE = 25,

    /* IP Layer Parameters per Interface */

    INTERFACE_MTU = 26,
    ALL_SUBNETS_ARE_LOCAL = 27,
    BROADCAST_ADDRESS = 28,
    PERFORM_MASK_DISCOVERY = 29,
    MASK_SUPPLIER = 30,
    PERFORM_ROUTER_DISCOVERY = 31,
    ROUTER_SOLICITATION_ADDRESS = 32,
    STATIC_ROUTE = 33,

    /* Link Layer Parameters per Interface */

    TRAILER_ENCAPSULATION = 34,
    ARP_CACHE_TIMEOUT = 35,
    ETHERNET_ENCAPSULATION = 36,

    /* TCP Parameters */

    TCP_DEFAULT_TTL = 37,
    TCP_KEEPALIVE_INTERVAL = 38,
    TCP_KEEPALIVE_GARBAGE = 39,

    /* Application and Service Parameters */

    NETWORK_INFORMATION_SERVICE_DOMAIN = 40,
    NETWORK_INFORMATION_SERVERS = 41,
    NETWORK_TIME_PROTOCOL_SERVERS = 42,
    VENDOR_SPECIFIC_INFORMATION = 43,
    NETBIOS_OVER_TCP_IP_NAME_SERVER = 44,
    NETBIOS_OVER_TCP_IP_DATAGRAM_DISTRIBUTION_SERVER = 4,
    NETBIOS_OVER_TCP_IP_NODE_TYPE = 46,
    NETBIOS_OVER_TCP_IP_SCOPE = 47,
    X_WINDOW_SYSTEM_FONT_SERVER = 48,
    X_WINDOW_SYSTEM_DISPLAY_MANAGER = 49,
    NETWORK_INFORMATION_SERVICE_PLUS_DOMAIN = 64,
    NETWORK_INFORMATION_SERVICE_PLUS_SERVERS = 65,
    MOBILE_IP_HOME_AGENT = 68,
    SMTP_SERVER = 69,
    POP3_SERVER = 70,
    NNTP_SERVER = 71,
    DEFAULT_WWW_SERVER = 72,
    DEFAULT_FINGER_SERVER = 73,
    DEFAULT_IRC_SERVER = 74,
    STREETTALK_SERVER = 75,
    STREETTALK_DIRECTORY_ASSISTANCE_SERVER = 76,

    /* DHCP Extensions */

    REQUESTED_IP_ADDRESS = 50,
    IP_ADDRESS_LEASE_TIME = 51,
    OPTION_OVERLOAD = 52,
    TFTP_SERVER_NAME = 66,
    BOOTFILE_NAME = 67,
    DHCP_MESSAGE_TYPE = 53,
    SERVER_IDENTIFIER = 54,
    PARAMETER_REQUEST_LIST = 55,
    MESSAGE = 56,
    MAXIMUM_DHCP_MESSAGE_SIZE = 57,
    RENEWAL_T1_TIME_VALUE = 58,
    REBINDING_T2_TIME_VALUE = 59,
    VENDOR_CLASS_IDENTIFIER = 60,
    CLIENT_IDENTIFIER = 61

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

    uint8_t magicCookie[4]; // use to identify the packet

    uint8_t options[308]; // optional parameters field
};

typedef struct dhcp_message dhcp_message;
struct dhcp_option
{
    uint8_t id;        // option id
    uint8_t len;       // option length
    uint8_t *data; // option data
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
/*
 * Internal representation of a DHCP message,
 * with options parsed into a list...
 */

#endif
