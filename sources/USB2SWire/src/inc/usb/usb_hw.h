/********************************************************************************************************
 * @file     usb_hw.h
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
#ifndef    _USB_HW_H_
#define    _USB_HW_H_
#pragma once

//#define USB_MAX_RX_FIFO_SIZE		256
//#define USB_MAX_TX_FIFO_SIZE		64

enum {
    USB_EDP_PRINTER_IN = 8, // endpoint 8 is alias of enpoint 0,  becareful.  // default hw buf len = 64
    USB_EDP_MOUSE = 2,          // default hw buf len = 8 // CDC_NOTIFICATION_EPNUM
    USB_EDP_KEYBOARD_IN = 1,    // default hw buf len = 8
    USB_EDP_KEYBOARD_OUT = 3,   // default hw buf len = 16
    USB_EDP_AUDIO_IN = 4,       // default hw buf len = 64
    USB_EDP_PRINTER_OUT = 5,    // default hw buf len = 64
    USB_EDP_SPEAKER = 6,        // default hw buf len = 16
    USB_EDP_MIC = 7,            // default hw buf len = 16
    USB_EDP_MS_IN = USB_EDP_PRINTER_IN,     // mass storage
    USB_EDP_MS_OUT = USB_EDP_PRINTER_OUT,
    USB_EDP_SOMATIC_IN = USB_EDP_AUDIO_IN,      //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
    USB_EDP_SOMATIC_OUT = USB_EDP_PRINTER_OUT,
    USB_EDP_CDC_IN = 4, // CDC_TX_EPNUM
    USB_EDP_CDC_OUT = 5, // CDC_RX_EPNUM
};

/**
 * @brief This function configures the fifo mode for endpoint8
 * @param[in]   dat the setting value needs to be configured 
 * @return none
 */
 static  inline  void  USBHW_Ep8FifoModeSet(unsigned char dat)
{
	reg_usb_ep8_fifo_mode = dat;
}

/**
 * @brief This function configures the threshold value for printer's hardware buffer
 * @param[in]   th the threshold value exceed which the content of buffer will be send out via USB
 * @return none
 */
 static  inline void USBHW_PrinterThresholdSet(unsigned char th)
{
	reg_usb_ep8_send_thre = th;
}

/**
 * @brief This function gets the irq source of control endpoint, i.e., endpoint0
 * @param none
 * @return the irq source of control endpoint
 */
static  inline unsigned long USBHW_CtrlEpIrqGet(void)
{
	return reg_ctrl_ep_irq_sta;
}

/**
 * @brief This function clears the irq status of control endpoint, i.e., endpoint0
 * @param[in]   irq the irq status of control endpoint
 * @return none
 */
static  inline void USBHW_CtrlEpIrqClr(int irq)
{
	reg_ctrl_ep_irq_sta = irq;
}

/**
 * @brief This function sets the control register of control endpoint, i.e., endpoint0
 * @param[in]   data the value needs to set
 * @return none
 */
static  inline void USBHW_CtrlEpCtrlWrite(unsigned char data)
{
	reg_ctrl_ep_ctrl = data;
}

/**
 * @brief This function resets the pointer of control endpoint, i.e., endpoint0
 * @param   none
 * @return none
 */
static  inline void USBHW_CtrlEpPtrReset(void)
{
	reg_ctrl_ep_ptr = 0;
}

/**
 * @brief This function reads the data register of control endpoint, i.e., endpoint0
 * @param   none
 * @return the data read from endpoint0's data register
 */
static  inline unsigned char USBHW_CtrlEpDataRead(void)
{
	return reg_ctrl_ep_dat;
}

/**
 * @brief This function writes one byte data to the data register of control endpoint, i.e., endpoint0
 * @param[in]   data the one byte data needs to write
 * @return none
 */
static  inline void USBHW_CtrlEpDataWrite(unsigned char data)
{
	reg_ctrl_ep_dat = data;
}

/**
 * @brief This function determines whether the control endpoint is busy
 * @param   none
 * @return 1: the control endpoint is busy 0: the control endpoint is idle 
 */
static  inline unsigned char USBHW_IsCtrlEpBusy()
{
	return reg_ctrl_ep_irq_sta & FLD_USB_EP_BUSY;
}

/**
 * @brief This function reads one byte data from the data register of the specified endpoint
 * @param[in]   ep the number of the endpoint 
 * @return one byte data read from the data register of the specified endpoint
 */
static  inline unsigned char USBHW_EpDataRead(unsigned long ep)
{
	return reg_usb_ep_dat(ep);
}

/**
 * @brief This function writes one byte data to the data register of the specified endpoint
 * @param[in]   ep the number of the endpoint 
 * @param[in]   data the one byte data needs to write
 * @return none
 */
static  inline void USBHW_EpDataWrite(unsigned long ep, unsigned char data)
{
	reg_usb_ep_dat(ep) = data;
}

/**
 * @brief This function determines whether the specified endpoint is busy
 * @param[in]   ep the number of the endpoint 
 * @return 1: the control endpoint is busy 0: the control endpoint is idle 
 */
static  inline unsigned long USBHW_IsEpBusy(unsigned long ep)
{
	return reg_usb_ep_ctrl(ep) & FLD_USB_EP_BUSY;
}

/**
 * @brief This function enables the ack transmission when required by host
 * @param[in]   ep the number of the endpoint 
 * @return none
 */
static  inline void USBHW_DataEpAck(unsigned long ep)
{
	reg_usb_ep_ctrl(ep) = FLD_USB_EP_BUSY;
}

/**
 * @brief This function enables the stall function when required by host
 * @param[in]   ep the number of the endpoint 
 * @return none
 */
static  inline void USBHW_DataEpStall(unsigned long ep)
{
	reg_usb_ep_ctrl(ep) = FLD_USB_EP_STALL;
}

/**
 * @brief This function resets the pointer register of specified endpoint
 * @param[in]   ep the number of the endpoint 
 * @return none
 */
static  inline void USBHW_EpPtrReset(unsigned long ep)
{
	reg_usb_ep_ptr(ep) = 0;
}

/**
 * @brief This function disables the manual interrupt
 * @param[in]   m the irq mode needs to set
 * @return none
 */
extern  void USBHW_ManualInterruptDisable(int m);

/**
 * @brief This function enables the manual interrupt
 * @param[in]   m the irq mode needs to set
 * @return none
 */
extern  void USBHW_ManualInterruptEnable(int m);

/**
 * @brief This function sends a bulk of data to host via the specified endpoint
 * @param[in]   ep the number of the endpoint 
 * @param[in]   data pointer to the data need to send
 * @param[in]   len length in byte of the data need to send
 * @return none
 */
extern  void USBHW_EpWrite(unsigned long ep,unsigned char * data, int len);

/**
 * @brief This function sends two bytes data to host via the control endpoint
 * @param[in]   v the two bytes data need to send
 * @return none
 */
extern  void USBHW_CtrlEpU16Write(unsigned short v);

/**
 * @brief This function reads two bytes data from host via the control endpoint
 * @param   none
 * @return the two bytes data read from the control endpoint
 */
extern  unsigned short USBHW_CtrlEpU16Read(void);

#endif // _USB_HW_H_
