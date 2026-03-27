#ifndef GPIO_H
#define GPIO_H

// PS  terrible code!

//bit operations
//#define BIT(n)                  		( 1<<(n) )
//#define BM_SET(addr,bit)                ( *(addr) |= (bit) )
//#define BM_CLR(addr,bit)                ( *(addr) &= ~(bit) )
//#define BM_IS_SET(addr,bit)             ( *(addr) & (bit) )

typedef enum{
	Bit_RESET = 0,
	Bit_SET,
}BitAction;


//reg addr
#define REG_BASE                        0x800000
#define REG_ADDR_8(addr)                 (*(volatile unsigned char  *)(REG_BASE+addr))

#define gpio_write_reg32(addr,data)      (*(volatile unsigned long  *)(addr)=data)
#define gpio_write_reg16(addr,data)      (*(volatile unsigned short *)(addr)=data)
#define gpio_write_reg8(addr,data)       (*(volatile unsigned char  *)(addr)=data)
#define gpio_read_reg32(addr)            (*(volatile unsigned long  *)(addr))
#define gpio_read_reg16(addr)            (*(volatile unsigned short *)(addr))
#define gpio_read_reg8(addr)             (*(volatile unsigned char  *)(addr))

//GPIO types
typedef enum{
	GPIO_GROUPA 	= 0x000,
	GPIO_GROUPB 	= 0x100,
	GPIO_GROUPC 	= 0x200,
	GPIO_GROUPD 	= 0x300,
	GPIO_GROUPE 	= 0x400,
	GPIO_GROUPF 	= 0x500,
}GPIO_Group;

//GPIO types
typedef enum{
	PULL_NONE 	    = 0,
	PULL_UP_1M 	    = 1,
	PULL_UP_10K 	= 2,
	PULL_DOWN_100K 	= 3,
}GPIO_PullType;


enum{
	    GPIO_PA0 = 0x000 | BIT(0),
		GPIO_PA1 = 0x000 | BIT(1),
		GPIO_PA2 = 0x000 | BIT(2),
		GPIO_PA3 = 0x000 | BIT(3),
		GPIO_PA4 = 0x000 | BIT(4),
		GPIO_PA5 = 0x000 | BIT(5),
		GPIO_PA6 = 0x000 | BIT(6),
		GPIO_PA7 = 0x000 | BIT(7),

		GPIO_PB0 = 0x100 | BIT(0),
		GPIO_PB1 = 0x100 | BIT(1),
		GPIO_PB2 = 0x100 | BIT(2),
		GPIO_PB3 = 0x100 | BIT(3),
		GPIO_PB4 = 0x100 | BIT(4),
		GPIO_PB5 = 0x100 | BIT(5),
		GPIO_PB6 = 0x100 | BIT(6),
		GPIO_PB7 = 0x100 | BIT(7),

		GPIO_PC0 = 0x200 | BIT(0),
		GPIO_PC1 = 0x200 | BIT(1),
		GPIO_PC2 = 0x200 | BIT(2),
		GPIO_PC3 = 0x200 | BIT(3),
		GPIO_PC4 = 0x200 | BIT(4),
		GPIO_PC5 = 0x200 | BIT(5),
		GPIO_PC6 = 0x200 | BIT(6),
		GPIO_PC7 = 0x200 | BIT(7),

		GPIO_PD0 = 0x300 | BIT(0),
		GPIO_PD1 = 0x300 | BIT(1),
		GPIO_PD2 = 0x300 | BIT(2),
		GPIO_PD3 = 0x300 | BIT(3),
		GPIO_PD4 = 0x300 | BIT(4),
		GPIO_PD5 = 0x300 | BIT(5),
		GPIO_PD6 = 0x300 | BIT(6),
		GPIO_PD7 = 0x300 | BIT(7),

		GPIO_PE0 = 0x400 | BIT(0),
		GPIO_PE1 = 0x400 | BIT(1),
		GPIO_PE2 = 0x400 | BIT(2),
		GPIO_PE3 = 0x400 | BIT(3),
		GPIO_PE4 = 0x400 | BIT(4),
		GPIO_PE5 = 0x400 | BIT(5),
		GPIO_PE6 = 0x400 | BIT(6),
		GPIO_PE7 = 0x400 | BIT(7),

		GPIO_PF0 = 0x500 | BIT(0),
		GPIO_PF1 = 0x500 | BIT(1),
		GPIO_PF2 = 0x500 | BIT(2),
		GPIO_PF3 = 0x500 | BIT(3),
		GPIO_PF4 = 0x500 | BIT(4),
		GPIO_PF5 = 0x500 | BIT(5),
		GPIO_PF6 = 0x500 | BIT(6),
		GPIO_PF7 = 0x500 | BIT(7)
};

typedef enum{
	GPIOA_GP0  = GPIO_GROUPA | BIT(0),
	GPIOA_GP1  = GPIO_GROUPA | BIT(1),
	GPIOA_GP2  = GPIO_GROUPA | BIT(2),
	GPIOA_GP3  = GPIO_GROUPA | BIT(3),
	GPIOA_GP4  = GPIO_GROUPA | BIT(4),
	GPIOA_GP5  = GPIO_GROUPA | BIT(5),
	GPIOA_GP6  = GPIO_GROUPA | BIT(6),
	GPIOA_GP7  = GPIO_GROUPA | BIT(7),
	GPIOA_ALL  = GPIO_GROUPA | 0X00ff,

	GPIOB_GP0  = GPIO_GROUPB | BIT(0),
	GPIOB_GP1  = GPIO_GROUPB | BIT(1),
	GPIOB_GP2  = GPIO_GROUPB | BIT(2),
	GPIOB_GP3  = GPIO_GROUPB | BIT(3),
	GPIOB_GP4  = GPIO_GROUPB | BIT(4),
	GPIOB_GP5  = GPIO_GROUPB | BIT(5),
	GPIOB_GP6  = GPIO_GROUPB | BIT(6),
	GPIOB_GP7  = GPIO_GROUPB | BIT(7),
	GPIOB_ALL  = GPIO_GROUPB | 0x00ff,

	GPIOC_GP0  = GPIO_GROUPC | BIT(0),
	GPIOC_GP1  = GPIO_GROUPC | BIT(1),
	GPIOC_GP2  = GPIO_GROUPC | BIT(2),
	GPIOC_GP3  = GPIO_GROUPC | BIT(3),
	GPIOC_GP4  = GPIO_GROUPC | BIT(4),
	GPIOC_GP5  = GPIO_GROUPC | BIT(5),
	GPIOC_GP6  = GPIO_GROUPC | BIT(6),
	GPIOC_GP7  = GPIO_GROUPC | BIT(7),
	GPIOC_ALL  = GPIO_GROUPC | 0x00ff,

	GPIOD_GP0  = GPIO_GROUPD | BIT(0),
	GPIOD_GP1  = GPIO_GROUPD | BIT(1),
	GPIOD_GP2  = GPIO_GROUPD | BIT(2),
	GPIOD_GP3  = GPIO_GROUPD | BIT(3),
	GPIOD_GP4  = GPIO_GROUPD | BIT(4),
	GPIOD_GP5  = GPIO_GROUPD | BIT(5),
	GPIOD_GP6  = GPIO_GROUPD | BIT(6),
	GPIOD_GP7  = GPIO_GROUPD | BIT(7),
	GPIOD_ALL  = GPIO_GROUPD | 0x00ff,

	GPIOE_GP0  = GPIO_GROUPE | BIT(0),
	GPIOE_GP1  = GPIO_GROUPE | BIT(1),
	GPIOE_GP2  = GPIO_GROUPE | BIT(2),
	GPIOE_GP3  = GPIO_GROUPE | BIT(3),
	GPIOE_GP4  = GPIO_GROUPE | BIT(4),
	GPIOE_GP5  = GPIO_GROUPE | BIT(5),
	GPIOE_GP6  = GPIO_GROUPE | BIT(6),
	GPIOE_GP7  = GPIO_GROUPE | BIT(7),
	GPIOE_ALL  = GPIO_GROUPE | 0x00ff,

	GPIOF_GP0  = GPIO_GROUPF | BIT(0),
	GPIOF_GP1  = GPIO_GROUPF | BIT(1),
	GPIOF_GP2  = GPIO_GROUPF | BIT(2),
	GPIOF_GP3  = GPIO_GROUPF | BIT(3),
	GPIOF_GP4  = GPIO_GROUPF | BIT(4),
	GPIOF_GP5  = GPIO_GROUPF | BIT(5),
	GPIOF_GP6  = GPIO_GROUPF | BIT(6),
	GPIOF_GP7  = GPIO_GROUPF | BIT(7),
	GPIOF_ALL  = GPIO_GROUPF | 0x00ff,

}GPIO_Pin;


#define reg_gpio_in(i)			REG_ADDR_8(0x580+((i>>8)<<3))
#define reg_gpio_ie(i)			REG_ADDR_8(0x581+((i>>8)<<3))
#define reg_gpio_oen(i)			REG_ADDR_8(0x582+((i>>8)<<3))
#define reg_gpio_out(i)			REG_ADDR_8(0x583+((i>>8)<<3))
#define reg_gpio_pol(i)			REG_ADDR_8(0x584+((i>>8)<<3))
#define reg_gpio_ds(i)			REG_ADDR_8(0x585+((i>>8)<<3))
#define reg_gpio_gpio_func(i)	REG_ADDR_8(0x586+((i>>8)<<3))
#define reg_gpio_irq_en(i)		REG_ADDR_8(0x587+((i>>8)<<3))

#define reg_gpio_config_func(i)		REG_ADDR8(0x5b0 +(i>>8))  	  //5b0 5b1 5b2 5b3 5b4 5b5

#define reg_gpio_irq_wakeup_en(i)	REG_ADDR8(0x587+((i>>8)<<3))  // reg_irq_mask: FLD_IRQ_GPIO_EN

#define reg_gpio_irq_risc0_en(i)  REG_ADDR8(0x5b8 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
#define reg_gpio_irq_risc1_en(i)  REG_ADDR8(0x5c0 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
#define reg_gpio_irq_risc2_en(i)  REG_ADDR8(0x5c8 + (i >> 8))     // reg_irq_mask: FLD_IRQ_GPIO_RISC2_EN

#define reg_gpio_wakeup_irq  REG_ADDR8(0x5b5)
enum{
    FLD_GPIO_CORE_WAKEUP_EN  = BIT(2),
    FLD_GPIO_CORE_INTERRUPT_EN = BIT(3),
};

static inline void gpio_core_wakeup_enable_all (int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_irq, FLD_GPIO_CORE_WAKEUP_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_irq, FLD_GPIO_CORE_WAKEUP_EN);
    }
}

static inline void gpio_core_irq_enable_all (int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_irq, FLD_GPIO_CORE_INTERRUPT_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_irq, FLD_GPIO_CORE_INTERRUPT_EN);
    }
}

static inline int gpio_is_output_en(u32 pin){
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

static inline int gpio_is_input_en(u32 pin){
	return BM_IS_SET(reg_gpio_ie(pin), pin & 0xff);
}

static inline void gpio_set_output_en(u32 pin, u32 value){
	u8	bit = pin & 0xff;
	if(!value){
		BM_SET(reg_gpio_oen(pin), bit);
	}else{
		BM_CLR(reg_gpio_oen(pin), bit);
	}
}

static inline void gpio_set_input_en(u32 pin, u32 value){
	u8	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_ie(pin), bit);
	}else{
		BM_CLR(reg_gpio_ie(pin), bit);
	}
}

static inline void gpio_set_data_strength(u32 pin, u32 value){
	u8	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_ds(pin), bit);
	}else{
		BM_CLR(reg_gpio_ds(pin), bit);
	}
}

static inline void gpio_write(u32 pin, u32 value){
	u8	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_out(pin), bit);
	}else{
		BM_CLR(reg_gpio_out(pin), bit);
	}
}

static inline void gpio_toggle(u32 pin) {
	reg_gpio_out(pin) ^= (pin & 0xFF);
}

static inline u32 gpio_read(u32 pin){
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}

static inline u32 gpio_read_cache(u32 pin, u8 *p){
	return p[pin>>8] & (pin & 0xff);
}

static inline void gpio_read_all(u8 *p){
	p[0] = REG_ADDR8(0x580);
	p[1] = REG_ADDR8(0x588);
	p[2] = REG_ADDR8(0x590);
	p[3] = REG_ADDR8(0x598);
	p[4] = REG_ADDR8(0x5a0);
	p[5] = REG_ADDR8(0x5a8);
}

static inline void gpio_set_interrupt_pol(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
	}
}

static inline void gpio_set_interrupt(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc0(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc0_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc0(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc1(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc1_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc1(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc2(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC2_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc2_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc2_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc2(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc2_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

#define rega_gpio_pull(pin) (0x0b+((pin>>8)<<1)-((pin&0x03)?1:0)+((pin&0xc0)?1:0))

#define shl_gpio_pull(pin) (((pin & 0x11)?4:0)+((pin & 0x22)?6:0)+((pin & 0x88)?2:0))

#if 1
void gpio_setup_up_down_resistor(u32 gpio, u32 up_down);
#define gpio_set_pull_resistor gpio_setup_up_down_resistor
#else
inline void gpio_set_pull_resistor(u32 gpio, u32 up_down) {
	analog_write(rega_gpio_pull(gpio),
		(analog_read(rega_gpio_pull(gpio))
			& (~(0x03<<shl_gpio_pull(gpio)))) | up_down << shl_gpio_pull(gpio));
}
#endif

#endif //GPIO_H 
