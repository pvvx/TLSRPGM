/********************************************************************************************************
 * @file     usb_desc.c
 *
 * @brief    This file provides set of functions to manage the USB CDC descriptors
 *
 * @author   jian.zhang@telink-semi.com
 * @date     Oct. 8, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *           The information contained herein is confidential property of Telink 
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *           Co., Ltd. and the licensee or the terms described here-in. This heading 
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this 
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "common.h"
#ifdef USE_USB_CDC
#include "usb.h"

#ifndef USB_VID
#define USB_VID		0x248a
#endif
#ifndef USB_PID
#define USB_PID		0x8267
#endif
#ifndef USB_MAX_POWER
#define USB_MAX_POWER 	200 // mA
#endif


SECTION_USB_CONST
USB_Descriptor_String_t language_desc = {
    {sizeof(USB_Descriptor_Header_t) + 2, DTYPE_String},
    {LANGUAGE_ID_ENG}};

SECTION_USB_CONST
USB_Descriptor_String_t vendor_desc = {
    {sizeof(USB_Descriptor_Header_t) + sizeof(STRING_VENDOR) - 2, DTYPE_String}, // Header
    STRING_VENDOR};

SECTION_USB_CONST
USB_Descriptor_String_t product_desc = {
    {sizeof(USB_Descriptor_Header_t) + sizeof(STRING_PRODUCT) - 2, DTYPE_String}, // Header
    STRING_PRODUCT};

SECTION_USB_CONST
USB_Descriptor_String_t serial_desc = {
    {sizeof(USB_Descriptor_Header_t) + sizeof(STRING_SERIAL) - 2, DTYPE_String}, // Header
    STRING_SERIAL};

SECTION_USB_CONST
USB_Descriptor_Device_t device_desc = {
    { sizeof(USB_Descriptor_Device_t), DTYPE_Device }, // Header
    
    0x0110, // USBSpecification, USB 2.0
    0x02, // Class: Communications and CDC Control
    0x00, // SubClass
    0x00, // Protocol: No class specific protocol required
    8, // Endpoint0 Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
	USB_VID, // VendorID
	USB_PID, // Product ID
    0x0100, // .ReleaseNumber
    USB_STRING_VENDOR, // .ManufacturerStrIndex
    USB_STRING_PRODUCT, // .ProductStrIndex
	USB_STRING_SERIAL, //.SerialNumStrIndex, iSerialNumber
    1 // bNumConfigurations
};

SECTION_USB_CONST
USB_Descriptor_Configuration_t configuration_desc = {
    { 
    	// Configuration Descriptor
        {
            sizeof(USB_Descriptor_Configuration_Header_t),
            DTYPE_Configuration 
        }, // Length, type
        sizeof(USB_Descriptor_Configuration_t), // TotalLength: variable
        USB_INTF_MAX, // NumInterfaces: 2 Interfaces
        1, // Configuration index: Configuration 1
        NO_DESCRIPTOR, // Configuration String: No String Descriptor
        USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP,
		/* Attributes
         D7: Reserved, set 1     : 1
         D6: Self Powered        : 0 (no)
         D5: Remote Wakeup       : 1
         D4..0: Reserved, set 0  : 0 */
        USB_CONFIG_POWER_MA(USB_MAX_POWER) // MaxPower = 100mA
    },

    {
        // IAD Descriptor: iad0
        {sizeof(USB_Descriptor_Interface_Association_t), DTYPE_InterfaceAssociation}, // Header
        USB_INTF_CDC_CCI, // FirstInterfaceIndex
        2, // TotalInterface
        CDC_CSCP_CDCClass, // Class: Communications and CDC Control
        CDC_CSCP_ACMSubclass, // Subclass
        CDC_CSCP_ATCommandProtocol, // protocol
        NO_DESCRIPTOR  // IADStrIndex: No String Descriptor
    },

    {
        // Interface Descriptor: cdc_interface
        {sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
        USB_INTF_CDC_CCI, // InterfaceNumber
        0, // AlternateSetting
        1, // TotalEndpoints
        CDC_CSCP_CDCClass, // Class
        CDC_CSCP_ACMSubclass, // SubClass
        CDC_CSCP_ATCommandProtocol, // Protocol
        NO_DESCRIPTOR //InterfaceStrIndex
    },

    {
        // cdc_descriptor
        // CDC_Functional_Header
        {
            {sizeof(USB_CDC_Descriptor_FunctionalHeader_t), DTYPE_CSInterface}, // Header
            CDC_DSUBTYPE_CSInterface_Header, // Subtype: Header Functional Descriptor
            0x0110 // CDCSpecification: CDC Version 1.10
        },

        // CDC_CallManagement =
        {
            {sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, // Header
            CDC_DSUBTYPE_CSInterface_CallManagement, // Subtype: Call Management Functional Descriptor
            0, /* MasterInterfaceNumber
	           D7..2: 0x00 (Reserved)
	           D1   : 0x00 (sends/receives call management information only over the Communication Class interface)
	           D0   : 0x01 (handles call management itself) */
            USB_INTF_CDC_DCI, // SlaveInterfaceNumber
        },

		// CDC_Functional_ACM
        {
            {sizeof(USB_CDC_Descriptor_FunctionalACM_t), DTYPE_CSInterface}, // Header
            CDC_DSUBTYPE_CSInterface_ACM, // Subtype: Abstract Control Management Functional Descriptor
            0x02 /* Capabilities:
	           D7..4: 0x00 (Reserved)
	           D3   : 0x00 (not supports the notification Network_Connection)
	           D2   : 0x00 (not supports the request Send_Break)
	           D1   : 0x01 (supports the request combination of Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and the notification Serial_State)
	           D0   : 0x00 (not supports the request combination of Set_Comm_Feature, Clear_Comm_Feature, and Get_Comm_Feature) */
        },

		// CDC_Functional_Union
        {
            {sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, // Header
            CDC_DSUBTYPE_CSInterface_Union, // Subtype: Union Functional Descriptor
            USB_INTF_CDC_CCI, // MasterInterfaceNumber
            USB_INTF_CDC_DCI, // SlaveInterfaceNumber
        },

        // CDC_NotificationEndpoint
        {
            {sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
            (ENDPOINT_DIR_IN | CDC_NOTIFICATION_EPNUM), // EndpointAddress
            (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes
            CDC_NOTIFICATION_EPSIZE, // EndpointSize: MaxPacketSize : 8 bytes
            0xfa // PollingIntervalMS 250
        },

        // CDC_DCI_Interface
        {
            {sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, // Header
            USB_INTF_CDC_DCI, // InterfaceNumber
            0, // AlternateSetting
            2, // TotalEndpoints: 2 Endpoints
            CDC_CSCP_CDCDataClass, // Class: CDC-Data
            CDC_CSCP_NoDataSubclass, // SubClass
            CDC_CSCP_NoDataProtocol, // Protocol
            NO_DESCRIPTOR // InterfaceStrIndex: No String Descriptor
        },

        // CDC_DataOutEndpoint =
        {
            {sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, // Header
            (ENDPOINT_DIR_OUT | CDC_RX_EPNUM), // EndpointAddress
            (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes: TransferType=Bulk
            CDC_TXRX_EPSIZE, // EndpointSize
            0x00 // PollingIntervalMS: ignored
        },

        // CDC_DataInEndpoint
        {
            {sizeof(USB_Descriptor_Endpoint_t),  DTYPE_Endpoint}, // Header
            (ENDPOINT_DIR_IN | CDC_TX_EPNUM), // EndpointAddress
            (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), // Attributes: TransferType=Bulk
            CDC_TXRX_EPSIZE, // EndpointSize
            0x00 // PollingIntervalMS: ignored
        },
    },
};


#endif // USE_USB_CDC
