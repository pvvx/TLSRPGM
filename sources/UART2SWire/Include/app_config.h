#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define USE_IRQ_SAVE	0
#define CLOCK_SYS_CLOCK_HZ  	24000000

#define	MCU_CORE_8266 		1
#define	MCU_CORE_8267 		2
#define MCU_CORE_8269 		3
#define MCU_CORE_825x 		4

#define CHIP_TYPE MCU_CORE_825x

#define MCU_CORE_TYPE CHIP_TYPE

//#define reg_prod_id	REG_ADDR16(0x7e)
enum {
  MCU_PROD_ID_8266 = 0x5325,
  MCU_PROD_ID_8267 = 0x5326,
  MCU_PROD_ID_8269 = 0x5327,
  MCU_PROD_ID_8253 = 0x5562,
} MCU_PROD_ID;

#define reg_mcu_id	REG_ADDR8(0x7e)
enum {
  MCU_PROD_ID__8266 = 0x25,
  MCU_PROD_ID__8267 = 0x26,
  MCU_PROD_ID__8269 = 0x27,
  MCU_PROD_ID__8253 = 0x62,
} MCU_PROD__ID;

#define reg_mcu_tid	REG_ADDR8(0x7d)
enum {
  MCU_PROD_TID__826x = 0x53,
  MCU_PROD_TID__825x = 0x55,
} MCU_PROD__TID;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
