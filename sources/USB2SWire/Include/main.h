/*
 * main.h
 *
 *  Created on: 22.11.2020
 *      Author: pvvx
 */

#ifndef MAIN_H_
#define MAIN_H_
//-------------------------------
#define VERSION_BCD 0x0003 // 0x1234 -> Ver 1.2.3.4
//-------------------------------
#define USE_IO_CRC			1
//-------------------------------
#define DATA_BUFF_SIZE 		1024
#define UART_DMA_BUFF_SIZE (DATA_BUFF_SIZE+16)
#define UART_RX_BUFF_SIZE	UART_DMA_BUFF_SIZE
#define UART_TX_BUFF_SIZE	UART_DMA_BUFF_SIZE
//-------------------------------


enum{
	CMD_FUNCS = 0,				//0
	CMD_FLASH_READ,				//1
	CMD_FLASH_WRITE,			//2
	CMD_FLASH_SECT_ERASE,		//3
	CMD_FLASH_ALL_ERASE,		//4
	CMD_FLASH_GET_JEDEC_ID,		//5
	CMD_FLASH_GET_STATUS,		//6
	CMD_SWIRE_READ,				//7
	CMD_SWIRE_WRITE,			//8
	CMD_SWIRE_AREAD,			//9
	CMD_SWIRE_AWRITE,			//a
	CMD_SWIRE_FIFO_READ,		//b
	CMD_SWIRE_FIFO_FWRITE,		//c
	CMD_FLASH_WRRD,				//d
	CMD_FLASH_RDCRC				//e Flash read CRC16 (blk max 1024 bytes)
};

enum{
	CMDF_GET_VERSION = 0,		//0
	CMDF_MCU_REBOOT,			//1
	CMDF_SWIRE_CFG,				//2
	CMDF_EXT_POWER,				//3
	CMDF_SWIRE_ACTIVATE,		//4
	CMDF_UART_BAUD				//5
};

enum{
	ERR_NONE = 0,	//0 Errors None
	ERR_FUNC,		//1 Function number error
	ERR_LEN,		//2 Data length error
	ERR_READ,		//3 Swire read timeout
	ERR_BUSY, 		//4 Timeout flag while reading analog register
	ERR_CRC,		//5 Error CRC
	ERR_BAUD		//6 Invalid baud rate number
};

typedef union __attribute__((packed)) _blk_head_t {
	struct __attribute__((packed)) {
		u8 cmd; // function number (input/output stream)
		union __attribute__((packed)) {
			struct __attribute__((packed)) {
				union __attribute__((packed)) {
					u8 func; // subfunction number (input stream CMD_FUNCS)
					u8 err; // error number (output stream)
				};
				u16 count; // number of bytes worked (output stream)
			};
			u8 argv[3]; // argument value (input stream)
		};
	};
	struct __attribute__((packed)) {
		u32 cmd		: 8; // function number (input/output stream)
		u32 argv	: 24; // argument value (input stream)
	}b;
	u32 ud;
}blk_head_t;

typedef struct __attribute__((packed)) _cmd_read_data_t {
	u16 len;
}cmd_read_data_t;

typedef struct __attribute__((packed)) _cmd_write_data_t {
	u8  data[DATA_BUFF_SIZE];
}cmd_write_data_t;

typedef struct __attribute__((packed)) _cmd_swire_cfg_t {
	u8  clk_div;
	u8	addr_len;
	u8  cmd_cpu_stop[6];
}cmd_swire_cfg_t;

typedef struct __attribute__((packed)) _cmd_flash_wrrd_t {
	u16  rdcount;
	u8	 wrdata[256];
}cmd_flash_wrrd_t;

typedef struct __attribute__((packed)) _cmd_version_t {
	u16 bcd_num;
	u16	chip_id;
	u8  pwr;
	u8  clk_div;
	u8	addr_len;
	u8  cmd_cpu_stop[6];
}cmd_version_t;

typedef struct __attribute__((packed)) _blk_rx_pkt_t{
	blk_head_t head;
	union __attribute__((packed)) {
		cmd_read_data_t		rd;
		cmd_write_data_t	wr;
		cmd_swire_cfg_t		swcfg;
		cmd_version_t		ver;
		cmd_flash_wrrd_t 	f;
		u8  uc[DATA_BUFF_SIZE];
		u16 uw[DATA_BUFF_SIZE>>1];
		u32 ud[DATA_BUFF_SIZE>>2];
		u8  data[DATA_BUFF_SIZE];
	};
} blk_rx_pkt_t;

typedef struct _dma_uart_buf_t {
	volatile u32 len;
	union __attribute__((packed)) {
		blk_rx_pkt_t pkt;
		u8 uc[UART_DMA_BUFF_SIZE];
		u16 uw[UART_DMA_BUFF_SIZE>>1];
		u32 ud[UART_DMA_BUFF_SIZE>>2];
	};
}dma_uart_buf_t;

extern dma_uart_buf_t urxb;
extern dma_uart_buf_t utxb;

#endif /* MAIN_H_ */
