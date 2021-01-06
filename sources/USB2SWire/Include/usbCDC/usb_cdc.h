/********************************************************************************************************
 * @file     usb_cdc.h
 *
 * @brief    This file provides set of functions to manage the USB cdc device
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
// correct pvvx
#pragma once
#ifndef _USB_CDC_H_
#define _USB_CDC_H_

#include "usbCDC/usb_stdDescriptors.h"

#ifndef USB_CDC_MAX_RX_BLK_SIZE
/*
 * USB_CDC_MAX_RX_BLK_SIZE определяет политику приема  размер приемного буфера:
 * = 64 - ведется прием до 64 байт (отдает в rxCb()) без ожидания таймаута
 * > 64 - ведется прием до USB_CDC_MAX_RX_BLK_SIZE байт (отдает в rxCb())
 *        c ожиданием межблочного (USB_RX_LAST64BLK_TIMEOUT) таймаута
 */
#define USB_CDC_MAX_RX_BLK_SIZE	256
#endif
#ifndef USB_RX_LAST64BLK_TIMEOUT
/*
 * USB_RX_LAST64BLK_TIMEOUT таймаут после последнего rx блока в 64 байта в us
 * Должен быть от 500 us -
 * 500 us не работает на всех USB, надо больше
 */
#define USB_RX_LAST64BLK_TIMEOUT 1750 // 1750
#endif

/**
 *  @brief  Define USB CDC subclass 
 */
enum CDC_Descriptor_ClassSubclassProtocol_t {
    CDC_CSCP_CDCClass               = 0x02,	// Значение класса дескриптора, показывающее, что устройство или интерфейс принадлежит классу CDC.
    CDC_CSCP_NoSpecificSubclass     = 0x00, // Значение подкласса дескриптора, показывающее, что устройство или интерфейс принадлежит к специфичному подклассу класса CDC.
    CDC_CSCP_ACMSubclass            = 0x02, // Значение подкласса дескриптора, показывающее, что устройство или интерфейс принадлежит к подклассу Abstract Control Model CDC (абстрактная модель управления).
    CDC_CSCP_ATCommandProtocol      = 0x01, // Значение протокола дескриптора, показывающее, что устройство или интерфейс принадлежит к протоколу команд AT класса CDC.
    CDC_CSCP_NoSpecificProtocol     = 0x00, // Значение класса дескриптора, показывающее, что устройство или интерфейс не принадлежит никакому определенному протоколу класса CDC.
    CDC_CSCP_VendorSpecificProtocol = 0xFF, // Значение класса дескриптора, показывающее, что устройство или интерфейс принадлежит протоколу класса CDC, специфичному для вендора.
    CDC_CSCP_CDCDataClass           = 0x0A, // Значение класса дескриптора, показывающее, что устройство или интерфейс принадлежит классу данных CDC.
    CDC_CSCP_NoDataSubclass         = 0x00, // Значение подкласса дескриптора, показывающее, что устройство или интерфейс не принадлежит никакому определенному подклассу класса данных CDC.
    CDC_CSCP_NoDataProtocol         = 0x00, // Значение протокола дескриптора, показывающее, что устройство или интерфейс не принадлежит никакому определенному протоколу класса данных CDC.
};

/**
 *  @brief  Define USB CDC class-specific request 
 */
enum {
    CDC_REQ_SendEncapsulatedCommand = 0x00,	// специфичный для класса CDC запрос для отправки на устройство энкапсулированной команды.
    CDC_REQ_GetEncapsulatedResponse = 0x01, // специфичный для класса CDC запрос для получения от устройства энкапсулированного ответа на команду.
    CDC_REQ_SetLineEncoding         = 0x20, // специфичный для класса CDC запрос для установки текущих настроек конфигурации виртуального последовательного порта.
    CDC_REQ_GetLineEncoding         = 0x21, // специфичный для класса CDC запрос для полуения текущих настроек конфигурации виртуального последовательного порта.
    CDC_REQ_SetControlLineState     = 0x22, // специфичный для класса CDC запрос для установки текущих состояний рукопожатия линии (handshake line) виртуального последовательного порта.
    CDC_REQ_SendBreak               = 0x23, // специфичный для класса CDC запрос для отправки команды перерыва (break) приемнику через канал несущей.
};

enum {
    CDC_NOTIF_SerialState = 0x20,
};

/**
 *  @brief  Define USB CDC class-specific interface type 
 */
enum {
    CDC_DSUBTYPE_CSInterface_Header           = 0x00,	// специфичный для класса CDC заголовок функционального дескриптора.
    CDC_DSUBTYPE_CSInterface_CallManagement   = 0x01,	// специфичный для класса CDC функциональный дескриптор управления вызовами.
    CDC_DSUBTYPE_CSInterface_ACM              = 0x02,	// специфичный для класса CDC функциональный дескриптор абстрактной модели управления (Abstract Control Model).
    CDC_DSUBTYPE_CSInterface_DirectLine       = 0x03,	// специфичный для класса CDC функциональный дескриптор прямой линии (Direct Line).
    CDC_DSUBTYPE_CSInterface_TelephoneRinger  = 0x04,	// специфичный для класса CDC функциональный дескриптор телефонного звонка.
    CDC_DSUBTYPE_CSInterface_TelephoneCall    = 0x05,	// специфичный для класса CDC функциональный дескриптор телефонного вызова.
    CDC_DSUBTYPE_CSInterface_Union            = 0x06,	// специфичный для класса CDC функциональный дескриптор объединения.
    CDC_DSUBTYPE_CSInterface_CountrySelection = 0x07,	// специфичный для класса CDC функциональный дескриптор выбора страны.
    CDC_DSUBTYPE_CSInterface_TelephoneOpModes = 0x08,	// специфичный для класса CDC функциональный дескриптор режимов работы телефона.
    CDC_DSUBTYPE_CSInterface_USBTerminal      = 0x09,	// специфичный для класса CDC функциональный дескриптор терминала.
    CDC_DSUBTYPE_CSInterface_NetworkChannel   = 0x0A,	// специфичный для класса CDC функциональный дескриптор канала сети.
    CDC_DSUBTYPE_CSInterface_ProtocolUnit     = 0x0B,	// специфичный для класса CDC функциональный дескриптор элемента протокола (Protocol Unit).
    CDC_DSUBTYPE_CSInterface_ExtensionUnit    = 0x0C,	// специфичный для класса CDC функциональный дескриптор элемента расширения (Extension Unit).
    CDC_DSUBTYPE_CSInterface_MultiChannel     = 0x0D,	// специфичный для класса CDC функциональный дескриптор многоканального управления.
    CDC_DSUBTYPE_CSInterface_CAPI             = 0x0E,	// специфичный для класса CDC функциональный дескриптор общего ISDN API.
    CDC_DSUBTYPE_CSInterface_Ethernet         = 0x0F,	// специфичный для класса CDC функциональный дескриптор Ethernet.
    CDC_DSUBTYPE_CSInterface_ATM              = 0x10,	// специфичный для класса CDC функциональный дескриптор режима асинхронной передачи (Asynchronous Transfer Mode).
};

enum{
    CDC_LINEENCODING_OneStopBit          = 0,	// Каждый фрейм содержит один стоп-бит.
    CDC_LINEENCODING_OneAndAHalfStopBits = 1,	// Каждый фрейм содержит полтора стоп-бита.
    CDC_LINEENCODING_TwoStopBits         = 2,	// Каждый фрейм содержит два стоп-бита.
};

enum{
    CDC_PARITY_None  = 0,	// Режим без бита четности в каждом фрейме.
    CDC_PARITY_Odd   = 1,	// Режим нечетного бита четности в каждом фрейме.
    CDC_PARITY_Even  = 2,	// Режим четного бита четности в каждом фрейме
    CDC_PARITY_Mark  = 3,	// Режим метки для бита четности в каждом фрейме.
    CDC_PARITY_Space = 4,	// Режим пропуска для бита четности в каждом фрейме.
};

/**
 *  @brief  Define USB CDC class-specific descriptor function header
 */
typedef struct{
    USB_Descriptor_Header_t    Header;
    unsigned char              Subtype;
    unsigned short             CDCSpecification;
} USB_CDC_Descriptor_FunctionalHeader_t;

typedef struct
{
    unsigned char    bFunctionLength;
    unsigned char    bDescriptorType;
    unsigned char    bDescriptorSubType;
    unsigned short   bcdCDC;
} USB_CDC_StdDescriptor_FunctionalHeader_t;

/**
 *  @brief  Define USB CDC class-specific descriptor of ACM subclass
 */
typedef struct{
    USB_Descriptor_Header_t    Header;
    unsigned char              Subtype;
    unsigned char              Capabilities;
} USB_CDC_Descriptor_FunctionalACM_t;

typedef struct{
    unsigned char    bFunctionLength;
    unsigned char    bDescriptorType;
    unsigned char    bDescriptorSubType;
    unsigned char    bmCapabilities;
} USB_CDC_StdDescriptor_FunctionalACM_t;


typedef struct{
    USB_Descriptor_Header_t    Header;
    unsigned char              Subtype;
    unsigned char              MasterInterfaceNumber;
    unsigned char              SlaveInterfaceNumber;
} USB_CDC_Descriptor_FunctionalUnion_t;


typedef struct{
    unsigned char    bFunctionLength;
    unsigned char    bDescriptorType;
    unsigned char    bDescriptorSubType;
    unsigned char    bMasterInterface;
    unsigned char    bSlaveInterface0;
} USB_CDC_StdDescriptor_FunctionalUnion_t;


typedef struct{
    unsigned int    BaudRateBPS;
    unsigned char   CharFormat;
    unsigned char   ParityType;
    unsigned char   DataBits;
} CDC_LineEncoding_t;

typedef void (*cdc_handlerFn_t)(unsigned char *pData);
typedef void (*cdc_cbrxFn_t)(unsigned char *pData, unsigned int len);
typedef void (*cdc_cbtxFn_t)(unsigned int len);

typedef struct {
    unsigned char len;
    unsigned char data[1];
} USBCDC_TxBuf_t;


typedef enum usbcdc_sts_e {
    USB_OK = 0,
    USB_BUSY
} USBCDC_Sts_t;

/**
 *  @brief  Define USB CDC device information
 */
typedef struct{
    struct{
        unsigned char       ControlInterfaceNumber;
        unsigned char       DataINEndpointNumber;
        unsigned short      DataINEndpointSize;
        unsigned char       DataINEndpointDoubleBank;

        unsigned char       DataOUTEndpointNumber;
        unsigned short      DataOUTEndpointSize;
        unsigned char       DataOUTEndpointDoubleBank;

        unsigned char       NotificationEndpointNumber;
        unsigned short      NotificationEndpointSize;
        unsigned char       NotificationEndpointDoubleBank;
    } Config;

    struct{
        struct{
            unsigned short      HostToDevice;
            unsigned short      DeviceToHost;
        } ControlLineStates;
        CDC_LineEncoding_t LineEncoding;
    } State;

} USB_ClassInfo_CDC_Device_t;

typedef struct {
#ifndef USB_TX_CALLBACK
    cdc_cbtxFn_t txCb;
#endif
#ifndef USB_RX_CALLBACK
    cdc_cbrxFn_t rxCb;
#endif
    unsigned char *txBuf; // если NULL - всё предано
    unsigned char *rxBuf; // если NULL - ?
    unsigned short lastSendIndex; // указатель в txBuf на ещё не переданный байт
    unsigned short lenToSend; // сколько байт ещё надо передать
    unsigned short lastRecvIndex; // указатель в rxBuf на ещё не принятый байт
} USBCDC_Ctrl_t;

extern USBCDC_Ctrl_t cdc_vs;

#if (USB_CDC_MAX_RX_BLK_SIZE > 64)
extern unsigned char usb_flg_rx_timeot;
void USB_RxTimeOuts(void);
#endif

/**
 * @brief This function handles the USB CDC class-specific requests from host
 * @param[in]   bRequest the request type
 * @param[in]   wValue word-sized field that varies according to request
 * @param[in]   wIndex word-sized field that varies according to request
 *              typically used to pass an index or offset
 * @param[in]   wLength number of bytes to transfer if there is a Data stage
 * @return none
 */
extern void USBCDC_ControlRequestProcess(unsigned char bRequest, unsigned short wValue, unsigned short wIndex, unsigned short wLength);

/**
 * @brief This function sends data to host as the USB CDC device
 * @param[in]   buf pointer to the data need to send
 * @param[in]   len length in byte of the data need to send
 * @return none
 */
USBCDC_Sts_t USBCDC_DataSend(unsigned char* buf, unsigned int len);

/**
 * @brief This function sends bulk data to host as the USB CDC device
 * @param[in]   none
 * @return the length of the data send out
 */
unsigned int USBCDC_BulkDataSend(void);

/**
 * @brief This function handles the received data
 * @param   none
 * @return none
 */
void USBCDC_DataRecv(void);

/**
 * @brief This function initializes the USB CDC device
 * @param   none
 * @return none
 */
static inline void USBCDC_Init(void) {
    cdc_vs.lastRecvIndex = 0;
#ifdef USB_SET_CFG_UART
    USB_SET_CFG_UART(LineEncoding);
#endif
}

/**
 * @brief This function sets the USB CDC tx and rx callback function
 * @param[in]   rxFunc rx callback function
 * @param[in]   txCb tx callback function
 * @return none
 */
#if !(defined(USB_TX_CALLBACK) || defined(USB_TX_CALLBACK))
static inline void USBCDC_CBSet(cdc_cbrxFn_t rxFunc, cdc_cbtxFn_t txCb) {
    cdc_vs.rxCb = rxFunc;
    cdc_vs.txCb = txCb;
}
#endif

/**
 * @brief This function sets the USB CDC rx buffer
 * @param[in]   buf pointer to the rx buffer
 * @return none
 */
static inline void USBCDC_RxBufSet(unsigned char *buf) {
    cdc_vs.rxBuf = buf;
}

/**
 * @brief This function determines whether the USB interface is available
 * @param   none
 * @return 1: the USB interface is available 0: the USB interface is busy
 */
static inline unsigned int USBCDC_IsAvailable(void) {
    return (cdc_vs.txBuf == 0);
}

/**
 * @brief This function used to send one byte to host
 * @param   none
 * @return 1: send out successfully 0: the USB interface is busy
 */
extern unsigned char usbWriteByte(u8 byte);

/**
 * @brief This function used to check whether the Tx of the control endpoint is busy
 * @param   none
 * @return 1: the control endpoint is busy 0: the control endpoint is idle
 */
extern unsigned char USBCDC_TxBusy(void);


#endif // _USB_CDC_H_
