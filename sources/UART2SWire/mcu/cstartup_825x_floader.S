
#define USE_FLASH_WAKEUP	1
#define USE_IRQ				0
#define USE_ICACHE			1
#define USE_SETSPISPEED		0
#define USE_FLL_D			0
#define USE_DATA_I			1

	.code	16
@********************************************************************************************************
@                                           MACROS AND DEFINIITIONS
@********************************************************************************************************

	.equ IRQ_STK_SIZE,	0x180
	.equ __RAM_START_ADDR, 	0x00840000
	.equ __RAM_SIZE_MAX, 	(32*1024)
	
@********************************************************************************************************
@                                            TC32 EXCEPTION VECTORS
@********************************************************************************************************

	.section	.vectors,"ax"
	.global		__reset
	.global 	__start
	.global		__RAM_START_ADDR 
	.global		__RAM_SIZE_MAX 

__start:					@ MUST,  referenced by boot.link

	.extern  _ramcode_size_div_16_
	.extern  _ramcode_size_div_256_
	.extern  _ictag_start_
	.extern  _ictag_end_

	.org 0x0
	tj	__reset
	.short (0x1234) @ user code
	
	.org 0x4
	.word	(0)	@ user code
	.word	(0x544c4e4b)	@ ID
	.word	(0x00880000 + _ramcode_size_div_16_)

	.org 0x10
#if USE_IRQ
	tj	__irq
#else
	tj	__reset
#endif
	.org 0x18
	.word	(_bin_size_)
	.word	(0) 		@ user code ?
@********************************************************************************************************
@                                   LOW-LEVEL INITIALIZATION
@********************************************************************************************************
	.extern  main
	.org 0x20
__reset:
@********************************************************************************************************
@				Send flash cmd 0xab to wakeup flash
@********************************************************************************************************
#if USE_FLASH_WAKEUP
FLASH_WAKEUP_BEGIN:
	tloadr      r0,FLASH_RECOVER + 0
	tmov		r1,#0
	tstorerb    r1,[r0,#1]
	tmov        r1,#171						@Flash deep cmd: 0xAB
	tstorerb    r1,[r0,#0]
	tmov		r2,#0
	tmov        r3,#6
TNOP:
	tadd        r2,#1
	tcmp        r2,r3
	tjle        TNOP
	tmov		r1,#1
	tstorerb    r1,[r0,#1]
#endif
@********************************************************************************************************
@                              		 FILL .DATA AND .BSS WITH 0xFF
@********************************************************************************************************
#if USE_FLL_D
	tloadr	r0, FLL_D					@r0 = 0xffffffff
	tloadr	r1, FLL_D+4					@r1 = _start_data_
	tloadr	r2, FLL_D+8     			@r2 = _start_data_ + 32

FLL_STK:
	tcmp	r1, r2
	tjge	FLL_STK_END
	tstorer r0, [r1, #0]
	tadd    r1, #4
	tj		FLL_STK
FLL_STK_END:
#endif
@********************************************************************************************************
@                              		 UPDATE SP UNDER IRQ/SVC MODE
@********************************************************************************************************
	tmovs   r0,#0x12					@r0 = 0x12 IRQ
	tmcsr	r0							@CPSR=r0
	tloadr	r0, DAT0 + 0				@r0 = __RAM_START_ADDR + __RAM_SIZE_MAX
	tmov	r13, r0  					@r13/SP= r0    	update SP under IRQ mode

	tmovs   r0,#0x13					@r0 = 0x13 SVC
	tmcsr	r0							@CPSR=r0
	tloadr	r0, DAT0 + 4				@r0 = __RAM_START_ADDR + __RAM_SIZE_MAX - IRQ_STK_SIZE
	tmov	r13, r0  					@r13= r0		update SP under SVC mode

@********************************************************************************************************
@                                    .BSS INITIALIZATION FOR 0
@********************************************************************************************************
	tmov	r0, #0
	tloadr	r1, DAT0 + 8				@r1 = _start_bss_
	tloadr	r2, DAT0 + 12				@r2 = _end_bss_

ZERO:
	tcmp	r1, r2
	tjge	ZERO_END
	tstorer	r0, [r1, #0]
	tadd    r1, #4
	tj		ZERO
ZERO_END:

@********************************************************************************************************
@                                    IC TAG INITIALIZATION
@********************************************************************************************************
#if USE_ICACHE
ZERO_TAG:
	tmov    r0, #0
	tloadr	r1, DAT0 + 24				@r1 = _ictag_start_
	tloadr	r2, DAT0 + 28				@r2 = _ictag_end_
ZERO_TAG_BEGIN:
	tcmp	r1, r2
	tjge	ZERO_TAG_END				@r1>=r2 jump to ZERO_TAG_END
	tstorer	r0, [r1, #0]				@*(unsigned int*)(_ictag_start_)=r0=0
	tadd    r1, #4						@r1 = r1 + 4
	tj		ZERO_TAG_BEGIN				@jump to ZERO_TAG_BEGIN
ZERO_TAG_END:

@********************************************************************************************************
@                                    IC CACHE INITIALIZATION
@********************************************************************************************************
SETIC:
	tloadr     	r1, DAT0 + 20			@ r1 = 0x80060c
	tloadr      r0, DAT0 + 16			@ IC tag start  r0 = _ramcode_size_div_256_
	tstorerb	r0, [r1, #0] 			@*(unsigned int*)(0x80060c) = r0
	tadd    	r0, #1					@ IC tag end
	tstorerb	r0, [r1, #1]			@ *(unsigned int*)(0x80060d) = r0
	@tmov		r0, #0;
	@tstorerb	r0, [r1, #2]
#endif
@********************************************************************************************************
@                                    DATA SECTION LOAD
@********************************************************************************************************
#if USE_DATA_I
	tloadr		r1, DATA_I				@ r1 = _dstored_
	tloadr		r2, DATA_I+4			@ r2 = _start_data_
	tloadr		r3, DATA_I+8			@ r3 = _end_data_
COPY_DATA:
	tcmp		r2, r3
	tjge		COPY_DATA_END			@ r2>=r3 jump to COPY_DATA_END
	tloadr		r0, [r1, #0]			@ r0 = *(unsigned int*)(_dstored_)
	tstorer 	r0, [r2, #0]			@ *(unsigned int*)(_start_data_) = r0
	tadd    	r1, #4					@ r1 = r1 + 4
	tadd		r2, #4					@ r2 = r2 + 4
	tj			COPY_DATA				@ jump to COPY_DATA
COPY_DATA_END:
#endif
#if USE_SETSPISPEED
SETSPISPEED:
	tloadr     	r1, DAT0 + ?
	tmov		r0, #0xbb				@ 0x0b for fast read; 0xbb for dual dat/adr
	tstorerb	r0, [r1, #0]
	tmov		r0, #3					@ 3 for dual dat/adr
	tstorerb	r0, [r1, #1]
#endif

	tjl	main

@********************************************************************************************************
@                                    IRQ CODE
@********************************************************************************************************
#if USE_IRQ
	.extern irq_handler
	.align 4
__irq:
	tpush    	{r14}
	tpush    	{r0-r7}
	tmrss    	r0

	tmov		r1, r8
	tmov		r2, r9
	tmov		r3, r10
	tmov		r4, r11
	tmov		r5, r12
	tpush		{r0-r5}

	@tjl      	irq_handler

	tpop		{r0-r5}
	tmov		r8, r1
	tmov		r9, r2
	tmov		r10,r3
	tmov		r11,r4
	tmov		r12,r5

	tmssr    	r0
	tpop		{r0-r7}
	treti    	{r15}
#endif

@********************************************************************************************************
@									DATA
@********************************************************************************************************
	.balign	4
DAT0:
	.word	(__RAM_START_ADDR + __RAM_SIZE_MAX)		    @0  irq stack end
	.word	(__RAM_START_ADDR + __RAM_SIZE_MAX - IRQ_STK_SIZE)	    @4  stack end
	.word	(_start_bss_)               @8
	.word	(_end_bss_)                 @12
#if USE_ICACHE
	.word	_ramcode_size_div_256_      @16
	.word	(0x80060c)                  @20
	.word	_ictag_start_               @24		@ IC tag start
	.word	_ictag_end_	            	@28		@ IC tag end
#endif
#if USE_DATA_I
DATA_I:	
	.word	(_dstored_)					@0
	.word	(_start_data_)				@4
	.word	(_end_data_)				@8
#endif
#if USE_FLASH_WAKEUP
FLASH_RECOVER:
	.word	(0x80000c)                  @0
#endif
#if USE_FLL_D
FLL_D:
	.word	0xffffffff					@0
	.word	(_start_data_)				@4
	.word	(_start_data_ + 32)			@8 @just clear Xk to decrease time. @(_end_bss_)
#endif
ASMEND:

	.end

