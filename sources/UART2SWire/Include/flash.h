
#ifndef _flash_h
#define _flash_h

// #pragma once

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
	FLASH_RELEASE_POWER_DOWN	=   0xAB,
	FLASH_UNIQUE_ID			=	0x4B,

	//Manufacturer/Device ID = 0x90
	//Read Security Register = 0x48
	//Read SFDP Register = 0x5A
	//Read Block Lock = 0x3D
	//Read Unique ID = 0x4B
};

#endif // _flash_h
