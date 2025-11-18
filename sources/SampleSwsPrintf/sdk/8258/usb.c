/********************************************************************************************************
 * @file     usb.c
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

#include "usb.h"

static USB_Request_Header_t Ctrl_Request;
static unsigned char *usb_response = 0;
static unsigned short usb_response_len = 0;
static unsigned char usb_g_stall = 0;

unsigned char usb_g_config = 0;
unsigned char usb_has_pdwn_irq = 0;

#if(USB_MODE_CDC_EN)
unsigned char usb_cdc_data[USB_CDC_TX_RX_EPSIZE];
unsigned short usb_cdc_data_len;
unsigned int usb_cdc_tx_cnt;
/**
 * @brief		Line Coding Structure is as follows:
 * 				Offset | Field       | Size | Value  | Description
 * 				0      | dwDTERate   |   4  | Number | Data terminal rate, in bits per second
 * 				4      | bCharFormat |   1  | Number | Stop bits: 0-1/1-1.5/2-2.
 * 				5      | bParityType |   1  | Number | Parity:0-None/1-Odd/2-Even/3-Mark/4-Space
 * 				6      | bDataBits   |   1  | Number | Data bits (5, 6, 7, 8 or 16).
 */

unsigned char LineCoding[7]={0x00,0xC2,0x01,0x00,0x00,0x00,0x08};
#endif

#if(USB_MODE_KEYBOARD_EN)
static unsigned char usb_k_rate = 0;
static unsigned char usb_k_protocol = 0;	//< 1: report protocol ;0: start protocol.
unsigned int usb_keyboard_tx_cnt;
#endif

#if(USB_MODE_MOUSE_EN)
static unsigned char usb_m_rate = 0;
static unsigned char usb_m_protocol = 0;	//< 1: report protocol ;0: start protocol.
#endif

#if(USB_MODE_MICPHONE_EN||USB_MODE_SPEAKER_EN)
unsigned char usb_audio_mic_cnt=0;
unsigned char usb_audio_speaker_cnt=0;
#endif


/**
 * @brief     This function servers to set USB Input.
 * @param[in] none
 * @return    none.
 */

void audio_rx_data_from_usb(void)
{

	unsigned char len = reg_usb_ep6_ptr;
	usbhw_reset_ep_ptr(USB_EDP_SPEAKER);

	signed short data;

	for (int i=0; i<len; i+=2)
	{
		if(i%4)
		{
			data = reg_usb_ep6_dat & 0xff;
			data |= reg_usb_ep6_dat << 8;
			reg_usb_mic_dat0 = data;
		}
		else
		{
			data = reg_usb_ep6_dat & 0xff;
			data |= reg_usb_ep6_dat << 8;
			reg_usb_mic_dat1 = data;
		}
	}
	reg_usb_ep6_ctrl = FLD_USB_EP_BUSY;
}

/**
 * @brief     This function serves to send data to USB.
 * @param[in] Input_Type - audio input type.
 * @param[in] Audio_Rate - audio rate.
 * @return    none.
 */

void audio_tx_data_to_usb(AudioInput_Typedef Input_Type,AudioRate_Typedef Audio_Rate)
{

    int i=0;
    unsigned short data_h=0;
    unsigned short data_l=0;

    unsigned char length = 0;

    usbhw_reset_ep_ptr(USB_EDP_MIC);

    switch(Audio_Rate)
    {
    case 	AUDIO_8K:		length = 8;break;
    case	AUDIO_16K:		length = 16;break;
    case	AUDIO_32K:		length = 32;break;
    default:				length = 32;break;
    }

    if(Input_Type==DMIC)
    {
		for (i=0; i<length; i++) {
			if(reg_dfifo_irq_status & FLD_AUD_DFIFO0_L_IRQ)
				break;
			if(i%2)
			{
				data_h = reg_usb_mic_dat1;
				reg_usb_ep7_dat = data_h;
				reg_usb_ep7_dat = data_h>>8;
			}
			else{
				data_l = reg_usb_mic_dat0;
				reg_usb_ep7_dat = data_l;
				reg_usb_ep7_dat = data_l>>8;
			}
		}
    }
    else
    {
		for (i=0;i<length; i++)
		{
			if(reg_dfifo_irq_status & FLD_AUD_DFIFO0_L_IRQ)
			{
				if(i%2) {
					reg_usb_ep7_dat = data_l;
					reg_usb_ep7_dat = data_l>>8;
				}
				else{
					reg_usb_ep7_dat = data_h;
					reg_usb_ep7_dat = data_h>>8;
				}
			}
			else
			{
				if(i%2)
				{
					data_l = reg_usb_mic_dat1;
					reg_usb_ep7_dat = data_l;
					reg_usb_ep7_dat = data_l>>8;
				}
				else
				{
					data_h = reg_usb_mic_dat0;
					reg_usb_ep7_dat = data_h;
					reg_usb_ep7_dat = data_h>>8;
				}
			}
		}
    }
	reg_usb_ep7_ctrl = FLD_USB_EP_BUSY;
}

/**
 * @brief		This function serves to enable manual interrupt
 * @param[in] 	none
 * @return 		none
 */
void usb_irq_init(void) {
	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD |
			                      FLD_CTRL_EP_AUTO_DESC |
			                      FLD_CTRL_EP_AUTO_CFG
			                      );
}

/**
 * @brief		This function serves to send response to USB host
 * @param[in] 	none
 * @return 		none
 */
void usb_tx_response(void) {
	unsigned short n;
	if(usb_response_len < 8) {
		n = usb_response_len;
	}
	else {
		n = 8;
	}

	usb_response_len -= n;
	usbhw_reset_ctrl_ep_ptr();

	while(n-- > 0) {
		usbhw_write_ctrl_ep_data(*usb_response);
		printf("0x%1x ",*usb_response);
		++usb_response;
	}
	printf(" \r\n");
}

/**
 * @brief		This function serves to handle interrupt request
 * @param[in] 	request_type - DATA_REQ and SETUP_REQ
 * @return 		none
 */
void usb_handle_request_process(unsigned char request_type) {
	unsigned char bmRequestType = Ctrl_Request.bmRequestType;
	unsigned char bRequest = Ctrl_Request.bRequest;
	unsigned char value_l = (Ctrl_Request.wValue) & 0xff;
    unsigned char value_h = (Ctrl_Request.wValue >> 8) & 0xff;
#if(USB_MODE_CDC_EN|USB_MODE_MOUSE_EN|USB_MODE_KEYBOARD_EN)
    unsigned char index_l = (Ctrl_Request.wIndex) & 0xff;
#endif
    //Step1: reset EDP0 rx buffer pointer
	usbhw_reset_ctrl_ep_ptr();

	//Step2: if request type is input request. and then
	if((bmRequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_DEVICETOHOST){						//bmRequestType<7> = 1
		// bmRequestType<6:5>: sub type of input request
		// 0b00: standard request
		// 0b01: class request
		// 0b10: vendor request
		switch(bmRequestType & CONTROL_REQTYPE_TYPE)
		{
			case (REQTYPE_STANDARD):						//##1.standard request
			{
 	 	 	 	if(USB_EDP_SETUP_IRQ_REQ == request_type)
 	 	 	 		printf("##1.USB Standard Input Request£º\r\n");

				switch(bRequest){
					case (REQ_GetConfiguration):			//1-1.get configuration
                        if(USB_EDP_SETUP_IRQ_REQ == request_type)
                        	printf("--1-1.get configuration \r\n");
						break;
				    case (REQ_GetDescriptor):				//1-2.get descriptor and return descriptor info. to host
		            	if(USB_EDP_SETUP_IRQ_REQ == request_type)
		            	{
		            		printf("--1-2.get descriptor \r\n");

		            		usb_response = 0;
							usb_response_len = 0;

							switch(value_h){

								case DTYPE_Device: 			//1-2-1. get device descriptor
									printf("++1-2-1.device descriptor \r\n");
									usb_response = usb_desc_get_device();
									usb_response_len = sizeof(USB_Descriptor_Device_t);
									break;
								case DTYPE_Configuration:   //1-2-2. get configuration descriptor
									printf("++1-2-2.configuration descriptor \r\n");
									usb_response = usb_desc_get_configuration();
									usb_response_len = sizeof(TL_USB_Descriptor_Configuration_t);
									break;
								case DTYPE_String:			//1-2-3. get string descriptor
									printf("++1-2-3.string descriptor \r\n");

									switch(value_l)
									{
										case USB_STRING_LANGUAGE:  //1-2-3-1. get language string
											printf("**1-2-3-1. Language string \r\n");
											usb_response = usb_desc_get_language();
											usb_response_len = sizeof(LANGUAGE_ID_ENG);
											break;

										case USB_STRING_VENDOR:  //1-2-3-2. get vendor string
											printf("**1-2-3-2. Vendor String \r\n");
											usb_response = usb_desc_get_vendor();
											usb_response_len = sizeof(USB_VENDOR_STRING);
											break;

										case USB_STRING_PRODUCT:  //1-2-3-2. get product string
											printf("**1-2-3-3. Product String \r\n");
											usb_response = usb_desc_get_product();
											usb_response_len = sizeof(USB_PRODUCT_STRING);
											break;

										case USB_STRING_SERIAL:  //1-2-3-2. get product serial number string
											printf("**1-2-3-4. Product Serial Number String \r\n");
											usb_response = usb_desc_get_serial();
											usb_response_len = sizeof(USB_SERIAL_STRING);
											break;

										default :
											printf("**1-2-3-4. Unknown String \r\n");
											usb_g_stall = 1;
											break;
									}
									break;
#if(USB_MODE_KEYBOARD_EN)
								//For HID device(Mouse/Keyboard/joystick )
								case HID_DTYPE_HID:			//1-2-4. get HID Descriptor
								{
									printf("++1-2-4.HID descriptor \r\n");
									if (index_l == USB_INTF_KEYBOARD_ID) 				//index_l is the interface number
									{
										printf("**1-2-4-1.HID descriptor: keyboard \r\n");	//keyboard
										usb_response = usb_desc_get_keyboard();
										usb_response_len = sizeof(USB_HID_Descriptor_HID_Keyboard_t);
									}
									break;
								}
								case HID_DTYPE_Report:		//1-2-5. get Report Descriptor
								{
									printf("++1-2-5.Report descriptor \r\n");
									if (index_l == USB_INTF_KEYBOARD_ID) {
										printf("**1-2-5-1.Report descriptor: keyboard \r\n");//keyboard
										usb_response = (unsigned char*) usb_keyboard_get_report_desc();
										usb_response_len = usb_keyboard_get_report_desc_size();
									}
									break;
								}
								case 0x23:					//1-2-6. physical descriptor
								{
									printf("++1-2-6.Physical descriptor \r\n");
									printf("**1-2-6-1.TODO \r\n");
									break;
								}
#endif
#if(USB_MODE_MOUSE_EN)
								//For HID device(Mouse/Keyboard/joystick )
								case HID_DTYPE_HID:			//1-2-4. get HID Descriptor
								{
									printf("++1-2-4.HID descriptor \r\n");
									if (index_l == USB_INTF_MOUSE_ID) 				//index_l is the interface number
									{
										printf("**1-2-4-1.HID descriptor: Mouse \r\n");	//mouse
										usb_response = usb_desc_get_mouse();
										usb_response_len = sizeof(USB_HID_Descriptor_HID_Mouse_t);
									}
									break;
								}
								case HID_DTYPE_Report:		//1-2-5. get Report Descriptor
								{
									printf("++1-2-5.Report descriptor \r\n");
									if (index_l == USB_INTF_MOUSE_ID) {
										printf("**1-2-5-1.Report descriptor: Mouse \r\n");//mouse
										usb_response = (unsigned char*) usb_mouse_get_report_desc();
										usb_response_len = usb_mouse_get_report_desc_size();
									}
									break;
								}
								case 0x23:					//1-2-6. physical descriptor (optional)
								{
									printf("++1-2-6.Physical descriptor \r\n");
									printf("**1-2-6-1.TODO \r\n");
									break;
								}
#endif
								default:
									printf("++1-2-7.Other descriptors,code: \r\n");
									printf("0x%1x ",(Ctrl_Request.wValue>>8)&0xFF);
									printf("\r\n");
									usb_g_stall = 1;
									break;
							}

							if(Ctrl_Request.wLength < usb_response_len) {
								usb_response_len = Ctrl_Request.wLength;
								printf("usb_response_len = %d\n",usb_response_len);
							}
		            	}

						usb_tx_response();	// return descriptor info. to host device
						printf("--1-2.return descriptor to host \r\n");
				    	break;

					case (REQ_GetStatus):		//1-3. get status
						printf("--1-3.get status \r\n");
						break;

					case (REQ_GetInterface):	//1-4. get interface
						printf("--1-4.get interface \r\n");
						break;

					case (REQ_SynchFrame):		//1-5. get synchFrame
						printf("--1-5.get sycnFrame \r\n");
						break;

					default:					//1-6. undefined request
						printf("--1-6.undefined request \r\n");
						usb_g_stall = 1;
						break;
				}
			    break;
			}
			case (REQTYPE_CLASS): 				//##2.class request
			{
				if(USB_EDP_SETUP_IRQ_REQ == request_type){
					printf("##2.USB Class Input Request \r\n");
					//CDC special input request
					switch (bRequest){
#if(USB_MODE_CDC_EN)
						case CDC_REQ_GetLineEncoding:
							printf("--2-1.GET_LINE_CODING¡£\r\n");
							usb_response = LineCoding;
							usb_response_len = sizeof(LineCoding);
							break;

						case CDC_NOTIF_SerialState:
							printf("--2-2.SERIAL_STATE¡£\r\n");
							break;
#endif
#if(USB_MODE_KEYBOARD_EN)
						//HID special input request: 3 input request and 3 output request
						case HID_REQ_GetReport:		//-- 2-1. get report request
							//This request is mandatory and must be supported by all devices.

							if(USB_EDP_SETUP_IRQ_REQ == request_type){
								printf("-- 2-1.get report request \r\n");
								switch(value_h){
								//value_h:Report Type
								//value_l:Report ID(value_l = 0, if not define Report ID)
									case 1://input
										printf("-- 2-1-1.report type: input(todo) \r\n");
										break;
									case 2://output
										printf("-- 2-1-2.report type: output(todo) \r\n");
										break;
									case 3://feature
										printf("-- 2-1-3.report type: feature(todo) \r\n");
										break;

									default:
										usb_g_stall = 1;
										break;
								}
							}
							//send report to USB host
							printf("-- 2-1.return report request(todo) \r\n");
							break;
						case HID_REQ_GetIdle:		//-- 2-2. get idle time request
							if(USB_EDP_SETUP_IRQ_REQ == request_type){
								printf("-- 2-2.return idle time request \r\n");
								usbhw_write_ctrl_ep_data(usb_k_rate);//send idle time to USB host(data period)
							}
							break;
						case HID_REQ_GetProtocol:  //-- 2-3. get protocol request
							//This request is required only for boot devices.
							if(USB_EDP_SETUP_IRQ_REQ == request_type){
								printf("-- 2-3.return protocol request \r\n");
								//usb_k_protocol:0 start protocol
								//usb_k_protocol:1 report protocol
								usbhw_write_ctrl_ep_data(usb_k_protocol);//send data to USB host(data period)
							}
							break;
#endif

#if(USB_MODE_MOUSE_EN)
						//HID special input request: 3 input request and 3 output request
						case HID_REQ_GetReport:		//-- 2-1. get report request
							//This request is mandatory and must be supported by all devices.

							if(USB_EDP_SETUP_IRQ_REQ == request_type){
								printf("-- 2-1.get report request \r\n");
								switch(value_h){
								//value_h:Report Type
								//value_l:Report ID(value_l = 0, if not define Report ID)
									case 1://input
										printf("-- 2-1-1.report type: input(todo) \r\n");
										break;
									case 2://output
										printf("-- 2-1-2.report type: output(todo) \r\n");
										break;
									case 3://feature
										printf("-- 2-1-3.report type: feature(todo) \r\n");
										break;
									default:
										usb_g_stall = 1;
										break;
								}
							}
							//send report to USB host
							printf("-- 2-1.return report request(todo) \r\n");
							break;
						case HID_REQ_GetIdle:		//-- 2-2. get idle time request
							if(USB_EDP_SETUP_IRQ_REQ == request_type){
								printf("-- 2-2.return idle time request \r\n");
								usbhw_write_ctrl_ep_data(usb_m_rate);//send idle time to USB host(data period)
							}
							break;
						case HID_REQ_GetProtocol:  //-- 2-3. get protocol request
							//This request is required only for boot devices.
							if(USB_EDP_SETUP_IRQ_REQ == request_type){
								printf("-- 2-3.return protocol request \r\n");
								//usb_m_protocol:0 start protocol
								//usb_m_protocol:1 report protocol
								usbhw_write_ctrl_ep_data(usb_m_protocol);//send data to USB host(data period)
							}
							break;
#endif
						default:
							printf("--2-3.unknown request \r\n");
							usb_g_stall = 1;
							break;
					}
					if(Ctrl_Request.wLength < usb_response_len) {
						usb_response_len = Ctrl_Request.wLength;
						printf("usb_response_len = %d\n",usb_response_len);
					}
				}
				usb_tx_response();
		        break;
			}
			case (REQTYPE_VENDOR): //##3.vendor request
				printf("##3.USB Vendor Input Request \r\n");
			    break;
			default:
				printf("##4.USB Not defined Input Request \r\n");
				usb_g_stall = 1;
			    break;
		}
	}
	//Step2: otherwise, request type is output request. and then
	else if((bmRequestType & CONTROL_REQTYPE_DIRECTION) == REQDIR_HOSTTODEVICE)
	{
		// bmRequestType<6:5>: sub type of output request
		// 0b00: standard request
		// 0b01: class request
		// 0b10: vendor request
		switch(bmRequestType&CONTROL_REQTYPE_TYPE)
		{
			case REQTYPE_STANDARD:  //##1.standard output request
				printf("##1.USB Standard Output Request£º\r\n");
				switch(bRequest)
				{
					case REQ_ClearFeature: 			//--1-1. Clear Feature
						printf("--1-1. Clear Feature \r\n");
					    break;
					case REQ_SetAddress:  			//--1-2. set address
						// Telink USB Device can automatically ack set-address request
						break;
					case REQ_SetConfiguration: 		//--1-3. set configuration
						printf("--1-3. set configuration \r\n");
						//enable uEDP0,after configuring uEDP0
						if(Ctrl_Request.wValue&0xff){//configuring for uEDP0 if wValue != 0
							usb_g_config = 1;
						}
						break;

					case REQ_SetDescriptor: 		//--1-4. set descriptor
						printf("--1-4. set descriptor \r\n");
						break;

					case REQ_SetFeature: 			//--1-5. set feature
						printf("--1-5. set feature \r\n");
						break;

					case REQ_SetInterface: 			//--1-6. set interface
						printf("--1-6. set interface \r\n");
						break;printf("--1-6. set interface \r\n");

					default:
						printf("--1-7. undefined output request \r\n");
						usb_g_stall = 1;
						break;
				}
				break;

			case REQTYPE_CLASS:  //##2.class output request
				if(USB_EDP_SETUP_IRQ_REQ == request_type)
				{
					printf("##2.USB Class Output Request \r\n");
				}
				switch(bRequest)
				{
#if(USB_MODE_CDC_EN)
				//CDC special output request
					case CDC_REQ_SetControlLineState:				//--2-1. set flow control function
						if(USB_EDP_SETUP_IRQ_REQ == request_type)
							printf("--2-1.SET_CONTROL_LINE_STATE \r\n");
						//None data stage
						break;
					case CDC_REQ_SetLineEncoding:					//--2-2. set serial parameter:baudrate,check,stop and data
					{
						if(USB_EDP_SETUP_IRQ_REQ == request_type){
							printf("--2-2.SET_LINE_CODING:\r\n");
						    printf("wIndex = %1d , wLength = %1d \r\n",index_l, Ctrl_Request.wLength );
						}
						else
						{
							usbhw_reset_ctrl_ep_ptr();
							for(int i = 0; i < 7; ++i)
							{
								LineCoding[i] = usbhw_read_ctrl_ep_data();
							}
							//-todo- according to LineCoding from USB HOST, user can set baud rate, stop bits, parity, data bit, etc.
						}
						break;
					}
#endif
#if(USB_MODE_KEYBOARD_EN)
					//HID special output request: 3 input request and 3 output request
				    case HID_REQ_SetReport:
				    	if(USB_EDP_SETUP_IRQ_REQ == request_type){
							printf("-- 2-1.Set Report \r\n");
							switch(value_h){
							//value_h:Report Type
							//value_l:Report ID(value_l = 0, if not define Report ID)
								case 1://input
									printf("-- 2-1-1.report type: input(todo) \r\n");
									break;
								case 2://output
									printf("-- 2-1-2.report type: output(todo) \r\n");
									break;
								case 3://feature
									printf("-- 2-1-3.report type: feature(todo) \r\n");
									break;
								default:
									usb_g_stall = 1;
									break;
							}
				    	}
				    	//USB host send report to device(DATA PERIOD: Report)
				    	//todo
				    	break;

					case HID_REQ_SetIdle:
						if(USB_EDP_SETUP_IRQ_REQ == request_type){
							printf("-- 2-2.Set Idle Time \r\n");
					        //value_h: Idle time 4ms ~ 1020ms
							//value_l: Report ID
							usb_k_rate = value_h;
						}
						break;

					case HID_REQ_SetProtocol://This request is required only for boot devices.
						if (USB_EDP_SETUP_IRQ_REQ == request_type){

							printf("-- 2-3.Set Protocol \r\n");
							//usb_k_protocol:0  start Protocol
							//usb_k_protocol:1  report Protocol
							usb_k_protocol = value_l;
						}
					    break;
#endif

#if(USB_MODE_MOUSE_EN)
					//HID special output request: 3 input request and 3 output request
					case HID_REQ_SetReport:
						if(USB_EDP_SETUP_IRQ_REQ == request_type){
							printf("-- 2-1.Set Report \r\n");
							switch(value_h){
							//value_h:Report Type
							//value_l:Report ID(value_l = 0, if not define Report ID)
								case 1://input
									printf("-- 2-1-1.report type: input(todo) \r\n");
									break;
								case 2://output
									printf("-- 2-1-2.report type: output(todo) \r\n");
									break;
								case 3://feature
									printf("-- 2-1-3.report type: feature(todo) \r\n");
									break;
								default:
									usb_g_stall = 1;
									break;
							}
						}
						//USB host send report to device(DATA PERIOD: Report)
						//todo
						break;

					case HID_REQ_SetIdle:
						if(USB_EDP_SETUP_IRQ_REQ == request_type){
							printf("-- 2-2.Set Idle Time \r\n");
							//value_h: Idle time 4ms ~ 1020ms
							//value_l: Report ID
							usb_m_rate = value_h;
						}
						break;

					case HID_REQ_SetProtocol://This request is required only for boot devices.
						if (USB_EDP_SETUP_IRQ_REQ == request_type){

							printf("-- 2-3.Set Protocol \r\n");
							//usb_m_protocol:0  start Protocol
							//usb_m_protocol:1  report Protocol
							usb_m_protocol = value_l;
						}
						break;
#endif
					default:
						printf("-- 2-3.unknown request \r\n");
						usb_g_stall = 1;
						break;
				}

				break;

			case REQTYPE_VENDOR:  //##3.vendor output request
				printf("##3.USB Vendor Output Request \r\n");
				break;

			default:
				printf("##4.USB Not defined Output Request\r\n");
				usb_g_stall = 1;
				break;
		}
	}

	return;
}

/**
 * @brief		This function serves to handle SETUP process
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_ctl_ep_setup(void)
{
	usbhw_reset_ctrl_ep_ptr();
	Ctrl_Request.bmRequestType = usbhw_read_ctrl_ep_data();
	Ctrl_Request.bRequest = usbhw_read_ctrl_ep_data();
	Ctrl_Request.wValue = usbhw_read_ctrl_ep_u16();
	Ctrl_Request.wIndex = usbhw_read_ctrl_ep_u16();
	Ctrl_Request.wLength = usbhw_read_ctrl_ep_u16();

	printf("0x%1x ",Ctrl_Request.bmRequestType);
	printf("0x%1x ",Ctrl_Request.bRequest);
	printf("0x%1x ",Ctrl_Request.wValue&0xFF);
	printf("0x%1x ",(Ctrl_Request.wValue>>8)&0xFF);
	printf("0x%1x ",Ctrl_Request.wIndex&0xFF);
	printf("0x%1x ",(Ctrl_Request.wIndex>>8)&0xFF);
	printf("0x%1x ",Ctrl_Request.wLength&0xFF);
	printf("0x%1x ",(Ctrl_Request.wLength>>8)&0xFF);
	printf(" \r\n");

	usb_g_stall = 0;

	usb_handle_request_process(USB_EDP_SETUP_IRQ_REQ);

	if(usb_g_stall)
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
		printf("STALL! \r\n");
	}
	else
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
		printf("ACK! \r\n");
	}
}
/**
 * @brief		This function serves to handle DATA process
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_ctl_ep_data(void) {
	usbhw_reset_ctrl_ep_ptr();
	usb_g_stall = 0;
	usb_handle_request_process(USB_EDP_DATA_IRQ_REQ);

	if(usb_g_stall)
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
		printf("STALL! \r\n");
	}
	else
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
		printf("ACK! \r\n");
	}
}

/**
 * @brief		This function serves to handle STATUS process
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_ctl_ep_status(void) {
	if(usb_g_stall)
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_STALL);
		printf("STALL! \r\n");
	}
	else
	{
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_ACK);
		printf("ACK! \r\n");
	}
}

#if(USB_MODE_CDC_EN)
void usb_cdc_tx_data_to_host(unsigned char * data_ptr, unsigned short data_len)
{

	if(usbhw_is_ep_busy(USB_EDP_CDC_IN)){
		return;
	}

	if(data_len >(USB_CDC_TX_RX_EPSIZE - 1)) {
		data_len = USB_CDC_TX_RX_EPSIZE - 1;
	}

	usbhw_reset_ep_ptr(USB_EDP_CDC_IN);

	while(data_len-- > 0) {					//only send 63Byte£¬other Bytes will be sent by EDP4 data IRQ
		reg_usb_ep_dat(USB_EDP_CDC_IN) = (*data_ptr);
		printf("0x%1x ",*data_ptr);
		++data_ptr;
	}

	usbhw_data_ep_ack(USB_EDP_CDC_IN);
	printf(" ACK \r\n");
}

void usb_cdc_rx_data_from_host(unsigned char* rx_buff)
{
	//receive data from usb and send them to uart
	unsigned char rx_from_usbhost_len = reg_usb_ep_ptr(USB_EDP_CDC_OUT);
	usbhw_reset_ep_ptr(USB_EDP_CDC_OUT);
	printf(" data len:%2d \r\n", rx_from_usbhost_len);

	if(rx_from_usbhost_len > 0 && rx_from_usbhost_len <= USB_CDC_TX_RX_EPSIZE){

		printf("rx data from USB CDC: ");
		for(int i = 0; i < rx_from_usbhost_len; ++i)
		{
			rx_buff[i] = reg_usb_ep_dat(USB_EDP_CDC_OUT);
			printf("0x%1x ",rx_buff[i]);
		}
		usb_cdc_data_len = rx_from_usbhost_len&0xff;		//uart dma: first 4 bytes is the length of packet

		printf(" \r\n");
	}
}

void usb_cdc_irq_data_process(void)
{
	unsigned char irq = usbhw_get_eps_irq();

	if(irq & FLD_USB_EDP4_IRQ ){
		usbhw_clr_eps_irq(FLD_USB_EDP4_IRQ);
		usbhw_reset_ep_ptr(USB_EDP_CDC_IN);

		usb_cdc_tx_cnt++;
		printf("CDC DATA INPUT IRQ:%d ! \r\n",usb_cdc_tx_cnt); //USB HOST INPUT
	}
	//CDC host-to-device data Output IRQ
	if(irq & FLD_USB_EDP5_IRQ){
		usbhw_clr_eps_irq(FLD_USB_EDP5_IRQ);
		printf("CDC DATA OUTPUT IRQ! \r\n"); //USB HOST OUTPUT
		usb_g_stall = 0;

		//receive data from usb host
		usb_cdc_rx_data_from_host(usb_cdc_data);

		if(usb_g_stall)
		{
			usbhw_data_ep_stall(USB_EDP_CDC_OUT);
			printf(" STALL! \r\n");
		}
		else
		{
			usbhw_data_ep_ack(USB_EDP_CDC_OUT);
			printf(" ACK \r\n");
		}
	}
}
#endif

#if(USB_MODE_SPEAKER_EN||USB_MODE_MICPHONE_EN)

/**
 * @brief		This function serves to send datat to USB Host or receive data from USB Host
 * @param[in] 	none
 * @return 		none
 */
void usb_audio_irq_data_process(void)
{
	unsigned char irq = usbhw_get_eps_irq();
	if(irq & FLD_USB_EDP7_IRQ)
	{
		usbhw_clr_eps_irq(FLD_USB_EDP7_IRQ);
		usb_audio_mic_cnt++;
	}

	if(irq & FLD_USB_EDP6_IRQ)
	{
		usbhw_clr_eps_irq(FLD_USB_EDP6_IRQ);
		usb_audio_speaker_cnt++;
	}
}
#endif

#if(USB_MODE_KEYBOARD_EN)
void usb_keyboard_irq_data_process(void)
{
	unsigned char irq = usbhw_get_eps_irq();

	//Keyboard EDP IN IRQ: tx
	if(irq & FLD_USB_EDP1_IRQ){
		usbhw_clr_eps_irq(FLD_USB_EDP1_IRQ);
		usbhw_reset_ep_ptr(USB_EDP_KEYBOARD_IN);

		usb_keyboard_tx_cnt++;

		printf("KEYBOARD DATA INPUT IRQ:%d !\n",usb_keyboard_tx_cnt);
	}
	//Keyboard EDP Output IRQ: rx
	if(irq & FLD_USB_EDP3_IRQ){
		usbhw_clr_eps_irq(FLD_USB_EDP3_IRQ);
		usbhw_reset_ep_ptr(USB_EDP_KEYBOARD_OUT);
		usb_g_stall = 0;
		printf("KEYBOARD DATA OUTPUT IRQ!\n");

		unsigned char key_val = usbhw_read_ep_data(USB_EDP_KEYBOARD_OUT);
		printf("key value = 0x%1x", key_val);

		if(usb_g_stall)
		{
			usbhw_data_ep_stall(USB_EDP_KEYBOARD_OUT); printf("STALL!\n");
		}
		else
		{
			usbhw_data_ep_ack(USB_EDP_KEYBOARD_OUT); printf("ACK!\n");
		}
	}

}
#endif
/**
 * @brief		This function serves to handle interrupt request from USB host
 * @param[in] 	none
 * @return 		none
 */

void usb_handle_irq_process(void) {

	unsigned char irq = usbhw_get_ctrl_ep_irq();

	if(irq & FLD_CTRL_EP_IRQ_SETUP) {
		printf("--------------------------------------------- \r\n");
		printf("USB SETUP IRQ!\n");
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_SETUP);
		usb_handle_ctl_ep_setup();
	}
	if(irq & FLD_CTRL_EP_IRQ_DATA) {
		printf("--------------------------------------------- \r\n");
		printf("USB DATA IRQ!\n");
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_DATA);
		usb_handle_ctl_ep_data();
	}
	if(irq & FLD_CTRL_EP_IRQ_STA) {
		printf("--------------------------------------------- \r\n");
		printf("USB STATUS IRQ!\n");
		usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_STA);
		usb_handle_ctl_ep_status();
	}

	if( reg_irq_src & FLD_IRQ_USB_PWDN_EN)
	{
		usb_has_pdwn_irq = 1;//when host suspend or usb hardware interface disconnected will produce pwdn irq
	}
	else
	{
		usb_has_pdwn_irq = 0;
	}

#if USB_MODE_CDC_EN
	usb_cdc_irq_data_process();
#endif

#if USB_MODE_KEYBOARD_EN
	usb_keyboard_irq_data_process();
#endif

#if (USB_MODE_MOUSE_EN)

#endif

#if (USB_MODE_SPEAKER_EN||USB_MODE_MICPHONE_EN)
	usb_audio_irq_data_process();
#endif
	//USB Reset IRQ
	if(reg_irq_src & FLD_IRQ_USB_RST_EN) {
		reg_irq_src |= FLD_IRQ_USB_RST_EN;
#if(USB_MODE_KEYBOARD_EN)
		usb_k_protocol = 1;				//1: report protocol; 0: start protocol
#endif

#if(USB_MODE_MOUSE_EN)
		usb_m_protocol = 1;
#endif

		printf("USB RESET! \r\n");
	}

	usb_g_stall = 0;
}
#if USB_MODE_KEYBOARD_EN
/**
 * @brief		This function serves to send HID report of Keyboard
 * @param[in] 	none
 * @return 		none
 */
int usb_keyboard_hid_report(unsigned char *data){
	//if(reg_usb_ep_ctrl(EDP_ID_KEYBOARD_IN) & FLD_USB_EP_BUSY)
	if(usbhw_is_ep_busy(USB_EDP_KEYBOARD_IN))
		return 0;

    printf("\n KeyBoard HID report:");

	reg_usb_ep_ptr(USB_EDP_KEYBOARD_IN) = 0;

	for(int i = 0; i < 8; ++i){
    	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = data[i];
    	printf("0x%1x ",data[i]);
    }

	reg_usb_ep_ctrl(USB_EDP_KEYBOARD_IN) = FLD_EP_DAT_ACK;
	printf("ACK!\n");
	return 1;
}
#endif

#if USB_MODE_MOUSE_EN
/**
 * @brief		This function serves to send HID report of Mouse
 * @param[in] 	none
 * @return 		none
 */
int usb_mouse_hid_report(unsigned char * p)
{
	if(usbhw_is_ep_busy(USB_EDP_MOUSE))
		return 0;

	signed short x = (signed char)p[1] ;//& BIT7 ? -(p[1] & 0x7f) : p[1];
	signed short y = (signed char)p[2] ;//& BIT7 ? -(p[2] & 0x7f) : p[2];

	reg_usb_ep_ptr(USB_EDP_MOUSE) = 0;

	if(usb_m_protocol == 0)
	{
		reg_usb_ep_dat(USB_EDP_MOUSE) = x & 0xff;
		reg_usb_ep_dat(USB_EDP_MOUSE) = y & 0xff;
	}
	else
	{
		reg_usb_ep_dat(USB_EDP_MOUSE) = p[0];
		reg_usb_ep_dat(USB_EDP_MOUSE) = x;
		reg_usb_ep_dat(USB_EDP_MOUSE) = y;
		reg_usb_ep_dat(USB_EDP_MOUSE) = p[3];
	}
	reg_usb_ep_ctrl(USB_EDP_MOUSE) = FLD_EP_DAT_ACK;
	return 1;
}
#endif
