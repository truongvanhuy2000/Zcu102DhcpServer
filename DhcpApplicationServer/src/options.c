#include "options.h"
dhcp_option createOption(uint8_t optionID, uint8_t optionLen, uint8_t *optionData)
{
    dhcp_option newOption;
    newOption.id = optionID;
    newOption.len = optionLen;
    newOption.data = malloc(newOption.len + 1);
    if (newOption.data)
    {
        memcpy(newOption.data, optionData, newOption.len);
    }
    return newOption;
}
dhcp_option getLeaseTimeOption(uint32_t time)
{
    dhcp_option leaseTime;
    leaseTime.id = IP_ADDRESS_LEASE_TIME;
    leaseTime.len = 4;
    leaseTime.data = malloc(leaseTime.len + 1);
    if (leaseTime.data)
    {
        memcpy(leaseTime.data, &time, leaseTime.len);
    }
    return leaseTime;
}
dhcp_option getGatewayPara(uint8_t *gatewayAddr)
{
    dhcp_option newOption;
    newOption.id = ROUTER;
    newOption.len = 4;
    newOption.data = malloc(newOption.len + 1);
    if (newOption.data)
    {
        memcpy(newOption.data, gatewayAddr, newOption.len);
    }
    return newOption;
}
dhcp_option getSubnetPara(uint8_t *subnetMask)
{
    dhcp_option newOption;
    newOption.id = SUBNET_MASK;
    newOption.len = 4;
    newOption.data = malloc(newOption.len + 1);
    if (newOption.data)
    {
        memcpy(newOption.data, subnetMask, newOption.len);
    }
    return newOption;
}

void deleteOptionList(dhcp_option_list *dhcpOptionList)
{
    dhcp_option_list *ptr;
    dhcp_option_list *temp;

    ptr = dhcpOptionList;
    while (ptr != NULL)
    {
        temp = ptr->next_option;
        free(ptr->dhcp_option.data);
		free(ptr);
        ptr = temp;
    }
}
int parseDhcpOption(dhcp_option *option, uint8_t *optionPtr)
{
    int optionLen;
    option->id = *optionPtr;
    option->len = *(optionPtr + 1);
    option->data = malloc(option->len + 1);

    if(!option->data)
    {
    	return 0;
    }
    memcpy(option->data, optionPtr + 2, option->len);
    optionLen = option->len + 2;

    return optionLen;
}

int parseDhcpOptionList(dhcp_msg *msg)
{
    dhcp_option_list *optionListPtr;
    uint8_t *optionArrayPtr = msg->hdr.options;
    int nextOptionPos;

    msg->opts = malloc(sizeof(dhcp_option_list));
    if (!msg->opts)
    {
        return 0;
    }
    memset(msg->opts, 0, sizeof(dhcp_option_list));
    optionListPtr = msg->opts;

    while (*optionArrayPtr != END)
    {
        nextOptionPos = parseDhcpOption(&optionListPtr->dhcp_option, optionArrayPtr);
        optionListPtr->next_option = malloc(sizeof(dhcp_option_list));
        optionListPtr = optionListPtr->next_option;
        optionArrayPtr += nextOptionPos;
    }
    optionListPtr->next_option = NULL;
    return 1;
}

dhcp_option *seachForOption(dhcp_option_list *optionList, int optionCode)
{
    while (optionList != NULL)
    {
        if (optionList->dhcp_option.id == optionCode)
        {
            return &(optionList->dhcp_option);
        }
        optionList = optionList->next_option;
    }
    return NULL;
}
int appendOptionToArray(uint8_t *optionArrPtr, dhcp_option *option)
{
    int optionLen;
    optionArrPtr[0] = option->id;
    optionArrPtr[1] = option->len;
    memcpy(optionArrPtr + 2, option->data, option->len);
    optionLen = option->len + 2;
    return optionLen;
}

int appendOptionToList(dhcp_msg *msg, dhcp_option *option)
{
    dhcp_option_list *temp;
    dhcp_option_list *ptr = msg->opts;
    while (ptr != NULL)
    {
        temp = ptr;
        ptr = ptr->next_option;
    }
    temp->next_option = malloc(sizeof(dhcp_option_list));
    if (!temp->next_option)
    {
        return 0;
    }
    memset(temp->next_option, 0, sizeof(dhcp_option_list));
    temp->next_option->dhcp_option = *option;
    temp->next_option->next_option = NULL;
    return 1;
}
void serializeOptionList(dhcp_msg *reply)
{
    uint8_t *optionArrPtr = reply->hdr.options;
    dhcp_option_list *optionPtr = reply->opts;
    int optionLen;
    do
    {
        optionLen = appendOptionToArray(optionArrPtr, &(optionPtr->dhcp_option));
        optionArrPtr += optionLen;
        optionPtr = optionPtr->next_option;
    } while (optionPtr != NULL);

    *optionArrPtr = END;
}
