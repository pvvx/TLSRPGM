
#pragma once

enum{
	FLASH_WRITE_CMD			=	0x02,
	FLASH_READ_CMD			=	0x03,
	FLASH_WRITE_ENABLE_CMD 	= 	0x06,
	FLASH_WRITE_DISABLE_CMD = 	0x04,
	FLASH_READ_STATUS_CMD	=	0x05,
	FLASH_SECT_ERASE_CMD	=	0x20,
	FLASH_BLK_ERASE_CMD		=	0xD8,
	FLASH_CHIP_ERASE_CMD	=	0x60,
	FLASH_POWER_DOWN		=	0xB9,
	FLASH_GET_JEDEC_ID		=	0x9F,
};

_attribute_ram_code_ void flash_erase_sector(u32 addr);
_attribute_ram_code_ void flash_write_page(u32 addr, u32 len, u8 *buf);
_attribute_ram_code_ void flash_read_page(u32 addr, u32 len, u8 *buf);
_attribute_ram_code_ void flash_get_jedec_id(u8 * p); // out 3 bytes: manufacturer, mem_type, cap_id
_attribute_ram_code_ void flash_erase_all(void);



