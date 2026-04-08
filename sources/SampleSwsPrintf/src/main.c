/********************************************************************************************************
 * @file     main.c
 *
 * @brief    This is the source file for TLSR8258
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

#include "app_config.h"

/**
 * @brief		This function serves to handle the interrupt of MCU
 * @param[in] 	none
 * @return 		none
 */
_attribute_ram_code_sec_noinline_
void irq_handler(void)
{
#if 0 // RF_MODE
	unsigned short rf_irq_src = rf_irq_src_get();
	if (rf_irq_src) {
		if (rf_irq_src & FLD_RF_IRQ_TX) {
			tx_state = 1;
//			gpio_toggle(LED1);
			rf_irq_clr_src(FLD_RF_IRQ_TX);
		}

		if (rf_irq_src & FLD_RF_IRQ_RX) {
			rx_state = 1;
//			gpio_toggle(LED2);
			rf_irq_clr_src(FLD_RF_IRQ_RX);
		}

		if (rf_irq_src & FLD_RF_IRQ_RX_TIMEOUT)
		{
			timeout_state = 1;
//			gpio_toggle(LED3);
			rf_irq_clr_src(FLD_RF_IRQ_RX_TIMEOUT);
		}

		if (rf_irq_src & FLD_RF_IRQ_FIRST_TIMEOUT)
		{
			timeout_state = 1;
//			gpio_toggle(LED3);
			rf_irq_clr_src(FLD_RF_IRQ_FIRST_TIMEOUT);
		}
	}
#endif
}


/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
int main (void)   //must on ramcode
{
	blc_pm_select_internal_32k_crystal();
#if (MCU_CORE_B85)
	cpu_wakeup_init();
	//user_read_flash_value_calib();
#elif (MCU_CORE_B80 || MCU_CORE_B80B)
	cpu_wakeup_init(INTERNAL_CAP_XTAL24M);
	wd_32k_stop();
	//user_read_otp_value_calib();
	//user_read_flash_value_calib();
#else
#error "MCU_CORE ?"
#endif

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();

    gpio_init(!deepRetWakeUp);

    //flash_init(flash_protect_en);

	clock_init(SYS_CLK);

#if 0
	rf_mode_init();
	rf_set_ble_1M_mode();
#endif

	user_init(deepRetWakeUp);

	while (1) {
		main_loop ();
	}
	return 0;
}


