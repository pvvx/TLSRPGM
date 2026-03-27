/********************************************************************************************************
 * @file	 cstartup_8266.S
 *
 * @brief	 for TLSR chips
 *
 * @author	 BLE Group
 * @date	 May. 12, 2018
 *
 * @par		 Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *			 All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 *			 Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *			 This heading MUST NOT be removed from this file.
 *
 *			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

	.code	16
@********************************************************************************************************
@											MACROS AND DEFINIITIONS
@********************************************************************************************************
@.include "version.in"

	.equ IRQ_STK_SIZE,	0x180
	
@********************************************************************************************************
@											 TC32 EXCEPTION VECTORS
@********************************************************************************************************

	.section	.vectors,"ax"
	.global		__reset
	.global		__irq
	.global		__start

__start:					@ MUST,	 referenced by boot.link

	.extern irq_handler

	.extern	_ramcode_size_div_256_
	.extern	_ramcode_size_div_16_align_256_
	.extern	_ictag_start_
	.extern	_ictag_end_
@	.extern	_dstored_
	.extern	_start_bss_
	.extern	_end_bss_
	.extern	_bin_size_

	.org 0x0
	tj	__reset
	.word	(0x12345678) @ version
	.org 0x8
	.word	(0x544c4e4b)
	.word	(0x00880000 + _ramcode_size_div_16_align_256_)

	.org 0x10
	tj		__irq
	.org 0x14
@	.word	(_dstored_)
	.org 0x18
	.word	(_bin_size_)
@	.word	(_rodata_size_)
@********************************************************************************************************
@									LOW-LEVEL INITIALIZATION
@********************************************************************************************************
	.extern	 main

	.org 0x20
__reset:
	tloadr		r0, FLL_D
	tloadr		r1, FLL_D+4
	tloadr		r2, FLL_D+8

FLL_STK:
	tcmp		r1, r2
	tjge		FLL_STK_END
	tstorer 	r0, [r1, #0]
	tadd		r1, #4
	tj			FLL_STK
FLL_STK_END:

	tloadr		r0, DAT0		@IRQ
	tmcsr		r0			
	tloadr		r0, DAT0 + 8	@ irq stack end
	tmov		r13, r0	 

	tloadr		r0, DAT0 + 4	@SVC
	tmcsr		r0	
	tloadr		r0, DAT0 + 12	@ stack end
	tmov		r13, r0	 

	tmov		r0, #0
	tloadr		r1, DAT0 + 16	@_start_bss_
	tloadr		r2, DAT0 + 20	@_end_bss_

ZERO:
	tcmp		r1, r2
	tjge		ZERO_END
	tstorer		r0, [r1, #0]
	tadd		r1, #4
	tj			ZERO
ZERO_END:

	tloadr		r1, DAT0 + 28	@ IC tag start
	tloadr		r2, DAT0 + 32	@ IC tag end

ZERO_TAG:
	tcmp		r1, r2
	tjge		ZERO_TAG_END
	tstorer		r0, [r1, #0]
	tadd		r1, #4
	tj			ZERO_TAG
ZERO_TAG_END:

SETIC:
	tloadr		r1, DAT0 + 24	@ 0x80060c
	tloadr		r0, DAT0 + 36	@ IC tag start
	tstorerb	r0, [r1, #0]	@ 0x80060c + 0
	tadd		r0, #1			@ IC tag end
	tstorerb	r0, [r1, #1]	@ 0x80060c + 1
	@tmov		r0, #0;
	@tstorerb	r0, [r1, #2]	@ 0x80060c + 2

	tjl			main

END:	tj	END

	.balign	4
DAT0:
	.word	0x12						@0 @IRQ
	.word	0x13						@4 @SVC
	.word	(irq_stk + IRQ_STK_SIZE)	@8 irq stack end
	.word	(0x80c000)					@12 stack end
	.word	(_start_bss_)				@16
	.word	(_end_bss_)					@20
	.word	(0x80060c)					@24
	.word	_ictag_start_				@28	@ IC tag start
	.word	_ictag_end_					@32	@ IC tag end
	.word	_ramcode_size_div_256_		@36 @ _icdata_ x 0x100

FLL_D:
	.word	0xffffffff
	.word	(_end_bss_)
	.word	(0x80c000)

	.align 4

__irq:
	tpush		{r14}
	tpush		{r0-r7}
	tmrss		r0
	
	tmov		r1, r8
	tmov		r2, r9
	tmov		r3, r10
	tmov		r4, r11
	tmov		r5, r12
	tpush		{r0-r5}
	
	tjl			irq_handler

	tpop		{r0-r5}
	tmov		r8, r1
	tmov		r9, r2
	tmov		r10,r3
	tmov		r11,r4
	tmov		r12,r5

	tmssr		r0
	tpop		{r0-r7}
	treti		{r15}

ASMEND:

	.section .bss
	.align 4
	.lcomm irq_stk, IRQ_STK_SIZE
	.end

