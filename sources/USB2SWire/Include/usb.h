/*
 * usb.h
 *
 *  Created on: 22.02.2020
 *      Author: pvvx
 */

#ifndef USB_H_
#define USB_H_

#include "app_config.h"

//------------- USB Config -------------------//
#define USB_CDC_MAX_RX_BLK_SIZE (DATA_BUFF_SIZE+6)

//------- USB product Information ------------//
#define STRING_VENDOR        L"Telink"
#define STRING_PRODUCT       L"USB2SWIRE"
#define STRING_SERIAL        L"ver_0002"
#define USB_MAX_POWER		 50 // mA
#define USB_VID		0x248a
#define USB_PID		0x8269

//extern volatile char usb_flg; //Flg bit0: DTR - Open/Close USB-COM port
//#define USB_RESET() usb_flg = 0 // Events: USB_SET_CTRL_UART DTR Off, USB_PWDN, USB_RESET
//#define USB_PWDN() usb_flg = 0 // Events: USB_SET_CTRL_UART DTR Off, USB_PWDN, USB_RESET
//#define USB_SET_CTRL_UART(flg) usb_flg = flg
//#define USB_SET_CFG_UART(p) // p = CDC_LineEncoding_t

void USBCDC_RxCb(unsigned char *data, unsigned int len);

#if (USE_USB_CDC)
#define USB_IrqHandle irq_handler
#endif

#define USB_RX_CALLBACK USBCDC_RxCb(p, len)
#define USB_TX_CALLBACK {utxb.len = 0;}

#define SECTION_USB_CODE _attribute_ram_code_
#define SECTION_USB_CONST const


#include "usbCDC/drivers.h"

#endif /* USB_H_ */
