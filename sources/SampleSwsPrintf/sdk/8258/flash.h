/********************************************************************************************************
 * @file     flash.h 
 *
 * @brief    This is the header file for TLSR8258
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

#pragma once

#include "compiler.h"

#define   GD25LE80C       1
#define   FLASH_TYPE      GD25LE80C

/**
 * @brief     flash command definition
 */
enum{
	FLASH_WRITE_STATUS_CMD		=	0x01,
	FLASH_WRITE_CMD				=	0x02,
	FLASH_READ_CMD				=	0x03,
	FLASH_WRITE_DISABLE_CMD 	= 	0x04,
	FLASH_READ_STATUS_CMD		=	0x05,
	FLASH_WRITE_ENABLE_CMD 		= 	0x06,
	FLASH_SECT_ERASE_CMD		=	0x20,
	FLASH_GD_PUYA_READ_UID_CMD	=	0x4B,	//Flash Type = GD/PUYA
	FLASH_32KBLK_ERASE_CMD		=	0x52,
	FLASH_XTX_READ_UID_CMD		=	0x5A,	//Flash Type = XTX
	FLASH_CHIP_ERASE_CMD		=	0x60,   //or 0xc7
	FLASH_PAGE_ERASE_CMD		=	0x81,   //caution: only P25Q40L support this function
	FLASH_64KBLK_ERASE_CMD		=	0xD8,
	FLASH_POWER_DOWN			=	0xB9,
	FLASH_POWER_DOWN_RELEASE	=	0xAB,
	FLASH_GET_JEDEC_ID			=	0x9F,
	FLASH_READ_SECURITY_REGISTERS_CMD	=	0x48,
	FLASH_WRITE_SECURITY_REGISTERS_CMD	=	0x42,
	FLASH_ERASE_SECURITY_REGISTERS_CMD	=	0x44,

};
/**
 * @brief     the lock area of the Security Registers.
 *            flash:GD25LE80C
 */
typedef enum{
      FLASH_LOCK_OTP_0x001000        =   0x08,
      FLASH_LOCK_OTP_0x002000        =   0x10,
      FLASH_LOCK_OTP_0x003000        =   0x20,

}Flash_OtpDef;

/**
 * @brief     flash type definition
 */
 
typedef enum{
	FLASH_TYPE_GD = 0 ,
	FLASH_TYPE_XTX,
	FLASH_TYPE_PUYA
}Flash_TypeDef;

/**
 * @brief     flash capacity definition
 * Call flash_read_mid function to get the size of flash capacity.
 * Example is as follows:
 * unsigned char temp_buf[4];
 * flash_read_mid(temp_buf);
 *  The value of temp_buf[2] reflects flash capacity.
 */
typedef enum {
    FLASH_SIZE_64K     = 0x10,
    FLASH_SIZE_128K    = 0x11,
    FLASH_SIZE_256K    = 0x12,
    FLASH_SIZE_512K    = 0x13,
    FLASH_SIZE_1M      = 0x14,
    FLASH_SIZE_2M      = 0x15,
    FLASH_SIZE_4M      = 0x16,
    FLASH_SIZE_8M      = 0x17,
} Flash_CapacityDef;

/**
 * @brief     flash protected area
 * define the section of the memory array that can be read but not change.
 */
/*
enum{
//---------------------  GD flash  ----------------------------/
	GD25D05C_OR_GD25LD05C_32k    = 0x0c,//000000H-007FFFH
	GD25D05C_OR_GD25LD05C_48k    = 0x08,//000000H-00BFFFH
	GD25D05C_OR_GD25LD05C_56k	 = 0x04,//000000H-00DFFFH
	GD25D05C_OR_GD25LD05C_64k    = 0x10,//000000H-00FFFFH

	GD25D10BC_0R_GD25LD10C_64k   = 0x10,//000000H-00FFFFH
	GD25D10BC_0R_GD25LD10C_96k   = 0x0c,//000000H-017FFFH
	GD25D10BC_0R_GD25LD10C_112k  = 0x08,//000000H-01BFFFH
	GD25D10BC_0R_GD25LD10C_120k  = 0x04,//000000H-01DFFFH
	GD25D10BC_0R_GD25LD10C_128k  = 0x14,//000000H-01FFFFH

	MD25D20_128k    			 = 0x14,//000000H-01FFFFH
	MD25D20_192k                 = 0x10,//000000H-02FFFFH
	MD25D20_224k                 = 0x0c,//000000H-037FFFH
	MD25D20_240k                 = 0x08,//000000H-03BFFFH
	MD25D20_248k                 = 0x04,//000000H-03DFFFH
	MD25D20_256k                 = 0x18,//000000H-03FFFFH

	GD25LLD40C_OR_MD25D40D_256k  = 0x18,//000000H-03FFFFH
	GD25LLD40C_OR_MD25D40D_384k  = 0x14,//000000H-05FFFFH
	GD25LLD40C_OR_MD25D40D_448k  = 0x10,//000000H-06FFFFH
	GD25LLD40C_OR_MD25D40D_480k  = 0x0c,//000000H-077FFFH
	GD25LLD40C_OR_MD25D40D_496k  = 0x08,//000000H-07BFFFH
	GD25LLD40C_OR_MD25D40D_504k  = 0x04,//000000H-07DFFFH
	GD25LLD40C_OR_MD25D40D_512k  = 0x1c,//000000H-07FFFFH

	GD25LD80CGIG_768k  			 = 0x18,//000000H-0BFFFFH
	GD25LD80CGIG_896k			 = 0x14,//000000H-0DFFFFH
	GD25LD80CGIG_960k			 = 0x10,//000000H-0EFFFFH
	GD25LD80CGIG_992k			 = 0x0c,//000000H-0F7FFFH
	GD25LD80CGIG_1008k			 = 0x08,//000000H-0FBFFFH
	GD25LD80CGIG_1016k			 = 0x04,//000000H-0FDFFFH
	GD25LD80CGIG_1024k			 = 0x1c,//000000H-0FFFFFH

	MD25D80D_768K                = 0x18,//000000H-0BFFFFH
	MD25D80D_896K                = 0x14,//000000H-0DFFFFH
	MD25D80D_960K                = 0x10,//000000H-0EFFFFH
	MD25D80D_992K  				 = 0x0c,//000000H-0F7FFFH
	MD25D80D_1008K               = 0x08,//000000H-0FBFFFH
	MD25D80D_1016K               = 0x04,//000000H-0FBFFFH
	MD25D80D_1024K               = 0x1c,//000000H-0FFFFFH

//---------------------  XTX  flash  ----------------------------/
	PN25F04C_BLOCK0_64K          = 0x24,//000000H-00FFFFh
	PN25F04C_BLOCK7_64K          = 0x04,//070000H-07FFFFH
	PN25F04C_BLOCK0_to_1_128K    = 0x28,//000000H-01FFFFH
	PN25F04C_BLOCK6_to_7_128K    = 0x08,//060000H-07FFFFH
	PN25F04C_BLOCK0_to_3_256K    = 0x2c,//000000H-03FFFFH
	PN25F04C_BLOCK4_to_7_256K    = 0x0c,//040000H-07FFFFH
	PN25F04C_BLOCK0_to_5_384K    = 0x30,//000000H-05FFFFH
	PN25F04C_BLOCK2_to_7_384K    = 0x10,//020000H-07FFFFH
	PN25F04C_BLOCK0_to_6_448K    = 0x34,//000000H-06FFFFH
	PN25F04C_BLOCK1_to_7_448K    = 0x14,//010000H-07FFFFH

//---------------------  Puya flash  ----------------------------/
	// Setting of P25D40H is the same with P25Q40L
	P25Q40L_BLOCK0_4k            = 0x0064,//000000H-000FFFH
	P25Q40L_BLOCK7_4k            = 0x0044,//07F000H-07FFFFH
	P25Q40L_BLOCK0_8k            = 0x0068,//000000H-001FFFH
	P25Q40L_BLOCK7_8k            = 0x0048,//07E000H-07FFFFH
	P25Q40L_BLOCK0_16k           = 0x006c,//000000H-003FFFH
	P25Q40L_BLOCK7_16k           = 0x004c,//07C000H-07FFFFH
	P25Q40L_BLOCK0_32k           = 0x0078,//000000H-007FFFH
	P25Q40L_BLOCK7_32k           = 0x0058,//078000H-07FFFFH
	P25Q40L_BLOCK0_64K           = 0x0024,//000000H-00FFFFH
	P25Q40L_BLOCK7_64K           = 0x0004,//070000H-07FFFFH
	P25Q40L_BLOCK0_to_1_128k     = 0x0028,//000000H-01FFFFH
	P25Q40L_BLOCK6_to_7_128k     = 0x0008,//060000H-07FFFFH
	P25Q40L_BLOCK0_to_3_256k     = 0x002c,//000000H-03FFFFH (0x002c /0x400c)
	P25Q40L_BLOCK4_to_7_256k     = 0x000c,//040000H-07FFFFH (0x000c /0x402c)
	P25Q40L_BLOCK0_to_5_384k     = 0x4008,//000000H-05FFFFH
	P25Q40L_BLOCK2_to_7_384k     = 0x4028,//020000H-07FFFFH
	P25Q40L_BLOCK0_to_6_448k     = 0x4004,//000000H-06FFFFH
	P25Q40L_BLOCK1_to_7_448k     = 0x4024,//010000H-07FFFFH
	P25Q40L_BLOCK0to_7_480k_1    = 0x4058,//000000H-077FFFH
	P25Q40L_BLOCK0to_7_480k_2    = 0x4078,//008000H-07FFFFH
	P25Q40L_BLOCK0to_7_496k_1    = 0x404c,//000000H-07BFFFH
	P25Q40L_BLOCK0to_7_496k_2    = 0x406c,//004000H-07FFFFH
	P25Q40L_BLOCK0to_7_504k_1    = 0x4048,//000000H-07DFFFH
	P25Q40L_BLOCK0to_7_504k_2    = 0x4068,//002000H-07FFFFH
	P25Q40L_BLOCK0to_7_508k_1    = 0x4044,//000000H-07EFFFH
	P25Q40L_BLOCK0to_7_508k_2    = 0x4064,//001000H-07FFFFH
	P25Q40L_BLOCK0to_7_512k      = 0x007c,//000000H-07FFFFH

	P25Q06L_BLOCK0_4k_1          = 0x0064,//000000H-000FFFH
	P25Q06L_BLOCK0_4k_2          = 0x0044,//00F000H-00FFFFH
	P25Q06L_BLOCK0_8k_1 		 = 0x0068,//000000H-001FFFH
	P25Q06L_BLOCK0_8k_2			 = 0x0048,//00E000H-00FFFFH
	P25Q06L_BLOCK0_16k_1         = 0x006c,//000000H-003FFFH
	P25Q06L_BLOCK0_16k_2         = 0x004c,//00C000H-00FFFFH
	P25Q06L_BLOCK0_32k_1         = 0x0078,//000000H-007FFFH(0x0078/0x4058)
	P25Q06L_BLOCK0_32k_2         = 0x0058,//008000H-00FFFFH(0x0058/0x4078)
	P25Q06L_BLOCK0_48k_1         = 0x404C,//000000H-00BFFFH
	P25Q06L_BLOCK0_48k_2         = 0x406C,//004000H-00FFFFH
	P25Q06L_BLOCK0_56k_1         = 0x4048,//000000H-00DFFFH
	P25Q06L_BLOCK0_56k_2         = 0x4068,//002000H-00FFFFH
	P25Q06L_BLOCK0_60k_1         = 0x4044,//000000H-00EFFFH
	P25Q06L_BLOCK0_60k_2         = 0x4064,//001000H-00FFFFH
	P25Q06L_BLOCK0_64k           = 0x007c,//001000H-00FFFFH
} Flash_ProtectedAreaDef;
*/

/**
 * @brief This function serves to erase a sector.
 * @param[in]   addr the start address of the sector needs to erase.
 * @return none
 */
_attribute_ram_code_ void flash_erase_sector(unsigned long addr);

/**
 * @brief This function writes the buffer's content to a page.
 * @param[in]   addr the start address of the page
 * @param[in]   len the length(in byte) of content needs to write into the page
 * @param[in]   buf the start address of the content needs to write into
 * @return none
 */
_attribute_ram_code_ void flash_write_page(unsigned long addr, unsigned long len, unsigned char *buf);

/**
 * @brief This function reads the content from a page to the buf.
 * @param[in]   addr the start address of the page
 * @param[in]   len the length(in byte) of content needs to read out from the page
 * @param[out]  buf the start address of the buffer
 * @return none
 */
_attribute_ram_code_ void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf);

/* according to your appliaction */
#if 1
/**
 * @brief     This function serves to erase a page(256 bytes).
 * @param[in] addr - the start address of the page needs to erase.
 * @return    none
 * @note      only 8359 support
 */
void flash_erase_page(unsigned int addr);

/**
 * @brief This function serves to erase a block(32k).
 * @param[in]   addr the start address of the block needs to erase.
 * @return none
 */
void flash_erase_32kblock(unsigned int addr);

/**
 * @brief This function serves to erase a block(64k).
 * @param[in]   addr the start address of the block needs to erase.
 * @return none
 */
void flash_erase_64kblock(unsigned int addr);

/**
 * @brief     This function serves to erase a page(256 bytes).
 * @param[in] addr - the start address of the page needs to erase.
 * @return    none
 */
void flash_erase_chip(void);


/**
 * @brief This function write the status of flash.
 * @param[in]  the value of status
 * @return status
 */
unsigned char flash_write_status(unsigned char data);

/**
 * @brief This function reads the status of flash.
 * @param[in]  none
 * @return none
 */
unsigned char flash_read_status(void);

/**
 * @brief  	Deep Power Down mode to put the device in the lowest consumption mode
 * 			it can be used as an extra software protection mechanism,while the device
 * 			is not in active use,since in the mode,  all write,Program and Erase commands
 * 			are ignored,except the Release from Deep Power-Down and Read Device ID(RDI)
 * 			command.This release the device from this mode
 * @param[in] none
 * @return none.
 */
void flash_deep_powerdown(void);

/**
 * @brief		The Release from Power-Down or High Performance Mode/Device ID command is a
 * 				Multi-purpose command.it can be used to release the device from the power-Down
 * 				State or High Performance Mode or obtain the devices electronic identification
 * 				(ID)number.Release from Power-Down will take the time duration of tRES1 before
 * 				the device will resume normal operation and other command are accepted.The CS#
 * 				pin must remain high during the tRES1(8us) time duration.
 * @param[in]   none
 * @return      none.
 */
void flash_release_deep_powerdown(void);

/***********************************
 * @brief	  This function serves to read MID of flash
 * @param[in] buf - store MID of flash
 * @return    none.
 */
void flash_read_mid(unsigned char *buf);

/**
 * @brief	  This function serves to read UID of flash
 * @param[in] idcmd - different flash vendor have different read-uid command
 *                    GD/PUYA:0x4B; XTX: 0x5A
 * @param[in] buf   - store UID of flash
 * @return    none.
 */
void flash_read_uid(unsigned char idcmd,unsigned char *buf);
/**
 * @brief 		 This function serves to read flash mid and uid,and check the correctness of mid and uid.
 * @param[out]   flash_mid - Flash Manufacturer ID
 * @param[out]   flash_uid - Flash Unique ID
 * @return       0:error 1:ok

 */
int flash_read_mid_uid_with_check( unsigned int *flash_mid ,unsigned char *flash_uid);
/**
 * @brief This function serves to protect data for flash.
 * @param[in]   type - flash type include GD,Puya and XTX
 * @param[in]   data - refer to Driver API Doc or Flash_ProtectedAreaDef in "flash.h".
 * @return none
 */
void flash_lock(Flash_TypeDef type , unsigned short data);

/**
 * @brief This function serves to protect data for flash.
 * @param[in]   type - flash type include GD,Puya and XTX
 * @return none
 */
void flash_unlock(Flash_TypeDef type);
/**
 * @brief This function serves to read data from the Security Registers of the flash.
 *        You can read a byte one time.
 *              the address of the  Security Registers #1 0x001000-0x0011ff
 *              the address of the  Security Registers #2 0x002000-0x0021ff
 *              the address of the  Security Registers #3 0x003000-0x0031ff
 *
 * @param[in]   the address of the data
 * @return      the data
 */

unsigned char flash_read_otp_byte(unsigned long addr);

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
void flash_write_otp(unsigned long addr, unsigned long len, unsigned char *buf);

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
void flash_erase_otp_512byte(unsigned long addr);

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

void flash_lock_otp(Flash_OtpDef lock_type);

#endif
