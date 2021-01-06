
#pragma once

#include "compiler.h"
// constants
// system clock


//  must use macro,  because used in #if
#define	CLOCK_TYPE_PLL	0
#define	CLOCK_TYPE_OSC	1
#define	CLOCK_TYPE_PAD	2
#define	CLOCK_TYPE_ADC	3

enum{
	CLOCK_SEL_32M_RC = 	0,
	CLOCK_SEL_HS_DIV = 	1,
	CLOCK_SEL_16M_PAD =	2,
	CLOCK_SEL_32M_PAD =	3,
	CLOCK_SEL_SPI  	  = 4,
	CLOCK_SEL_40M_INTERNAL = 5,
	CLOCK_SEL_32K_RC  =	6,
};

enum{
	CLOCK_HS_240M_PLL =	0,
	CLOCK_HS_40M_ADC = 	1,
	CLOCK_HS_32M_OSC =	2,
	CLOCK_HS_16M_OSC = 	3,
};




#ifdef	USE_SYS_TICK_PER_US
	extern	u32 sys_tick_per_us;

	#define 	CLOCK_PLL_CLOCK		   		192000000
	#define	    CLOCK_SYS_CLOCK_1S   		(sys_tick_per_us * 1000000)
	#define		CLOCK_SYS_CLOCK_1MS  		(sys_tick_per_us * 1000)
	#define		CLOCK_SYS_CLOCK_1US	  		sys_tick_per_us

#else

	enum{
		CLOCK_PLL_CLOCK = 192000000,

		CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
		CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
		CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
	};

	#define 	sys_tick_per_us		CLOCK_SYS_CLOCK_1US

#endif

void 	set_tick_per_us (u32 t);

enum{
	CLOCK_MODE_SCLK = 0,
	CLOCK_MODE_GPIO = 1,
	CLOCK_MODE_WIDTH_GPI = 2,
	CLOCK_MODE_TICK = 3
};

#if(CLOCK_SYS_TYPE == CLOCK_TYPE_PLL)
	#define CLK_FHS_MZ		192
#elif(CLOCK_SYS_TYPE == CLOCK_TYPE_PAD)
	#if(CLOCK_SYS_CLOCK_HZ == 32000000)
		#define CLK_FHS_MZ		32
	#elif(CLOCK_SYS_CLOCK_HZ == 12000000)
		#define CLK_FHS_MZ		12
	#else
		#error
	#endif
#elif(CLOCK_SYS_TYPE == CLOCK_TYPE_OSC)
	#if(CLOCK_SYS_CLOCK_HZ == 32000000)
		#define CLK_FHS_MZ		192
	#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
		#define CLK_FHS_MZ		32			//  DIVIDE == 2,  32/2 = 16, see reg 0x66
	#elif(CLOCK_SYS_CLOCK_HZ == 8000000)
		#define CLK_FHS_MZ		32			//  DIVIDE == 2,  32/2 = 16, see reg 0x66
	#else
		#error
	#endif
#else
		#error
#endif

_attribute_ram_code_ void clock_init();
_attribute_ram_code_ void sleep_us (u32 microsec);		//  use register counter to delay 


//  delay precisely
#define		CLOCK_DLY_1_CYC    _ASM_NOP_
#define		CLOCK_DLY_2_CYC    _ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_3_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_4_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_5_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_6_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_7_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_8_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_9_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_10_CYC   _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_

#if (CLOCK_SYS_CLOCK_HZ == 30000000 || CLOCK_SYS_CLOCK_HZ == 32000000)
	#define		CLOCK_DLY_100NS		CLOCK_DLY_3_CYC							// 100,  94
	#define		CLOCK_DLY_200NS		CLOCK_DLY_6_CYC							// 200, 188
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC 		// 200, 188
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	#define 	CLOCK_DLY_63NS 		CLOCK_DLY_3_CYC 		//  63 ns
	#define		CLOCK_DLY_100NS		CLOCK_DLY_4_CYC			//  100 ns
	#define		CLOCK_DLY_200NS		CLOCK_DLY_8_CYC			//  200 ns
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC 		//	600 ns
#elif (CLOCK_SYS_CLOCK_HZ == 12000000 || CLOCK_SYS_CLOCK_HZ == 16000000)
	#define 	CLOCK_DLY_63NS 		CLOCK_DLY_1_CYC 		//  63 ns
	#define		CLOCK_DLY_100NS		CLOCK_DLY_2_CYC			//  128 ns
	#define		CLOCK_DLY_200NS		CLOCK_DLY_4_CYC			//  253 ns
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC 		//	253 ns
#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
	#define		CLOCK_DLY_100NS		CLOCK_DLY_5_CYC			// 104
	#define		CLOCK_DLY_200NS		CLOCK_DLY_10_CYC		// 208
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC		//	600 ns
#elif (CLOCK_SYS_CLOCK_HZ == 6000000 || CLOCK_SYS_CLOCK_HZ == 8000000)
	#define		CLOCK_DLY_100NS		CLOCK_DLY_1_CYC			//  125 ns
	#define		CLOCK_DLY_200NS		CLOCK_DLY_2_CYC			//  250
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_5_CYC 		//  725
#else
#define		CLOCK_DLY_100NS		CLOCK_DLY_1_CYC			//  125 ns
#define		CLOCK_DLY_200NS		CLOCK_DLY_2_CYC			//  250
#define 	CLOCK_DLY_600NS 	CLOCK_DLY_5_CYC 		//  725
#endif

/**
 * @brief     This function performs to gets system timer0 address.
 * @param[in] none.
 * @return    timer0 address.
 */

static inline unsigned long clock_time(void)
{
	return reg_system_tick;
}

/**
 * @brief     This function performs to calculation exceed us of the timer.
 * @param[in] ref - Variable of reference timer address.
 * @param[in] span_us - Variable of span us.
 * @return    the exceed.
 */

static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int us)
{
	return ((unsigned int)(clock_time() - ref) > us * CLOCK_SYS_CLOCK_1US);
}
