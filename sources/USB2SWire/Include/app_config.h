#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

//------------- USB/UART Config ---------------//
// USE_USB_CDC or USE_INT_UART (!)
#define USE_USB_CDC			1
#define USE_INT_UART		(!USE_USB_CDC)
//-------------------------------
#define SWIRE_OFF 0

//-------------- UART Config -----------------//
#define UART_BAUD 			230400 	// 115200 or 230400

//-------- GPIO Config -------------//
#define GPIO_DM    GPIO_PE2
#define GPIO_DP    GPIO_PE3
#define GPIO_SWM   GPIO_PA7
#define GPIO_SWS   GPIO_PB0
#define GPIO_TX    GPIO_PC2
#define GPIO_RX    GPIO_PC3
#define GPIO_RESET GPIO_PB1
#define GPIO_POWER GPIO_PC4

#define CLR_GPIO_RESET() BM_CLR(reg_gpio_oen(GPIO_RESET), GPIO_RESET & 0xff) // Pin RST = "0"
#define SET_GPIO_RESET() BM_SET(reg_gpio_oen(GPIO_RESET), GPIO_RESET & 0xff) // Pin RST = "Z" (open drain)

#define CLR_GPIO_POWER() BM_CLR(reg_gpio_out(GPIO_POWER), GPIO_POWER & 0xff) // Pin POW = "0"
#define SET_GPIO_POWER() BM_SET(reg_gpio_out(GPIO_POWER), GPIO_POWER & 0xff) // Pin POW = "1"


//-------- External Flash Config -------------//
#define USE_EXT_FLASH 0	// not used in this project!
#define CS_EXT_FLASH GPIO_PB5 // not used in this project!

//-------- Chip Config -------------//
#define CHIP_TYPE MCU_CORE_8269	// in this project is always = MCU_CORE_8269!
#define MCU_CORE_TYPE CHIP_TYPE

/////////////////// Clock Сщташп  ///////////////////
// E104_BT10 module is used !
#define CLOCK_FHS_TYPE		FHS_SEL_PLL // FHS_SEL_PLL, FHS_SEL_RC, FHS_SEL_PAD
#define CLOCK_SYS_TYPE  	CLK_SEL_HSDIV // one of the following: CLK_SEL_HSDIV, CLK_SEL_RC32M, CLK_SEL_PADM, CLK_SEL_PADK

#define CLOCK_SYS_CLOCK_HZ  32000000

#define	QUARTZ_16MHZ	16
#define	QUARTZ_12MHZ	12
// E104_BT10 module Quartz = 16 MHz!
#define SET_PLL 		QUARTZ_16MHZ	// QUARTZ_16MHZ, QUARTZ_12MHZ

/////////////////// watchdog  ///////////////////////

#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		100  //ms


/////////////////// IRQ  /////////////////////////////////
#define USE_IRQ_SAVE 	USE_USB_CDC

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
