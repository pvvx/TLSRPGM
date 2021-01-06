/********************************************************************************************************
 * @file     irq_i.h
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#pragma once

// NOTE("If irq switches changed, pls change IRQ_INIT_VALUE accordingly.");
// enalbe interrupt
static inline u8 irq_enable(){
	u8 r = reg_irq_en;		// don't worry,  the compiler will optimize the return value if not used
	reg_irq_en = 1;
	return r;
}
static inline u8 irq_disable(){
	u8 r = reg_irq_en;		// don't worry,  the compiler will optimize the return value if not used
	reg_irq_en = 0;
	return r;
}
static inline void irq_restore(u8 en){
	reg_irq_en = en;
}

// enable one interrupt
static inline u32 irq_get_mask(){
	return reg_irq_mask;
}

static inline void irq_set_mask(u32 msk){ 
    SET_FLD(reg_irq_mask, msk);
}

static inline void irq_clr_mask(u32 msk){
    CLR_FLD(reg_irq_mask, msk);
}

// interrupt source
static inline u32 irq_get_src(){
	return reg_irq_src;
}

static inline void irq_clr_src(){
	reg_irq_src = ONES_32;	// set to clear
}

static inline void irq_enable_type(u32 msk){
	irq_set_mask(msk);
}
static inline void irq_disable_type(u32 msk){
	irq_clr_mask(msk);
}

#if (__DEBUG__)			// Debug only,  to indicate in irq_handler
static int irq_in_handler = 0;
static inline void irq_set_in_handler(void){
	irq_in_handler = 1;
}
static inline void irq_set_out_handler(void){
	irq_in_handler = 0;
}
static inline u32 irq_is_in_handler(void){
	return irq_in_handler;
}
#endif

