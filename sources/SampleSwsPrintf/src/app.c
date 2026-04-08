/********************************************************************************************************
 * @file     app.c 
 *
 * @brief    This is the source file for TLSR8258
 *
 * @author	 pvvx
 *         
 *******************************************************************************************************/
#include "app_config.h"
#include "sws_printf.h"

void user_init(int deepRetWakeUp)
{
	if(deepRetWakeUp) {
		sws_init();
		printf("*******************************************************************************************************\n");
		printf("* Test SWS Printf\n");
		printf("*******************************************************************************************************\n");
	}
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////


void main_loop (void)
{
	static int count;

	printf("\rMain loop count: %d", count);
	count++;
	if(count > 5000) {
		printf("\n-------------------------------------------------------------------------------------------------------\n");
		printf("Goto Sleep...............................\n");
		sws_buffer_flush();
#if (MCU_CORE_B85)
		cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW16K, PM_WAKEUP_TIMER, (clock_time() + 500*CLOCK_SYS_TIMER_CLK_1MS));
#elif (MCU_CORE_B80 || MCU_CORE_B80B)
	    cpu_long_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW16K, PM_WAKEUP_TIMER, 500*CLOCK_32K_SYS_TIMER_CLK_1MS);
#endif

	}
}




