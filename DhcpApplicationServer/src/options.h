#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>
#include <stdio.h>
#include "dhcp.h"

/*
 * Code ID of DHCP and BOOTP options
 * as defined in RFC 2132
 */
void deleteOptionList(dhcp_option_list *dhcpOptionList);
int parseDhcpOption(dhcp_option *option, uint8_t *optionPtr);
int parseDhcpOptionList(dhcp_msg *msg);
dhcp_option *seachForOption(dhcp_option_list *optionList, int optionCode);
int appendOptionToArray(uint8_t *optionArrPtr, dhcp_option *option);
void serializeOptionList(dhcp_msg *reply);
int appendOptionToList(dhcp_msg *msg, dhcp_option *option);

dhcp_option createOption(uint8_t optionID, uint8_t optionLen, uint8_t *optionData);

dhcp_option getLeaseTimeOption(uint32_t time);
dhcp_option getGatewayPara(uint8_t *gatewayAddr);
dhcp_option getSubnetPara(uint8_t *subnetMask);

#endif
