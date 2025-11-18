/********************************************************************************************************
 * @file     usb_desc.c
 *
 * @brief    This is the source file for TLSR8258
 *
 * @author	 Driver Group
 * @date     May 8, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
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
 * @par      History:
 * 			     1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/
#include "usb_desc.h"

#define USB8K	0x40,0x1f,0x00		//8k
#define USB16K	0x80,0x3e,0x00	 	//16k
#define USB22K	0x22,0x56,0x00  	//22.05k
#define USB32K	0x00,0x7d,0x00  	//32k
#define USB44K	0x44,0xac,0x00  	//44k
#define USB48K	0x80,0xbb,0x00  	//48k
#define USB96K	0x00,0x77,0x01		//96k

#define USB_RATE	USB16K

/**
 * @brief	Language string descriptor
 */
static const USB_Descriptor_String_t language_desc = { {
		sizeof(USB_Descriptor_Header_t) + 2, DTYPE_String
	},
	{ LANGUAGE_ID_ENG }
};

/**
 * @brief	Manufacturer string descriptor
 */
static const USB_Descriptor_String_t vendor_desc = { {
		sizeof(USB_Descriptor_Header_t)
		+ sizeof(USB_VENDOR_STRING) - 2, DTYPE_String
	}, // Header
	USB_VENDOR_STRING
};

/**
 * @brief	Product string descriptor
 */

static const USB_Descriptor_String_t product_desc = { {
		sizeof(USB_Descriptor_Header_t) + sizeof(USB_PRODUCT_STRING) - 2,
		DTYPE_String
	}, // Header
	USB_PRODUCT_STRING
};


/**
 * @brief	Serial number string descriptor
 */
static const USB_Descriptor_String_t serial_desc = { {
		sizeof(USB_Descriptor_Header_t)
		+ sizeof(USB_SERIAL_STRING) - 2, DTYPE_String
	}, // Header
	USB_SERIAL_STRING
};

/**
 * @brief	Device descriptor
 */
static const USB_Descriptor_Device_t device_desc = {
	{sizeof(USB_Descriptor_Device_t), DTYPE_Device}, 		// Header
	0x0110, 												// USBSpecification, USB 1.1
#if(USB_MODE_CDC_EN)
	CDC_CSCP_CDCClass,         								// bDeviceClass =0x02 -> communication device class
#else
	USB_CSCP_NoDeviceClass,
#endif
	USB_CSCP_NoDeviceSubclass, 								// SubClass
	USB_CSCP_NoDeviceProtocol, 								// Protocol
	8, 														// Endpoint0Size
	USB_VENDOR_ID,  										// VendorID
	USB_PRODUCT_ID, 										// ProductID
	USB_VERSION_ID, 										// ReleaseNumber
	USB_VENDOR_STRING_INDEX, 								// ManufacturerStrIndex
	USB_PRODUCT_STRING_INDEX,  								// ProductStrIndex
	USB_SERIAL_STRING_INDEX,   								// SerialNumStrIndex
	1   													// bNumConfigurations
};

#if(USB_MODE_MOUSE_EN)
/**
 * @brief	Mouse HID report descriptor
 */

static const USB_Descriptor_HIDReport_Datatype_t usb_mouse_report_desc[] = {

	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x02, // USAGE (Mouse)
	0xa1, 0x01, // COLLECTION (Application)
	0x09, 0x01, // USAGE (Pointer)
	0xa1, 0x00, // COLLECTION (Physical)
	0x05, 0x09, // USAGE_PAGE (Button)
	0x19, 0x01, // USAGE_MINIMUM (Button 1)
	0x29, 0x03, // USAGE_MAXIMUM (Button 3)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x01, // LOGICAL_MAXIMUM (1)
	0x95, 0x03, // REPORT_COUNT (3)
	0x75, 0x01, // REPORT_SIZE (1)
	0x81, 0x02, // INPUT (Data,Var,Abs)
	0x95, 0x01, // REPORT_COUNT (1)
	0x75, 0x05, // REPORT_SIZE (5)
	0x81, 0x03, // INPUT (Cnst,Var,Abs)
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x30, // USAGE (X)
	0x09, 0x31, // USAGE (Y)
	0x09, 0x38, // USAGE (Wheel)
	0x15, 0x81, // LOGICAL_MINIMUM (-127)
	0x25, 0x7f, // LOGICAL_MAXIMUM (127)
	0x75, 0x08, // REPORT_SIZE (8)
	0x95, 0x03, // REPORT_COUNT (3)
	0x81, 0x06, // INPUT (Data,Var,Rel)
	0xc0,       // END_COLLECTION
	0xc0        // END_COLLECTION
};

#endif

#if(USB_MODE_KEYBOARD_EN)
/**
 * @brief	Keyboard HID report descriptor
 */
static const USB_Descriptor_HIDReport_Datatype_t usb_keyboard_report_desc[] = {
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x06, // USAGE (Keyboard)
	0xa1, 0x01, // COLLECTION (Application)
	0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)
	0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0x01, // LOGICAL_MAXIMUM (1)
	0x95, 0x08, // REPORT_COUNT (8)
	0x75, 0x01, // REPORT_SIZE (1)
	0x81, 0x02, // INPUT (Data,Var,Abs)
	0x95, 0x01, // REPORT_COUNT (1)
	0x75, 0x08, // REPORT_SIZE (8)
	0x81, 0x03, // INPUT (Cnst,Var,Abs)
	0x95, 0x06, // REPORT_COUNT (6)
	0x75, 0x08, // REPORT_SIZE (8)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x25, 0xFF, // LOGICAL_MAXIMUM (255)
	0x05, 0x07, // USAGE_PAGE (Keyboard/Keypad)
	0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00, // INPUT (Data,Ary,Abs)
	0x25, 0x01, // LOGICAL_MAXIMUM (1)
	0x95, 0x05, // REPORT_COUNT (5)
	0x75, 0x01, // REPORT_SIZE (1)
	0x05, 0x08, // USAGE_PAGE (LEDs)
	0x19, 0x01, // USAGE_MINIMUM (Num Lock)
	0x29, 0x05, // USAGE_MAXIMUM (Kana)
	0x91, 0x02, // OUTPUT (Data,Var,Abs)
	0x95, 0x01, // REPORT_COUNT (1)
	0x75, 0x03, // REPORT_SIZE (3)
	0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
	0xc0        // END_COLLECTION
};
#endif
/**
 * @brief	Configuration descriptor
 */
static const TL_USB_Descriptor_Configuration_t
configuration_desc = {
	{
		// configuration descriptor
		{sizeof(USB_Descriptor_Configuration_Header_t), DTYPE_Configuration}, 	// Length, type
		sizeof(TL_USB_Descriptor_Configuration_t), 								// TotalLength: variable
		USB_INTF_MAX_ID, 														// TotalInterfaces
		1, 																		// ConfigurationNumber
		NO_DESCRIPTOR, 															// Configuration String
		USB_CONFIG_ATTR_RESERVED, 												// Attributes
		USB_CONFIG_POWER_MA(500) 												// MaxPower = 500mA
	},
#if(USB_MODE_PRINTER_EN)
    {
		// printer class: interface descriptor
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, 					// Header
		USB_INTF_PRINTER_ID, 													// InterfaceNumber
		0, 																		// AlternateSetting
		2, 																		// TotalEndpoints
		PRNT_CSCP_PrinterClass, 												// Class:printer interface class->0x07
		PRNT_CSCP_PrinterSubclass, 												// SubClass:printer subclass
		PRNT_CSCP_BidirectionalProtocol, 										// Protocol:
		NO_DESCRIPTOR 															// InterfaceStrIndex
    },
    {
		{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
		ENDPOINT_DIR_IN | USB_EDP_PRINTER_IN,
		EP_TYPE_BULK,
		0x0040,																// MaxPacketSize
		0x00 																	// Interval
    },
    {
		{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
		ENDPOINT_DIR_OUT | USB_EDP_PRINTER_OUT,
		EP_TYPE_BULK,
		0x0040,																// MaxPacketSize
		0x00 																	// Interval
    },
#endif

#if(USB_MODE_CDC_EN)
	{
		// CDC class: interface descriptor
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, 					// Header
		USB_INTF_CDC_CCI, 														// InterfaceNumber
		0, 																		// AlternateSetting
		1, 																		// TotalEndpoints
		CDC_CSCP_CDCClass, 														// Class:CDC interface class->0x02
		CDC_CSCP_ACMSubclass, 													// SubClass:Abstract Control->0x02
		CDC_CSCP_ATCommandProtocol, 											// Protocol:common AT commands protocol->0x01
		NO_DESCRIPTOR 															// InterfaceStrIndex
	},
	{
		/* CDC class: functional descriptor (class-specific interface descriptor)  */
		// Header Functional Descriptor
		{
			{sizeof(USB_CDC_Descriptor_FunctionalHeader_t), DTYPE_CSInterface}, // Header:CS Interface
			CDC_DSUBTYPE_CSInterface_Header, 									// Subtype:Header Functional Descriptor->0x00
			0x0110 																// USB protocol version:USB1.1
		},
		// Abstract Control Management Functional Descriptor
		{
			{sizeof(USB_CDC_Descriptor_FunctionalACM_t), DTYPE_CSInterface}, 	// Header:CS Interface
			CDC_DSUBTYPE_CSInterface_ACM, 										// Subtype:ACM Functional Descriptor
			0x02 																// bmCapabilities:
			/* <7:4>:reserved ->0
			 * <3>:1-support,0-Not support; Nerwork_Connection
			 * <2>:1-support,0-Not support; Send_Break
			 * <1>:1-support,0-Not support; Set_Line_Coding,Set_Contorl_Line_State,Get_Line_Coding,Serial_State
			 * <0>:1-support,0-Not support; Set_Comm_Feature,Clear_Comm_Feature,Get_Comm_Feature
			 * */
		},
		// Union Functional Descriptor
		{
			{sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, 	// Header
			CDC_DSUBTYPE_CSInterface_Union, 									// Subtype
			0, 																	// MasterInterfaceNumber
			1, 																	// SlaveInterfaceNumber
		},
		// call Management Functional Descriptor
		{
			{sizeof(USB_CDC_Descriptor_FunctionalUnion_t), DTYPE_CSInterface}, 	// Header
			CDC_DSUBTYPE_CSInterface_CallManagement, 							// Subtype
			0, 																	// bmCapabilities
			1, 																	// bDataInterface
		},
		/* Endpoint Descriptor : Interface 0 - CDC class Interface */
		// Interrupt Input Endpoint 1 : manage all device
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, 				// Header
			(ENDPOINT_DIR_IN | USB_EDP_CDC_INFO_IN), 						// EndpointAddress
			(EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), 	// Attributes
			USB_CDC_NOTIFICATION_EPSIZE, 											// EndpointSize
			0x40 																// PollingIntervalMS
		},

		/* Interface Descriptor :interface 1 - data class Interface */
		// data class interface: serial data transmit
		{
			{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface}, 				// Header
			USB_INTF_CDC_DCI, 													// InterfaceNumber
			0, 																	// AlternateSetting
			2, 																	// TotalEndpoints
			CDC_CSCP_CDCDataClass, 												// Class
			CDC_CSCP_NoDataSubclass, 											// SubClass
			CDC_CSCP_NoDataProtocol, 											// Protocol
			NO_DESCRIPTOR 														// InterfaceStrIndex
		},
		/* Endpoint Descriptor :interface 1 - data class Interface */
		// Bulk Output Endpoint 5 Descriptor
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint}, 				// Header
			(ENDPOINT_DIR_OUT | USB_EDP_CDC_OUT), 									// EndpointAddress
			(EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), 		// Attributes
			USB_CDC_TX_RX_EPSIZE, 													// EndpointSize
			0x00 																// PollingIntervalMS
		},
		// Bulk Input Endpoint 4 Descriptor
		{
			{sizeof(USB_Descriptor_Endpoint_t),  DTYPE_Endpoint}, 				// Header
			(ENDPOINT_DIR_IN | USB_EDP_CDC_IN), 									// EndpointAddress
			(EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA), 		// Attributes
			USB_CDC_TX_RX_EPSIZE, 													// EndpointSize
			0x00 																// PollingIntervalMS
		},
	},
#endif
#if(USB_MODE_KEYBOARD_EN)
	/* 1.Interface Descriptor(Keyboard) */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},
		USB_INTF_KEYBOARD_ID, // InterfaceNumber
		0, // AlternateSetting
		2, // bNumEndpoints
		HID_CSCP_HIDClass, HID_CSCP_BootSubclass,
		HID_CSCP_KeyboardBootProtocol, NO_DESCRIPTOR
	},
	{
		/* 2.HID Descriptor(Keyboard) */
		{
			{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID}, 0x0110, // HIDSpec
			USB_HID_COUNTRY_US, 1, // TotalReportDescriptors
			HID_DTYPE_Report, {sizeof(usb_keyboard_report_desc)&0xFF, (sizeof(usb_keyboard_report_desc)>>8)&0xFF}, // HIDReportLength
		},
		/* 3.EndPoint Descriptor(Keyboard INPUT) */
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
			ENDPOINT_DIR_IN | USB_EDP_KEYBOARD_IN,
			EP_TYPE_INTERRUPT, 0x0008, // EndpointSize
			USB_KEYBOARD_POLL_INTERVAL // PollingInterval0MS
		},
		/* 3.EndPoint Descriptor(Keyboard OUTPUT) */
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},
			ENDPOINT_DIR_OUT | USB_EDP_KEYBOARD_OUT,
			EP_TYPE_INTERRUPT, 0x0008, // EndpointSize
			USB_KEYBOARD_POLL_INTERVAL // PollingInterval0MS
		}
	},
#endif

#if(USB_MODE_MOUSE_EN)
	/* 1.Interface Descriptor(Mouse) */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},						// bLength:0x09, bDescriptorType:0x04
	    USB_INTF_MOUSE_ID, 															// bInterfaceNumber :	0x00
		0, 																			// bAlternateSetting:	0x00
		1, 																			// bNumEndpoints	:	0x01
		HID_CSCP_HIDClass,															// bInterfaceClass	:	0x03 (HID Class:0x03)
		HID_CSCP_BootSubclass,														// bInterfaceSubClass:	0x01 (ONLY SUPPORT BIOS SubClass IN HID 1.1 PROTOCOL)
		HID_CSCP_MouseBootProtocol,													// bInterfaceProtocol:	0x02(Mouse:0x02,keyboard:0x01 in BIOS SubClass)
		NO_DESCRIPTOR																// iConfiguration:		0x00(None)
	},
	{
		/* 2.HID Descriptor(Mouse) */
		{
			{sizeof(USB_HID_Descriptor_HID_t), HID_DTYPE_HID},						// bLength:0x09, bDescriptorType:0x21 (HID Descriptor Code:0x21)
			0x0110, 																// bcdHID:0x10,0x01(HID 1.1 Protocol)
			USB_HID_COUNTRY_US,														// bCountyCode:0x21
			1, 																		// bNumDescriptors:0x01
			HID_DTYPE_Report,														// bDescriptorType:0x22
			{sizeof(usb_mouse_report_desc), 0x00}, 									// bDescriptorLength:0x0034
		},
		/* 3.EndPoint Descriptor(Mouse)*/
		{
			{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},					// bLength:0x07, bDescriptorType:0x05
			ENDPOINT_DIR_IN | USB_EDP_MOUSE,										// bEndpointAddress:	0x81
			EP_TYPE_INTERRUPT,  													// bmAttributes:		0x03(Transmit Type:Interrupt Transmit)
			0x0008,																	// wMaxPacketSize:		0x0008
			USB_MOUSE_POLL_INTERVAL 											    // bInterval:			0x04
		}
	},
#endif

#if(USB_MODE_AUDIO_EN)
	/* 1.STD Interface Descriptor(Audio) */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},						// bLength:0x09, bDescriptorType:0x04
		USB_INTF_AUDIO_ID, 															// bInterfaceNumber :	0x00
		0, 																			// bAlternateSetting:	0x00
		0, 																			// bNumEndpoints	:	0
		AUDIO_CSCP_AudioClass,														// bInterfaceClass	:	audio
		AUDIO_CSCP_ControlSubclass,													// bInterfaceSubClass:	audio control
		AUDIO_CSCP_ControlProtocol,													// bInterfaceProtocol:	audio control protocol
		NO_DESCRIPTOR																// iConfiguration:		0x00(None)
	},
	/* 2.Class-specific Interface Descriptor(Audio)  */

	#if(USB_MODE_SPEAKER_EN&&USB_MODE_MICPHONE_EN)
		{
			{sizeof(USB_Audio_Descriptor_Interface_AC_TL_t), DTYPE_CSInterface},		// bLength:0x0a, DescriptorType:0x24
			AUDIO_DSUBTYPE_CSInterface_Header,											// bSubtype: audio control header
			{0x00,0x01}, 																	// bACSpecification[2] :audio Device Class v1.00
			{0x34,0x00},																	// bTotalLength: l-byte	, h-byte
			/*			(sizeof(USB_Audio_Descriptor_Interface_AC_TL_t)\
			 			+	sizeof(USB_Audio_Descriptor_InputTerminal_t)\
			 			+	sizeof(USB_Audio_Descriptor_OutputTerminal_t)\
			 			+	sizeof(USB_Audio_Descriptor_InputTerminal_t)\
			 			+	sizeof(USB_Audio_Descriptor_OutputTerminal_t)), */
			0x02, 																		// InCollection: 2 AudioStreaming interface(MIC/Speaker)
			USB_INTF_MIC_ID, 															// InterfaceNumber_speaker: Microphone AS id: 2
			USB_INTF_SPEAKER_ID  														// InterfaceNumber_mic:     Speaker AS id: 3
		},
	#else

		#if(USB_MODE_MICPHONE_EN)
			{
				{sizeof(USB_Audio_Descriptor_Interface_AC_t), DTYPE_CSInterface},			// bLength:0x09, DescriptorType:0x24
				AUDIO_DSUBTYPE_CSInterface_Header,											// bSubtype: audio control header
				{0x00,0x01}, 																	// bACSpecification[2] :audio Device Class v1.00
				{0x1e,0x00},																	// bTotalLength: variable
				0x01, 																		// InCollection: 1 AudioStreaming interface(MIC)
				USB_INTF_MIC_ID, 																		// InterfaceNumber_mic: Microphone AS id: 2
			},
		#endif

		#if(USB_MODE_SPEAKER_EN)
			{
				{sizeof(USB_Audio_Descriptor_Interface_AC_t), DTYPE_CSInterface},			// bLength:0x09, DescriptorType:0x24
				AUDIO_DSUBTYPE_CSInterface_Header,											// bSubtype: audio control header
				{0x00,0x01}, 																	// bACSpecification[2] :audio Device Class v1.00
				{0x1e,0x00},																	// bTotalLength: variable
				0x01, 																		// InCollection: 1 AudioStreaming interface(Speaker)
				USB_INTF_SPEAKER_ID, 														// InterfaceNumber_speaker: Speaker AS id: 3
			},
		#endif
	#endif
#endif
	#if(USB_MODE_MICPHONE_EN)
	/* 1.MIC_INPUT_TERMINAL */
	{
		{sizeof(USB_Audio_Descriptor_InputTerminal_t), DTYPE_CSInterface},				// bLength: 0x0c, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_InputTerminal,										// DescriptorSubType:Input Terminal
		0x01,																			// TerminalID:0x01
		AUDIO_TERMINAL_IN_MIC,															// TerminalType:USB Microphone
		0x00,																			// AssocTerminal:
		0x01,																			// NrChannels:mono 1 channel
		0x0000,																			// ChannelConfig:Left Front,Right Front
		0x00,																			// ChannelName String
		0x00,																	 		// Terminal String
	},
	/* 2.MIC_OUTPUT_TERMINAL */
	{
		{sizeof(USB_Audio_Descriptor_OutputTerminal_t), DTYPE_CSInterface},				// bLength: 0x09, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_OutputTerminal,										// DescriptorSubType:Input Terminal
		0x03,																			// TerminalID:0x03
		AUDIO_TERMINAL_STREAMING,														// TerminalType:USB Streaming
		0x00,																			// AssocTerminal:ID 0
		0x01,																			// SourceID:1 from input terminal
		0x00,																 			// Terminal String
	},

	#endif

	#if(USB_MODE_SPEAKER_EN)
	/* 1.SPEAKER_INPUT_TERMINAL */
	{
		{sizeof(USB_Audio_Descriptor_InputTerminal_t), DTYPE_CSInterface},				// bLength: 0x0c, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_InputTerminal,										// DescriptorSubType:Input Terminal
		0x04,																			// TerminalID:0x01
		AUDIO_TERMINAL_STREAMING,																		// TerminalType:USB Streaming
		0x00,																			// AssocTerminal:
		0x01,																			// NrChannels:mono 1 channel
		0x0000,																		// ChannelConfig:Left Front,Right Front
		0x00,																			// ChannelName String
		0x00,																	 		// Terminal String
	},
	/* 2.SPEAKER_OUTPUT_TERMINAL */
	{
		{sizeof(USB_Audio_Descriptor_OutputTerminal_t), DTYPE_CSInterface},				// bLength: 0x09, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_OutputTerminal,										// DescriptorSubType:Input Terminal
		0x06,																			// TerminalID:0x06
		AUDIO_TERMINAL_OUT_SPEAKER,																		// TerminalType:USB speaker
		0x00,																			// AssocTerminal:ID 0
		0x04,																			// SourceID:4  SPEAKER_INPUT_TERMINAL
		0x00,																 			// Terminal String
	},
	#endif

	#if(USB_MODE_MICPHONE_EN)
	/* 1.MIC_STD_IF_SET0 */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},							// bLength: 0x09, DescriptorType:Interface
		USB_INTF_MIC_ID,																// InterfaceNum(id):
		0x00,																			// AlternateSetting:0
		0x00,																			// NumEndpoint:0
		0x01,																			// InterfaceClass:audio
		0x02,																			// InterfaceSubClass:audio streaming
		0x00,																			// InterfaceProtocol
		0x00																			// Interface String
	},
	/* 2.MIC_STD_IF_SET1 */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},							// bLength: 0x09, DescriptorType:Interface
		USB_INTF_MIC_ID,																// InterfaceNum(id):
		0x01,																			// AlternateSetting:1
		0x01,																			// NumEndpoint:1
		0x01,																			// InterfaceClass:audio
		0x02,																			// InterfaceSubClass:audio streaming
		0x00,																			// InterfaceProtocol
		0x00																			// Interface String
	},
	/* 3.MIC_AS_GENERAL */
	{
		{sizeof(USB_Audio_Descriptor_Interface_AS_t), DTYPE_CSInterface},				// bLength: 0x07, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_General,												// DescriptorSubType:AS_GENERAL
		0x03,																			// TerminalLink:#3USB USB Streaming OT
		0x01,																			// Delay:1
		{0x01,0x00}																		// FormatTag:PCM
	},
	/* 4.MIC_AS_FORMAT_TYPE */
	{
		{sizeof(USB_Audio_Descriptor_Format_t)+3, DTYPE_CSInterface},						// bLength: 0x0b, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_FormatType,  										// DescriptorSubType:Format_type
		0x01,  																			// FormatType:Format type 1
		0x01,  																			// NumberOfChanne:1
		0x02,  																			// SubframeSize:2byte
		0x10,  																			// BitsResolution:16bit
		0x01,  																			// SampleFreqType:One sampling frequency.
	},
	/* 5.USB sampling rate */
	{
		USB_RATE																		// Sample rate
	},
	/* 6.MIC_ENDPOINT */
	{
		{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},							// bLength: 0x07, DescriptorType:audio interface descriptor
		ENDPOINT_DIR_IN|USB_EDP_MIC,													// EndpointAddress:USB_EDP_SPEAKER
		EP_TYPE_ISOCHRONOUS,															// Attributes:0x01,Isochronous
		0x0100,																			// MaxPacketSize: 256
		0x01																			// Interval
	},
	/* 7.MIC_AUDIO_ENDPOINT */
	{
		{sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t), DTYPE_CSEndpoint},			// bLength: 0x07, DescriptorType:audio endpoint descriptor
		AUDIO_DSUBTYPE_CSEndpoint_General,												// DescriptorSubType:audio endpiont general
		0x00,																			// Attributes:0x00
		0x00,																			// LockDelayUnits
		{0x00,0x00}																		// LockDelay
	},
	#endif

	#if(USB_MODE_SPEAKER_EN)
	/* 1.SPK_STD_IF_SET0 */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},							// bLength: 0x09, DescriptorType:Interface
		USB_INTF_SPEAKER_ID,															// InterfaceNum(id):
		0x00,																			// AlternateSetting:0
		0x00,																			// NumEndpoint:0
		0x01,																			// InterfaceClass:audio
		0x02,																			// InterfaceSubClass:audio streaming
		0x00,																			// InterfaceProtocol
		0x00																			// Interface String
	},
	/* 2.SPK_STD_IF_SET1 */
	{
		{sizeof(USB_Descriptor_Interface_t), DTYPE_Interface},							// bLength: 0x09, DescriptorType:Interface
		USB_INTF_SPEAKER_ID,																// InterfaceNum(id):
		0x01,																			// AlternateSetting:1
		0x01,																			// NumEndpoint:1
		0x01,																			// InterfaceClass:audio
		0x02,																			// InterfaceSubClass:audio streaming
		0x00,																			// InterfaceProtocol
		0x00																			// Interface String
	},
	/* 3.SPK_AS_GENERAL */
	{
		{sizeof(USB_Audio_Descriptor_Interface_AS_t), DTYPE_CSInterface},				// bLength: 0x07, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_General,												// DescriptorSubType:AS_GENERAL
		0x04,																			// TerminalLink:#4 USB Streaming IT
		0x01,																			// Delay:1
		{0x01,0x00}																		// FormatTag:PCM																			// Interface String
	},
	/* 4.SPK_AS_FORMAT_TYPE */
	{
		{sizeof(USB_Audio_Descriptor_Format_t)+3, DTYPE_CSInterface},						// bLength: 0x0b, DescriptorType:audio interface descriptor
		AUDIO_DSUBTYPE_CSInterface_FormatType,  										// DescriptorSubType:Format_type
		0x01,  																			// FormatType:Format type 1
		0x02,  																			// NumberOfChanne:2
		0x02,  																			// SubframeSize:2byte
		0x10,  																			// BitsResolution:16bit
		0x01,  																			// SampleFreqType:One sampling frequency.
	},
	/* 5.USB sampling rate */
	{
		USB_RATE																		// Sample rate
	},
	/* 6.SPK_ENDPOINT */
	{
		{sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint},							// bLength: 0x07, DescriptorType:endpoint descriptor
		ENDPOINT_DIR_OUT|USB_EDP_SPEAKER,												// EndpointAddress:USB_EDP_SPEAKER
		0x09,																			// Attributes:0x09,Isochronous,Synchronization Type(Asynchronous)
		0x0100,																		// MaxPacketSize: 256
		0x01																			// Interval
	},
	/* 7.SPK_AUDIO_ENDPOINT */
	{
		{sizeof(USB_Audio_Descriptor_StreamEndpoint_Spc_t), DTYPE_CSEndpoint},			// bLength: 0x07, DescriptorType:audio endpoint descriptor
		AUDIO_DSUBTYPE_CSEndpoint_General,												// DescriptorSubType:audio endpiont general
		0x00,																			// Attributes:0x00
		0x00,																			// LockDelayUnits
		{0x00,0x00}																		// LockDelay
	},
	#endif
};




/**
 * @brief		This function serves to return the address of language descriptor
 * @param[in] 	none
 * @return 		The address of language descriptor
 */
unsigned char *usb_desc_get_language(void) {
	return (unsigned char *)(&language_desc);
}

/**
 * @brief		This function serves to return the address of vendor descriptor
 * @param[in] 	none
 * @return 		The address of vendor descriptor
 */
unsigned char *usb_desc_get_vendor(void) {
	return (unsigned char *)(&vendor_desc);
}

/**
 * @brief		This function serves to return the address of product descriptor
 * @param[in] 	none
 * @return 		The address of product descriptor
 */
unsigned char *usb_desc_get_product(void) {
	return (unsigned char *)(&product_desc);
}
/**
 * @brief		This function serves to return the address of serial number descriptor
 * @param[in] 	none
 * @return 		The address of serial number descriptor
 */
unsigned char *usb_desc_get_serial(void) {
	return (unsigned char *)(&serial_desc);
}

/**
 * @brief		This function serves to return the address of device descriptor
 * @param[in] 	none
 * @return 		The address of device descriptor
 */
unsigned char *usb_desc_get_device(void) {
	return (unsigned char *)(&device_desc);
}

/**
 * @brief		This function serves to return the address of configuration descriptor
 * @param[in] 	none
 * @return 		The address of configuration descriptor
 */
unsigned char *usb_desc_get_configuration(void) {
	return (unsigned char *)(&configuration_desc);
}

#if(USB_MODE_CDC_EN)
/**
 * @brief		This function serves to return the address of CDC configuration descriptor
 * @param[in] 	none
 * @return 		The address of CDC configuration descriptor
 */
unsigned char* usb_desc_get_cdc(void) {
	return (unsigned char*) (&configuration_desc.cdc_descriptor);
}

/**
 * @brief		This function serves to return the address of interface descriptor
 * @param[in] 	none
 * @return 		The address of interface descriptor
 */
unsigned char* usb_desc_get_cdc_inf(void) {
	return (unsigned char*) (&configuration_desc.cdc_interface);
}
#endif


#if(USB_MODE_KEYBOARD_EN)
/**
 * @brief		This function serves to return the address of Keyboard HID report descriptor
 * @param[in] 	none
 * @return 		The address of Keyboard HID report descriptor
 */
unsigned char* usb_keyboard_get_report_desc(void) {
	return (unsigned char*) (usb_keyboard_report_desc);
}

/**
 * @brief		This function serves to return the length of Keyboard HID report descriptor
 * @param[in] 	none
 * @return 		The length of Keyboard HID report descriptor
 */
unsigned short usb_keyboard_get_report_desc_size(void) {
	return sizeof(usb_keyboard_report_desc);
}

/**
 * @brief		This function serves to return the address of Keyboard configuration descriptor
 * @param[in] 	none
 * @return 		The address of Keyboard configuration descriptor
 */
unsigned char* usb_desc_get_keyboard(void) {
	return (unsigned char*) (&configuration_desc.keyboard_descriptor);
}

#endif


#if(USB_MODE_MOUSE_EN)

/**
 * @brief		This function serves to return the address of Mouse HID report descriptor
 * @param[in] 	none
 * @return 		The address of Mouse HID report descriptor
 */
unsigned char* usb_mouse_get_report_desc(void) {
	return (unsigned char*) (usb_mouse_report_desc);
}

/**
 * @brief		This function serves to return the length of Mouse HID report descriptor
 * @param[in] 	none
 * @return 		The length of Mouse HID report descriptor
 */
unsigned short usb_mouse_get_report_desc_size(void) {
	return sizeof(usb_mouse_report_desc);
}

/**
 * @brief		This function serves to return the address of Mouse configuration descriptor
 * @param[in] 	none
 * @return 		The address of Mouse configuration descriptor
 */
unsigned char* usb_desc_get_mouse(void) {
	return (unsigned char*) (&configuration_desc.mouse_descriptor);
}

#endif
