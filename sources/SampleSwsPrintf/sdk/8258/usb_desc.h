/********************************************************************************************************
 * @file     usb_desc.h
 *
 * @brief    This is the header file for TLSR8258
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
 * 			 1.initial release(DEC. 26 2018)
 *
 * @version  A001
 *
 *******************************************************************************************************/

#ifndef USB_DESC_H_
#define USB_DESC_H_

#include "./usbstd/usbstd.h"

// #define DOXYGEN_EN  

#ifdef DOXYGEN_EN
#define USB_MODE_CDC_EN					1     	//by defined in complier setting
#define USB_MODE_MOUSE_EN				1		//by defined in complier setting
#define USB_MODE_KEYBOARD_EN			1		//by defined in complier setting
#define USB_MODE_SPEAKER_EN				1		//by defined in complier setting
#define USB_MODE_MICPHONE_EN			1		//by defined in complier setting
#endif
//#define USB_MODE_MASS_STORAGE_EN		1		//undefined
//#define USB_MODE_SOMATIC_EN			1		//undefined
#define USB_MODE_PRINTER_EN				0

#if(USB_MODE_SPEAKER_EN||USB_MODE_MICPHONE_EN)
	#define USB_MODE_AUDIO_EN				1
#endif


#if(USB_MODE_CDC_EN)
#define USB_VENDOR_STRING				L"Telink Semi-conductor Ltd, Co"
#define USB_PRODUCT_STRING				L"Telink CDC"
#define USB_SERIAL_STRING				L"CDC demo "
#endif

#if(USB_MODE_MOUSE_EN)
#define USB_VENDOR_STRING				L"Telink Semi-conductor Ltd, Co"
#define USB_PRODUCT_STRING				L"Telink Mouse"
#define USB_SERIAL_STRING				L"Mouse demo"
#endif

#if(USB_MODE_KEYBOARD_EN)
#define USB_VENDOR_STRING				L"Telink Semi-conductor Ltd, Co"
#define USB_PRODUCT_STRING				L"Telink Keyboard"
#define USB_SERIAL_STRING				L"Keyboard demo"
#endif

#if(USB_MODE_AUDIO_EN)
#define USB_VENDOR_STRING				L"Telink Semi-conductor Ltd, Co"
#define USB_PRODUCT_STRING				L"Telink Audio16"
#define USB_SERIAL_STRING				L"Audio16 demo"
#endif

#if((!USB_MODE_AUDIO_EN)&&(!USB_MODE_KEYBOARD_EN)&&(!USB_MODE_MOUSE_EN)&&(!USB_MODE_CDC_EN))
#define USB_VENDOR_STRING				L"Telink Semi-conductor Ltd, Co"
#define USB_PRODUCT_STRING				L"Telink No Product"
#define USB_SERIAL_STRING				L"USB demo"
#endif

#define USB_VENDOR_ID					0x248a
#define USB_PRODUCT_ID					0x8004
#define USB_VERSION_ID					0x0100


/**
 *  @brief  Define the Number of EndPoint for application
 *  		The default length of hw buffer is as follows.
 *  		EDP1/2: 8 bytes
 *  		EDP3/6/7: 16 bytes
 *  		EDP4/5/8: 64 bytes
 */

enum {
	USB_EDP_KEYBOARD_IN 	= 1,
	USB_EDP_MOUSE 			= 2,	USB_EDP_CDC_INFO_IN  	= 2,
	USB_EDP_KEYBOARD_OUT 	= 3,
	USB_EDP_AUDIO_IN 		= 4,	USB_EDP_CDC_IN 			= 4,						USB_EDP_SOMATIC_IN  = 4,
									USB_EDP_CDC_OUT 		= 5,	USB_EDP_MS_OUT = 5,	USB_EDP_SOMATIC_OUT = 5,
	USB_EDP_SPEAKER 		= 6,
	USB_EDP_PRINTER_OUT 	= 7,	USB_EDP_MIC 			= 7,
	USB_EDP_PRINTER_IN 		= 8, 									USB_EDP_MS_IN  = 8,
};

/**
 * @brief	String Descriptor: Language/Vendor/Product/Serial number
 */
enum {
	USB_LANGUAGE_STRING_INDEX 	= 0,
	USB_VENDOR_STRING_INDEX		= 1,
	USB_PRODUCT_STRING_INDEX	= 2,
	USB_SERIAL_STRING_INDEX		= 3,
};

#if(USB_MODE_CDC_EN)

/** Size in bytes of the CDC device-to-host notification IN endpoint. */
#define 	USB_CDC_NOTIFICATION_EPSIZE        	8
/** Size in bytes of the CDC data IN and OUT endpoints. */
#define 	USB_CDC_TX_RX_EPSIZE               	64
#endif

#if(USB_MODE_KEYBOARD_EN)
#define 	USB_KEYBOARD_POLL_INTERVAL		   	10
#endif

#if(USB_MODE_MOUSE_EN)
#define 	USB_MOUSE_POLL_INTERVAL				4
#endif
/**
 * @brief	CDC/Keyboard interface Descriptor
 */
enum {
#if(USB_MODE_CDC_EN)
	USB_INTF_CDC_CCI,
	USB_INTF_CDC_DCI,
#endif

#if(USB_MODE_KEYBOARD_EN)
	USB_INTF_KEYBOARD_ID,
#endif

#if(USB_MODE_MOUSE_EN)
	USB_INTF_MOUSE_ID,
#endif
#if(USB_MODE_PRINTER_EN)
	USB_INTF_PRINTER_ID,
#endif
#if(USB_MODE_MICPHONE_EN||USB_MODE_SPEAKER_EN)
	USB_INTF_AUDIO_ID,
#endif
#if(USB_MODE_MICPHONE_EN)
	USB_INTF_MIC_ID,
#endif
#if(USB_MODE_SPEAKER_EN)
	USB_INTF_SPEAKER_ID,
#endif
	USB_INTF_MAX_ID,
};



/**
 * @brief	CDC Configuration Descriptor definition
 */
typedef struct {
    USB_Descriptor_Configuration_Header_t Config;		//9 bytes
#if(USB_MODE_PRINTER_EN)
    USB_Descriptor_Interface_t  printer_interface;		//9 bytes
    USB_Descriptor_Endpoint_t printer_in_edp;			//7 bytes
    USB_Descriptor_Endpoint_t printer_out_edp;			//7 bytes
#endif

#if(USB_MODE_CDC_EN)
    USB_Descriptor_Interface_t  cdc_interface;
    USB_CDC_Descriptor_t  cdc_descriptor;
#endif

#if(USB_MODE_KEYBOARD_EN)
	USB_Descriptor_Interface_t keyboard_interface;
	USB_HID_Descriptor_HID_Keyboard_t keyboard_descriptor;
#endif

#if(USB_MODE_MOUSE_EN)
	USB_Descriptor_Interface_t mouse_interface;
	USB_HID_Descriptor_HID_Mouse_t mouse_descriptor;
#endif

#if(USB_MODE_AUDIO_EN)
	USB_Descriptor_Interface_t audio_interface;			//9 bytes

#if(USB_MODE_SPEAKER_EN&&USB_MODE_MICPHONE_EN)
	USB_Audio_Descriptor_Interface_AC_TL_t audio_ac_tl;
#else
	USB_Audio_Descriptor_Interface_AC_t audio_ac;		//9 bytes
#endif
#endif

#if(USB_MODE_MICPHONE_EN)
	USB_Audio_Descriptor_InputTerminal_t audio_mic_input_terminal;	//12 bytes
	USB_Audio_Descriptor_OutputTerminal_t audio_mic_output_terminal;//9 bytes
#endif

#if(USB_MODE_SPEAKER_EN)
	USB_Audio_Descriptor_InputTerminal_t audio_spk_input_terminal;
	USB_Audio_Descriptor_OutputTerminal_t audio_spk_output_terminal;
#endif

#if(USB_MODE_MICPHONE_EN)
	USB_Descriptor_Interface_t audio_mic_interface_set0;	//9 bytes
	USB_Descriptor_Interface_t audio_mic_interface_set1;	//9 bytes
	USB_Audio_Descriptor_Interface_AS_t audio_mic_as;		//7 bytes
	USB_Audio_Descriptor_Format_t audio_mic_format;			//8 bytes
	USB_Audio_SampleFreq_t audio_mic_rate;					//3 bytes
	USB_Descriptor_Endpoint_t audio_mic_edp;				//7 bytes
	USB_Audio_Descriptor_StreamEndpoint_Spc_t audio_mic_Stream_EDP_Spc;//7 bytes
#endif

#if(USB_MODE_SPEAKER_EN)
	USB_Descriptor_Interface_t audio_spk_interface_set0;
	USB_Descriptor_Interface_t audio_spk_interface_set1;
	USB_Audio_Descriptor_Interface_AS_t audio_spk_as;
	USB_Audio_Descriptor_Format_t audio_spk_format;
	USB_Audio_SampleFreq_t audio_spk_rate;					//3 bytes
	USB_Descriptor_Endpoint_t audio_spk_edp;
	USB_Audio_Descriptor_StreamEndpoint_Spc_t audio_spk_Stream_EDP_Spc;
#endif

} TL_USB_Descriptor_Configuration_t;


/**
 * @brief		This function serves to return the address of language descriptor
 * @param[in] 	none
 * @return 		The address of language descriptor
 */
unsigned char *usb_desc_get_language(void);


/**
 * @brief		This function serves to return the address of vendor descriptor
 * @param[in] 	none
 * @return 		The address of vendor descriptor
 */
unsigned char *usb_desc_get_vendor(void);

/**
 * @brief		This function serves to return the address of product descriptor
 * @param[in] 	none
 * @return 		The address of product descriptor
 */
unsigned char *usb_desc_get_product(void);

/**
 * @brief		This function serves to return the address of serial number descriptor
 * @param[in] 	none
 * @return 		The address of serial number descriptor
 */
unsigned char *usb_desc_get_serial(void);

/**
 * @brief		This function serves to return the address of device descriptor
 * @param[in] 	none
 * @return 		The address of device descriptor
 */
unsigned char *usb_desc_get_device(void);

/**
 * @brief		This function serves to return the address of configuration descriptor
 * @param[in] 	none
 * @return 		The address of configuration descriptor
 */
unsigned char *usb_desc_get_configuration(void);

#if(USB_MODE_CDC_EN)
/**
 * @brief		This function serves to return the address of CDC configuration descriptor
 * @param[in] 	none
 * @return 		The address of CDC configuration descriptor
 */
unsigned char* usb_desc_get_cdc(void);

/**
 * @brief		This function serves to return the address of interface descriptor
 * @param[in] 	none
 * @return 		The address of interface descriptor
 */
unsigned char* usb_desc_get_cdc_inf(void);

#endif

#if(USB_MODE_KEYBOARD_EN)
/**
 * @brief		This function serves to return the address of Keyboard HID report descriptor
 * @param[in] 	none
 * @return 		The address of Keyboard HID report descriptor
 */
unsigned char* usb_keyboard_get_report_desc(void);

/**
 * @brief		This function serves to return the length of Keyboard HID report descriptor
 * @param[in] 	none
 * @return 		The length of Keyboard HID report descriptor
 */
unsigned short usb_keyboard_get_report_desc_size(void);

/**
 * @brief		This function serves to return the address of Keyboard configuration descriptor
 * @param[in] 	none
 * @return 		The address of Keyboard configuration descriptor
 */
unsigned char* usb_desc_get_keyboard(void);

#endif

#if(USB_MODE_MOUSE_EN)

/**
 * @brief		This function serves to return the address of Mouse HID report descriptor
 * @param[in] 	none
 * @return 		The address of Mouse HID report descriptor
 */
unsigned char* usb_mouse_get_report_desc(void);

/**
 * @brief		This function serves to return the length of Mouse HID report descriptor
 * @param[in] 	none
 * @return 		The length of Mouse HID report descriptor
 */
unsigned short usb_mouse_get_report_desc_size(void);

/**
 * @brief		This function serves to return the address of Mouse configuration descriptor
 * @param[in] 	none
 * @return 		The address of Mouse configuration descriptor
 */
unsigned char* usb_desc_get_mouse(void);

#endif


#endif /* USB_DESC_H_ */
