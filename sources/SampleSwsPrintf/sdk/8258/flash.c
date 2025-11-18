/********************************************************************************************************
 * @file     flash.c 
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


#include "flash.h"
#include "spi_i.h"
#include "irq.h"
#include "timer.h"
#include "watchdog.h"

_attribute_ram_code_ static inline int flash_is_busy(){
	return mspi_read() & 0x01;				//  the busy bit, pls check flash spec
}

/**
 * @brief     This function serves to set flash write command.
 * @param[in] cmd - set command.
 * @return    none
 */
_attribute_ram_code_ static void flash_send_cmd(unsigned char cmd){
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

/**
 * @brief     This function serves to send flash address.
 * @param[in] addr - the flash address.
 * @return    none
 */
_attribute_ram_code_ static void flash_send_addr(unsigned int addr){
	mspi_write((unsigned char)(addr>>16));
	mspi_wait();
	mspi_write((unsigned char)(addr>>8));
	mspi_wait();
	mspi_write((unsigned char)(addr));
	mspi_wait();
}

/**
 * @brief     This function serves to wait flash done.
 *            (make this a asynchorous version).
 * @param[in] none.
 * @return    none.
 */
_attribute_ram_code_ static void flash_wait_done(void)
{
	sleep_us(100);
	flash_send_cmd(FLASH_READ_STATUS_CMD);

	int i;
	for(i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
	mspi_high();
}


/**
 * @brief This function serves to erase a sector.
 * @param[in]   addr the start address of the sector needs to erase.
 * @return none
 */
_attribute_ram_code_ void flash_erase_sector(unsigned long addr){
	unsigned char r = irq_disable();

	wd_clear();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_SECT_ERASE_CMD);
	flash_send_addr(addr);
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}


/**
 * @brief This function writes the buffer's content to a page.
 * @param[in]   addr the start address of the page
 * @param[in]   len the length(in byte) of content needs to write into the page
 * @param[in]   buf the start address of the content needs to write into
 * @return none
 */
_attribute_ram_code_ void flash_write_page(unsigned long addr, unsigned long len, unsigned char *buf){
	unsigned char r = irq_disable();

	// important:  buf must not reside at flash, such as constant string.  If that case, pls copy to memory first before write
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_CMD);
	flash_send_addr(addr);

	unsigned int i;
	for(i = 0; i < len; ++i){
		mspi_write(buf[i]);		/* write data */
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

/**
 * @brief This function reads the content from a page to the buf.
 * @param[in]   addr the start address of the page
 * @param[in]   len the length(in byte) of content needs to read out from the page
 * @param[out]  buf the start address of the buffer
 * @return none
 */
_attribute_ram_code_ void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf){
	unsigned char r = irq_disable();


	flash_send_cmd(FLASH_READ_CMD);
	flash_send_addr(addr);

	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
	mspi_wait();
	/* get data */
	for(int i = 0; i < len; ++i){
		*buf++ = mspi_get();
		mspi_wait();
	}
	mspi_high();

	irq_restore(r);
}


/* according to your appliaction */
#if 1

/**
 * @brief This function serves to erase a page(256 bytes).
 * @param[in]   addr the start address of the page needs to erase.
 * @return none
 */
_attribute_ram_code_ void flash_erase_page(unsigned int addr)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_PAGE_ERASE_CMD);
	flash_send_addr(addr);
	mspi_high();
	flash_wait_done();

    irq_restore(r);
}

/**
 * @brief This function serves to erase a block(32k).
 * @param[in]   addr the start address of the block needs to erase.
 * @return none
 */
_attribute_ram_code_ void flash_erase_32kblock(unsigned int addr)
{
	unsigned char r = irq_disable();

	wd_clear();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_32KBLK_ERASE_CMD);
	flash_send_addr(addr);
	mspi_high();
	flash_wait_done();

    irq_restore(r);
}

/**
 * @brief This function serves to erase a block(64k).
 * @param[in]   addr the start address of the block needs to erase.
 * @return none
 */
_attribute_ram_code_ void flash_erase_64kblock(unsigned int addr)
{
	unsigned char r = irq_disable();

	wd_clear();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_64KBLK_ERASE_CMD);
	flash_send_addr(addr);
	mspi_high();
	flash_wait_done();

    irq_restore(r);
}

/**
 * @brief This function serves to erase a page(256 bytes).
 * @param[in]   addr the start address of the page needs to erase.
 * @return none
 */
_attribute_ram_code_ void flash_erase_chip(void)
{
	unsigned char r = irq_disable();

	wd_clear();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_CHIP_ERASE_CMD);
	mspi_high();
	flash_wait_done();

    irq_restore(r);
}

/**
 * @brief This function write the status of flash.
 * @param[in]  the value of status
 * @return status
 */
_attribute_ram_code_ unsigned char flash_write_status(unsigned char data)
{
	unsigned char r = irq_disable();
	unsigned char result;
	//int i;
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_STATUS_CMD);
	mspi_write(data);
	mspi_wait();
	mspi_high();
	flash_wait_done();

	sleep_us(100);
	flash_send_cmd(FLASH_READ_STATUS_CMD);

	result = mspi_read();
	mspi_high();

	irq_restore(r);
	return  result;
}

/**
 * @brief This function reads the status of flash.
 * @param[in]  none
 * @return none
 */
_attribute_ram_code_ unsigned char flash_read_status(void){
	unsigned char r = irq_disable();
	unsigned char status =0;
	flash_send_cmd(FLASH_READ_STATUS_CMD);
	/* get low 8 bit status */
	status = mspi_read();
	mspi_high();
	irq_restore(r);
	return status;
}




/**
 * @brief  	Deep Power Down mode to put the device in the lowest consumption mode
 * 			it can be used as an extra software protection mechanism,while the device
 * 			is not in active use,since in the mode,  all write,Program and Erase commands
 * 			are ignored,except the Release from Deep Power-Down and Read Device ID(RDI)
 * 			command.This release the device from this mode
 * @param[in] none
 * @return none.
 */
_attribute_ram_code_ void flash_deep_powerdown(void)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_POWER_DOWN);
	mspi_high();
	sleep_us(1);

    irq_restore(r);
}

/**
 * @brief		The Release from Power-Down or High Performance Mode/Device ID command is a
 * 				Multi-purpose command.it can be used to release the device from the power-Down
 * 				State or High Performance Mode or obtain the devices electronic identification
 * 				(ID)number.Release from Power-Down will take the time duration of tRES1 before
 * 				the device will resume normal operation and other command are accepted.The CS#
 * 				pin must remain high during the tRES1(8us) time duration.
 * @param[in] none
 * @return none.
 */
_attribute_ram_code_ void flash_release_deep_powerdown(void)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_POWER_DOWN_RELEASE);
	mspi_high();
	flash_wait_done();
	mspi_high();

    irq_restore(r);
}
/**
 * @brief	  MAC id. Before reading UID of flash, you must read MID of flash. and then you can
 *            look up the related table to select the idcmd and read UID of flash
 * @param[in] buf - store MID of flash
 * @return    none.
 */
_attribute_ram_code_ void flash_read_mid(unsigned char *buf){
	unsigned char j = 0;
	unsigned char r = irq_disable();
	flash_send_cmd(FLASH_GET_JEDEC_ID);
	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);	/* auto mode */
	mspi_wait();

	for(j = 0; j < 3; ++j){
		*buf++ = mspi_get();
		mspi_wait();
	}
	mspi_high();

	irq_restore(r);
}
/**
 * @brief	  UID. Before reading UID of flash, you must read MID of flash. and then you can
 *            look up the related table to select the idcmd and read UID of flash
 * @param[in] idcmd - get this value to look up the table based on MID of flash
 * @param[in] buf   - store UID of flash
 * @return    none.
 */
_attribute_ram_code_ void flash_read_uid(unsigned char idcmd,unsigned char *buf)
{
	unsigned char j = 0;
	unsigned char r = irq_disable();
	flash_send_cmd(idcmd);
	if(idcmd==FLASH_GD_PUYA_READ_UID_CMD)				//< GD/puya
	{
		flash_send_addr(0x00);
		mspi_write(0x00);		/* dummy,  to issue clock */
		mspi_wait();
	}
	else if (idcmd==FLASH_XTX_READ_UID_CMD)		//< XTX
	{
		flash_send_addr(0x80);
		mspi_write(0x00);		/* dummy,  to issue clock */
		mspi_wait();

	}
	mspi_write(0x00);			/* dummy,  to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);		/* auto mode */
	mspi_wait();

	for(j = 0; j < 16; ++j){
		*buf++ = mspi_get();
		mspi_wait();
	}
	mspi_high();
	irq_restore(r);
}
/**
 * @brief 		 This function serves to read flash mid and uid,and check the correctness of mid and uid.
 * @param[out]   flash_mid - Flash Manufacturer ID
 * @param[out]   flash_uid - Flash Unique ID
 * @return       0:error 1:ok

 */
_attribute_ram_code_ int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid)
{
	  unsigned char no_uid[16]={0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01};
	  int i,f_cnt=0;
	  unsigned int mid;
	  flash_read_mid((unsigned char*)&mid);
	  mid = mid&0xffff;
	  *flash_mid  = mid;
	 /*
	   Flash Type    CMD        MID      Company
	   GD25LD40C 	 0x4b     0x60C8     GD
	   GD25LD05C  	 0x4b 	  0x60C8     GD
	   P25Q40L   	 0x4b     0x6085     PUYA
	   MD25D40DGIG	 0x4b     0x4051     GD
	   GD25D10C      0x4b     0x40C8     GD
	   PN25F04C      0x5a     0x311C     XTX
	 */
	  if( (mid == 0x60C8) || (mid == 0x6085) ||(mid == 0x4051)||(mid==0x40C8)){
		  flash_read_uid(FLASH_GD_PUYA_READ_UID_CMD,(unsigned char *)flash_uid);
	  }
	  else if(mid==0x311C){
		  flash_read_uid(FLASH_XTX_READ_UID_CMD	,(unsigned char *)flash_uid);
	  }
	  else{
		  return 0;
	  }
	  for(i=0;i<16;i++){
		if(flash_uid[i]==no_uid[i]){
			f_cnt++;
		}
	  }
	  if(f_cnt==16){//no uid flash
			  return 0;

	  }else{
		  return  1;
	  }
}
/**
 * @brief This function serves to protect data for flash.
 * @param[in]   type - flash type include GD,Puya and XTX
 * @param[in]   data - refer to Driver API Doc or Flash_ProtectedAreaDef in "flash.h".
 * @return none
 */
_attribute_ram_code_ void flash_lock(Flash_TypeDef type , unsigned short data)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_STATUS_CMD);
	if ((type == FLASH_TYPE_GD)||(type == FLASH_TYPE_XTX)){
		mspi_write((unsigned char)data);   //8 bit status
	}else if(type == FLASH_TYPE_PUYA){

		mspi_write((unsigned char)data);
		mspi_wait();
		mspi_write((unsigned char)(data>>8));//16bit status

	}
	mspi_wait();
	mspi_high();
	flash_wait_done();
	sleep_us(100);
	mspi_high();
	irq_restore(r);
}

/**
 * @brief This function serves to protect data for flash.
 * @param[in]   type - flash type include GD,Puya and XTX
 * @return none
 */
_attribute_ram_code_ void flash_unlock(Flash_TypeDef type)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_STATUS_CMD);
	if ((type == FLASH_TYPE_GD)||(type == FLASH_TYPE_XTX)){
		mspi_write(0);   //8 bit status
	}else if(type == FLASH_TYPE_PUYA){

		mspi_write(0);
		mspi_wait();
		mspi_write(0);//16bit status

	}
	mspi_wait();
	mspi_high();
	flash_wait_done();
	sleep_us(100);
	mspi_high();
	irq_restore(r);
}
#endif

#if(FLASH_TYPE==GD25LE80C)


/**
 * @brief This function serves to read data from the Security Registers of the flash.
 *        You can read a byte one time.
 *              the address of the  Security Registers #1 0x001000-0x0011ff
 *              the address of the  Security Registers #1 0x002000-0x0021ff
 *              the address of the  Security Registers #1 0x003000-0x0031ff
 *
 * @param[in]   the address of the data
 * @return      the data
 */
_attribute_ram_code_ unsigned char flash_read_otp_byte(unsigned long addr)
{
	unsigned char r = irq_disable();
	unsigned char value=0;


	flash_send_cmd(FLASH_READ_SECURITY_REGISTERS_CMD);
	flash_send_addr(addr);

	mspi_write(0x00);		/* dummy,  to issue clock */
	mspi_wait();

	/* get data */

	value= mspi_read();
	mspi_wait();

	mspi_high();
	flash_wait_done();
	irq_restore(r);
	return value;
}
/**
 * @brief This function serves to write data to the Security Registers of the flash you choose.
 *        You can write data on the address below.
 *        Attention:after you lock the the Security Registers,you can't write or erase the area anymore.
 *              the address of the  Security Registers #1 0x001000-0x0011ff
 *              the address of the  Security Registers #2 0x002000-0x0021ff
 *              the address of the  Security Registers #3 0x003000-0x0031ff
 *
 * @param[in]   addr: the start address of the data
 * @param[in]   len : the length(in byte) of content needs to write
 * @param[in]   buf : the start address of the content needs to write into
 * @return      none
 */

_attribute_ram_code_ void flash_write_otp(unsigned long addr, unsigned long len, unsigned char *buf){
	unsigned char r = irq_disable();

	// important:  buf must not reside at flash, such as constant string.  If that case, pls copy to memory first before write
	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_SECURITY_REGISTERS_CMD);
	flash_send_addr(addr);

	for(int i = 0; i < len; i++){
	mspi_write(buf[i]);		/* write data */
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

/**
 * @brief This function serves to erase the data of the Security Registers that you choose.You can
 *        erase 512-byte one time.(Even you choose the middle area of the Security Registers,it will
 *        rease the whole area.)
 *       Attention:after you lock the the Security Registers,you can't write or erase the area anymore.
 *              the address of the  Security Registers #1 0x001000-0x0011ff
 *              the address of the  Security Registers #2 0x002000-0x0021ff
 *              the address of the  Security Registers #3 0x003000-0x0031ff
 *
 * @param[in]   the address that you want to erase.
 * @return      none
 */
_attribute_ram_code_ void flash_erase_otp_512byte(unsigned long addr){
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_ERASE_SECURITY_REGISTERS_CMD);
	flash_send_addr(addr);
	mspi_wait();

	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

/**
 * @brief This function serves to lock the Security Registers that you choose.Once you lock the
 *        Security Register,it will be permanently protected.It means the erase and write instruction
 *        will be ignored.
 *              the address of the  Security Registers #1 0x001000-0x0011ff   lock instruction:FLASH_LOCK_OTP_0x001000
 *              the address of the  Security Registers #2 0x002000-0x0021ff   lock instruction:FLASH_LOCK_OTP_0x001000
 *              the address of the  Security Registers #3 0x003000-0x0031ff   lock instruction:FLASH_LOCK_OTP_0x001000
 *
 * @param[in]   the lock instruction ,you can choose the aera that you want to lock.
 * @return      none
 */

_attribute_ram_code_ void flash_lock_otp(Flash_OtpDef lock_type){
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(FLASH_WRITE_STATUS_CMD);

	mspi_write(0x00);
	mspi_wait();
	mspi_write(lock_type);
	mspi_wait();

	mspi_high();
	flash_wait_done();
	irq_restore(r);
}

#endif
