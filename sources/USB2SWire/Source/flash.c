#include "common.h"
#include "spi_i.h"
#include "flash.h"

extern _attribute_ram_code_ void sleep_us (u32 us);

#if USE_EXT_FLASH
#else
_attribute_ram_code_ static inline int flash_is_busy(){
	return mspi_read() & 0x01;				//  the busy bit, pls check flash spec
}
#endif

_attribute_ram_code_ static void flash_send_cmd(u8 cmd){
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

_attribute_ram_code_ static void flash_send_addr(u32 addr){
	mspi_write((u8)(addr>>16));
	mspi_wait();
	mspi_write((u8)(addr>>8));
	mspi_wait();
	mspi_write((u8)(addr));
	mspi_wait();
}

//  make this a asynchorous version
_attribute_ram_code_ static void flash_wait_done()
{
	sleep_us(100);
	flash_send_cmd(FLASH_READ_STATUS_CMD);
#if USE_EXT_FLASH
	reg_spi_ctrl |= MASK_VAL(FLD_SPI_RD, 1, FLD_SPI_DATA_OUT_DIS, 1);  //enable read and disable output
	mspi_get();
	int i;
	for(i = 0; i < 10000000; ++i){
		mspi_wait();
		if(!(mspi_get() & 0x01)){ //  the busy bit, pls check flash spec
			break;
		}
	}
#else
	int i;
	for(i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
#endif
	mspi_high();
}

_attribute_ram_code_ void flash_erase_all(void){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif

	WATCHDOG_CLEAR;  //in case of watchdog timeout

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_CHIP_ERASE_CMD);
	mspi_high();
	flash_wait_done();

#if USE_IRQ_SAVE
	irq_restore(r);
#endif
}

_attribute_ram_code_ void flash_erase_sector(u32 addr){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif

	WATCHDOG_CLEAR;  //in case of watchdog timeout

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_SECT_ERASE_CMD);
	flash_send_addr(addr);
	mspi_high();
	flash_wait_done();

#if USE_IRQ_SAVE
	irq_restore(r);
#endif
}

_attribute_ram_code_ void flash_write_page(u32 addr, u32 len, u8 *buf){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif
#if USE_EXT_FLASH
	BM_CLR(reg_spi_ctrl, BIT(5) | FLD_SPI_DATA_OUT_DIS | FLD_SPI_RD); //enable output, enable write, disabling share mode
#endif
	// important:  buf must not reside at flash, such as constant string.  If that case, pls copy to memory first before write
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_CMD);
	flash_send_addr(addr);

	u32 i;
	for(i = 0; i < len; ++i){
		mspi_write(buf[i]);		/* write data */
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();

#if USE_IRQ_SAVE
	irq_restore(r);
#endif
}

_attribute_ram_code_ void flash_read_page(u32 addr, u32 len, u8 *buf){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif
#if USE_EXT_FLASH
	BM_CLR(reg_spi_ctrl, BIT(5) | FLD_SPI_DATA_OUT_DIS | FLD_SPI_RD); //enable output, enable write
#endif
	flash_send_cmd(FLASH_READ_CMD);
	flash_send_addr(addr);
#if USE_EXT_FLASH
	reg_spi_ctrl |= MASK_VAL(FLD_SPI_RD, 1, FLD_SPI_DATA_OUT_DIS, 1);  //enable read and disable output
	mspi_get();
#else
	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
#endif
	mspi_wait();
	/* get data */
	u32 i;
	for(i = 0; i < len; ++i){
		*buf++ = mspi_get();
		mspi_wait();
	}
	mspi_high();

#if USE_IRQ_SAVE
	irq_restore(r);
#endif
}

#if 1
// out 3 bytes: manufacturer, mem_type, cap_id
_attribute_ram_code_ void flash_get_jedec_id(u8 *p){
#if USE_IRQ_SAVE
	u8 r = irq_disable();
#endif
#if USE_EXT_FLASH
	BM_CLR(reg_spi_ctrl, BIT(5) | FLD_SPI_DATA_OUT_DIS | FLD_SPI_RD); //enable output, enable write
#endif
	flash_send_cmd(FLASH_GET_JEDEC_ID);
#if USE_EXT_FLASH
	reg_spi_ctrl |= MASK_VAL(FLD_SPI_RD, 1, FLD_SPI_DATA_OUT_DIS, 1);  //enable read and disable output
	mspi_get();
	mspi_wait();
	*p++  = mspi_get(); // manufacturer
	mspi_wait();
	*p++  = mspi_get(); // mem_type
	mspi_wait();
	*p  = mspi_get(); // cap_id
#else
	*p++  = mspi_read(); // manufacturer
	*p++  = mspi_read(); // mem_type
	*p  = mspi_read(); // cap_id
#endif
	mspi_high();
#if USE_IRQ_SAVE
	irq_restore(r);
#endif
}
#endif

