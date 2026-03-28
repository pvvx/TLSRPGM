
#pragma once

enum{
	FLASH_WRITE_CMD				=	0x02,
	FLASH_READ_CMD				=	0x03,
	FLASH_WRITE_ENABLE_CMD 		= 	0x06,
	FLASH_WRITE_DISABLE_CMD 	= 	0x04,
	FLASH_READ_STATUS_CMD		=	0x05,
	FLASH_SECT_ERASE_CMD		=	0x20,
	FLASH_GD_PUYA_READ_UID_CMD	=	0x4B,	//Flash Type = GD/PUYA
	FLASH_32KBLK_ERASE_CMD		=	0x52,
	FLASH_XTX_READ_UID_CMD		=	0x5A,	//Flash Type = XTX
	FLASH_CHIP_ERASE_CMD		=	0x60,   //or 0xc7
	FLASH_GET_JEDEC_ID			=	0x9F,
	FLASH_PAGE_ERASE_CMD		=	0x81,   //caution: only P25Q40L support this function
	FLASH_POWER_DOWN			=	0xB9,
	FLASH_BLK_ERASE_CMD			=	0xD8,
};

_attribute_ram_code_ void flash_erase_sector(u32 addr);
_attribute_ram_code_ void flash_write_page(u32 addr, u32 len, u8 *buf);
_attribute_ram_code_ void flash_read_page(u32 addr, u32 len, u8 *buf);
_attribute_ram_code_ void flash_get_jedec_id(u8 * p); // out 3 bytes: manufacturer, mem_type, cap_id
_attribute_ram_code_ void flash_erase_all(void);
_attribute_ram_code_ void flash_read_uid(unsigned char *buf); // read 16 bytes



