#include "common.h"

_attribute_ram_code_
static inline void analog_wait(){
	while(reg_ana_ctrl & FLD_ANA_BUSY){}
}

_attribute_ram_code_ u8 analog_read(u8 addr){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif
	reg_ana_addr = addr;
	reg_ana_ctrl = (FLD_ANA_START);
	analog_wait();
	u8 data = reg_ana_data;
	reg_ana_ctrl = 0;		// finish
#if USE_IRQ_SAVE
	irq_restore(r);
#endif
	return data;
}

_attribute_ram_code_ void analog_write(u8 addr, u8 v){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif
	reg_ana_addr = addr;
	reg_ana_data = v;
	reg_ana_ctrl = (FLD_ANA_START | FLD_ANA_RW);
	analog_wait();
	reg_ana_ctrl = 0; 		// finish
#if USE_IRQ_SAVE
	irq_restore(r);
#endif
}

#if 0
_attribute_ram_code_ void analog_read_blk(u8 addr, u8 *v, int len){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif	
	reg_ana_ctrl = 0;		// issue clock
	reg_ana_addr = addr;
	while(len--){
		reg_ana_ctrl = FLD_ANA_CYC | FLD_ANA_START;
		analog_wait();
		*v++ = reg_ana_data;
	}
	reg_ana_ctrl = 0; 		// finish
#if USE_IRQ_SAVE
	irq_restore(r);
#endif	
}

_attribute_ram_code_ void analog_write_blk(u8 addr, u8 *v, int len){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif	
	reg_ana_addr = addr;
	while(len--){
		reg_ana_data = *v++;
		reg_ana_ctrl = FLD_ANA_CYC | FLD_ANA_START | FLD_ANA_RW; 	// multi write
		analog_wait();
	}
	reg_ana_ctrl = 0; 		// finish
#if USE_IRQ_SAVE
	irq_restore(r);
#endif	
}

#endif

_attribute_ram_code_
void gpio_setup_up_down_resistor(u32 gpio, u32 up_down) {
	 u8 pin = gpio & 0xff;
	 u8 base_ana_reg = 0x0b + ((gpio >> 8) << 1);

	 if(pin & 0x03) {
		 base_ana_reg -= 1;
	 }
	 else if(pin & 0xc0) {
		 base_ana_reg += 1;
	 }

	 u8 mask_not = 0xfc;   //default for  PX2  PX6
	 u8 shift_num = 0;

	 if(pin & 0x88) { //PX3  PX7
		  mask_not = 0xf3;
		  shift_num = 2;
	 }
	 else if(pin & 0x11) {   //PX0  PX4
		 mask_not = 0xcf;
		 shift_num = 4;
	 }
	 else if(pin & 0x22) {   //PX1  PX5
		 mask_not = 0x3f;
		 shift_num = 6;
	 }

	 analog_write(base_ana_reg, (analog_read(base_ana_reg) & mask_not) | (up_down << shift_num));
}

