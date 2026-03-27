
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define	MCU_CORE_8266	1
#define	MCU_CORE_8366	2
#define MCU_CORE_8368	3
#define	MCU_CORE_8267	4
#define MCU_CORE_8263	5
#define MCU_CORE_8261	6
#define MCU_CORE_8269	7

#define	CLOCK_TYPE_PLL	0
#define	CLOCK_TYPE_OSC	1
#define	CLOCK_TYPE_PAD	2
#define	CLOCK_TYPE_ADC	3

// reg_prod_id	REG_ADDR16(0x7e)
enum {
  MCU_PROD_ID_8266 = 0x5325,
  MCU_PROD_ID_8267 = 0x5326,
  MCU_PROD_ID_8269 = 0x5327,
} MCU_PROD_ID;

#define reg_mcu_id	REG_ADDR8(0x7e)
enum {
  MCU_PROD_ID__8266 = 0x25,
  MCU_PROD_ID__8267 = 0x26,
  MCU_PROD_ID__8269 = 0x27,
} MCU_PROD__ID;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

#include "app_config.h"
#include "compiler.h"
#include "types.h"
#include "bit.h"
#include "register_8267.h"
#include "gpio_default_8267.h"
#include "gpio.h"
#include "utils.h"
#include "analog.h"
#include "irq_i.h"
#include "main.h"
#if (USE_USB_CDC)
#include "usb.h"
#endif

