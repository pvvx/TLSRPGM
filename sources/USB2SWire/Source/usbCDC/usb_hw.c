/********************************************************************************************************
 * @file     usb_hw.c
 *
 * @brief    This file provides set of functions to manage the USB interface
 *
 * @author   xiaodong.zong@telink-semi.com; jian.zhang@telink-semi.com
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

// some API:
// Endpont8 is the alias of endpoint0
SECTION_USB_CODE
void USBHW_ManualInterruptDisable(int m) {
	reg_ctrl_ep_irq_mode |= m;

}

SECTION_USB_CODE
void USBHW_ManualInterruptEnable(int m) {
	reg_ctrl_ep_irq_mode &= (~m);
}

SECTION_USB_CODE
void USBHW_EpWrite(unsigned long ep,unsigned char * data, int len) {
	int i;
	reg_usb_ep_ptr(ep) = 0;

	for (i = 0; i < len; i++)
		reg_usb_ep_dat(ep) = data[i];

	reg_usb_ep_ctrl(ep) = FLD_EP_DAT_ACK;		// ACK
}

// handy help function
SECTION_USB_CODE
void USBHW_CtrlEpU16Write(unsigned short v){
	USBHW_CtrlEpDataWrite(v & 0xff);
	USBHW_CtrlEpDataWrite(v >> 8);
}

SECTION_USB_CODE
unsigned short USBHW_CtrlEpU16Read(void){
	unsigned short v = USBHW_CtrlEpDataRead();
	return (USBHW_CtrlEpDataRead() << 8) | v;
}

#endif // USE_USB_CDC



