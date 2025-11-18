/********************************************************************************************************
 * @file     usb.h
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

#ifndef USB_H_
#define USB_H_

#include "usbhw.h"
#include "usbhw_i.h"
#include "usb_desc.h"

extern unsigned char usb_g_config;
extern unsigned char usb_has_pdwn_irq;

#if(USB_MODE_CDC_EN)
extern unsigned char usb_cdc_data[USB_CDC_TX_RX_EPSIZE];
extern unsigned short usb_cdc_data_len;
#endif

#if(USB_MODE_MICPHONE_EN||USB_MODE_SPEAKER_EN)
extern unsigned char usb_audio_mic_cnt;
extern unsigned char usb_audio_speaker_cnt;
#endif

enum{
	USB_EDP_SETUP_IRQ_REQ = 0,
	USB_EDP_DATA_IRQ_REQ,
};
/**
 * @brief     This function servers to set USB Input.
 * @param[in] none
 * @return    none.
 */

void audio_rx_data_from_usb(void);

/**
 * @brief     This function serves to send data to USB.
 * @param[in] Input_Type - audio input type.
 * @param[in] Audio_Rate - audio rate.
 * @return    none.
 */

void audio_tx_data_to_usb(AudioInput_Typedef Input_Type,AudioRate_Typedef Audio_Rate);

/**
 * @brief		This function serves to enable manual interrupt
 * @param[in] 	none
 * @return 		none
 */
void usb_irq_init(void);

/**
 * @brief		This function serves to handle interrupt request
 * @param[in] 	data_request - DATA_REQ and SETUP_REQ
 * @return 		none
 */
void usb_handle_request_process(unsigned char data_request);

/**
 * @brief		This function serves to handle SETUP process
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_ctl_ep_setup();

/**
 * @brief		This function serves to handle DATA process
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_ctl_ep_data(void);

/**
 * @brief		This function serves to handle STATUS process
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_ctl_ep_status();

/**
 * @brief		This function serves to handle interrupt request from USB host
 * @param[in] 	none
 * @return 		none
 */
void usb_handle_irq_process(void);

#if USB_MODE_CDC_EN
/**
 * @brief		This function serves to send data to USB host in CDC device
 * @param[in] 	data_ptr -  the pointer of data, which need to be sent.
 * @param[in] 	data_len -  the length of data, which need to be sent.
 * @return 		none
 */
void usb_cdc_tx_data_to_host(unsigned char * data_ptr, unsigned short data_len);
#endif

#if(USB_MODE_KEYBOARD_EN)
/**
 * @brief		This function serves to send HID report of Keyboard
 * @param[in] 	none
 * @return 		none
 */
int usb_keyboard_hid_report(unsigned char *data);
#endif

#if USB_MODE_MOUSE_EN
/**
 * @brief		This function serves to send HID report of Mouse
 * @param[in] 	none
 * @return 		none
 */
int usb_mouse_hid_report(unsigned char * p);
#endif

#endif /* USB_H_ */
