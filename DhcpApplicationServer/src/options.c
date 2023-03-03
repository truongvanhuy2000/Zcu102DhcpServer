#include "options.h"

//This function is used for creating new option for dhcp packet
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

//used for release the allocated memory for dhcp option linked list
void deleteOptionList(dhcp_option_list *dhcpOptionList)
{
    dhcp_option_list *ptr;
    dhcp_option_list *temp;

    ptr = dhcpOptionList;
    while (ptr != NULL)
    {
        temp = ptr->next_option;
        if (ptr->dhcp_option.data)
        {
            free(ptr->dhcp_option.data);
        }
        free(ptr);
        ptr = temp;
    }
}

//is used for parse the indivisual option
int parseDhcpOption(dhcp_option *option, uint8_t *optionPtr)
{
    int optionLen;
    option->id = *optionPtr;
    option->len = *(optionPtr + 1);
    option->data = malloc(option->len + 1);

    if (!option->data)
    {
        return 0;
    }
    memcpy(option->data, optionPtr + 2, option->len);
    optionLen = option->len + 2;

    return optionLen;
}

//used to parse the serialized dhcp option into linked list
int parseDhcpOptionList(dhcp_msg *msg)
{
    dhcp_option_list *optionListPtr;
    dhcp_option_list *optionListTempPtr;
    uint8_t *optionArrayPtr = msg->hdr.options;
    int optionLen;
    if (*optionArrayPtr == END)
    {
        return 0;
    }
    msg->opts = malloc(sizeof(dhcp_option_list));
    optionListPtr = msg->opts;
    while (1)
    {
        optionLen = parseDhcpOption(&optionListPtr->dhcp_option, optionArrayPtr);
        optionListPtr->next_option = NULL;
        optionArrayPtr += optionLen;
        if(*optionArrayPtr == END)
        {
        	break;
        }
        optionListTempPtr = optionListPtr;
        optionListPtr = optionListPtr->next_option;
        optionListPtr = malloc(sizeof(dhcp_option_list));
    	if (!optionListPtr)
		{
			return 0;
		}
        optionListTempPtr->next_option = optionListPtr;

    }
    return 1;
}

//used to seach for a specific option using its id
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

//append the option to the an array
int appendOptionToArray(uint8_t *optionArrPtr, dhcp_option *option)
{
    int optionLen;
    optionArrPtr[0] = option->id;
    optionArrPtr[1] = option->len;
    memcpy(optionArrPtr + 2, option->data, option->len);
    optionLen = option->len + 2;
    return optionLen;
}

//append the option to the last of the linked list
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

//serialize the data before sending
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
