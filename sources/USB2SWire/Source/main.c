/*
 * Poject TlsrTool FLOADER
 * pvvx 09/2019
 *
 * Poject TlsrComProg
 * pvvx 02/2020
 */
#include "common.h"
#include "spi_i.h"
#include "flash.h"
#include "crc.h" // Calculator CRC-16/MODBUS: https://crccalc.com/
#include "usb.h"
#include "swire.h"
//===============================
#if CHIP_TYPE != MCU_CORE_8269
#error "CHIP_TYPE = MCU_CORE_8269!"
#endif
//------------------------------- Init UART ---
typedef struct _uart_tab_baud_t {
	u8 bwpc;
	u8 cdiv;
} uart_tab_baud_t;

unsigned char new_uart_baud = 1; // 230400
unsigned char cur_uart_baud;

uart_tab_baud_t uart_tab_baud[] = {
	{8,30}, //0 115200 // 32000000/(8+1)/(30+1)=114695.340502
	{9,13},	//1 230400 // 32000000/(9+1)/(13+1)=228571.428571
	{9,6},	//2 460800 // 32000000/(9+1)/(6+1)=457142.857  32000000/(8+1)/(7+1)=444444.444
	{7,7},	//3 500000 // 32000000/(7+1)/(7+1)=500000
	{6,4},	//4 921600 // 32000000/(10+1)/(2+1)=969696.96 32000000/(6+1)/(4+1)=914285.7
	{3,7},	//5 1000000 // 32000000/(3+1)/(7+1)=1000000
	{3,3},	//6 1600000 // 32000000/(4+1)/(3+1)=1600000
	{3,3},	//7 2000000 // 32000000/(3+1)/(3+1)=2000000
	{3,1}	//8 3200000 // 32000000/(4+1)/(1+1)=3200000
};
//-------------------------------
dma_uart_buf_t urxb = {.len = 0};
dma_uart_buf_t utxb = {.len = 0};
//-------------------------------
_attribute_ram_code_  void * memcpy (void * to, const void * from, size_t size) {
	u8 * pto = (u8 *)to;
	u8 * pfrom = (u8 *)from;
	while(size) {
		size--;
		*pto++ = *pfrom++;
	}
	return to;
}
/* flash chip select low */
_attribute_ram_code_ void _swire_set_fcs_low(void) {
	unsigned char tmp = 0; //[0x0d] set csn low
	swire_write_bytes(0x000d, &tmp, 1);
}
/* flash chip select hi */
_attribute_ram_code_ void _swire_set_fcs_hi(void) {
	unsigned char tmp = FLD_MASTER_SPI_CS; // [0x0d] set csn high
	swire_write_bytes(0x000d, &tmp, 1);
}
/* flash send fcmd + faddr */
_attribute_ram_code_ void _swire_fcmd_faddr(unsigned char cmd, unsigned int addr) {
	union {
		unsigned int  ud;
		unsigned char uc[4];
	}tmp;
	_swire_set_fcs_low();
	tmp.ud = addr;
	tmp.uc[3] = cmd;
	swire_write_bytes(0x000c, &tmp.uc[3], 1); // send flash cmd
	swire_write_bytes(0x000c, &tmp.uc[2], 1); // send 16..23 bits addr
	swire_write_bytes(0x000c, &tmp.uc[1], 1); // send 8..15 bits addr
	swire_write_bytes(0x000c, &tmp.uc[0], 1); // send 0..7 bits addr
}
/* flash chip select low + send write byte command */
_attribute_ram_code_ void swire_fcmd(unsigned char fcmd) {
	unsigned char tmp = fcmd;
	_swire_set_fcs_low();
	swire_write_bytes(0x000c, &tmp, 1); //[0x0c] send fwrite enable command
	_swire_set_fcs_hi();
}
//-------------------------------


#if (USE_USB_CDC)

volatile char usb_flg = 0; // Flg bit0: DTR - Open/Close USB-COM port
/*
_attribute_ram_code_
void irq_handler(void){
	USB_IrqHandle();
}
*/
/* rxFunc rx callback function
 * Called from Irq (!) */
_attribute_ram_code_
void USBCDC_RxCb(unsigned char *data, unsigned int len){
	if (len) { // есть данные?
		if(urxb.len == 0
			&& data
			&& len >= sizeof(blk_head_t)
#if USE_IO_CRC
			+ 2 // + sizeof(CRC)
#endif
			&& len <= USB_CDC_MAX_RX_BLK_SIZE) {
			urxb.len = len;
		}
		USBCDC_RxBufSet((unsigned char *)&urxb.uc); // назначить новый буфер (в данном приложении единственный)
	}
}
#ifndef USB_TX_CALLBACK
_attribute_ram_code_
void USBCDC_TxCb(void) {
	utxb.len = 0;
}
#endif
#else
_attribute_ram_code_
void irq_handler(void){
}
#endif // USE_USB_CDC


_attribute_ram_code_
void flash_write_sector(u32 addr, u32 len, u8 *buf) {
	u32 sz = 256;
	while(len) {
		if (len < sz) sz = len;
		flash_write_page(addr, sz, buf);
		addr += sz;
		buf += sz;
		len -= sz;
	}
}
#if USE_INT_UART
_attribute_ram_code_
inline void uart_init(void) {
	u32 uartCLKdiv = uart_tab_baud[new_uart_baud].cdiv;
	u32 bwpc = uart_tab_baud[new_uart_baud].bwpc;
	cur_uart_baud = new_uart_baud;
	// reg_uart_clk_div/reg_uart_ctrl0
	REG_ADDR32(0x094) = MASK_VAL(FLD_UART_CLK_DIV, uartCLKdiv, FLD_UART_CLK_DIV_EN, 1)
		|	((MASK_VAL( FLD_UART_BWPC, bwpc) // set bit width
			| MASK_VAL(FLD_UART_STOP_BIT, 1) // 00: 1 bit, 01: 1.5bit 1x: 2bits;
			| FLD_UART_RX_DMA_EN | FLD_UART_TX_DMA_EN) // enable UART DMA mode
			<< 16);
	reg_uart_rx_timeout = MASK_VAL(FLD_UART_TIMEOUT_BW, (bwpc+1)*12) | FLD_UART_BW_MUL2;

	// reg_dma0_addr/reg_dma0_ctrl
	REG_ADDR32(0x500) = (unsigned short)((u32)(&urxb)) //set receive buffer address
		| 	((FLD_DMA_WR_MEM // set DMA0 mode to 0x01 for receive.write to memory
			| MASK_VAL(FLD_DMA_BUF_SIZE, UART_RX_BUFF_SIZE>>4))  //set rx buffer size
			<< 16);
	REG_ADDR32(0x504) = (unsigned short)((u32)(&utxb)) //set tx buffer address
		| 	(MASK_VAL(FLD_DMA_BUF_SIZE, UART_TX_BUFF_SIZE>>4) //set tx buffer size
			<< 16);
}
#endif

_attribute_ram_code_
int main (void) {
	reg_irq_en = 0;
	// Open clk for MCU running
	REG_ADDR8(0x60) = 0x00;
	REG_ADDR8(0x61) = 0x00;
	REG_ADDR8(0x62) = 0x00;
	REG_ADDR8(0x63) = 0xff;
	REG_ADDR8(0x64) = 0xff;
	REG_ADDR8(0x65) = 0xff;
	REG_ADDR32(0x60) = 0 // reg_rst_clk0
//	| FLD_CLK_SPI_EN
//	| FLD_CLK_I2C_EN
#if (USE_USB_CDC)
	| FLD_CLK_USB_EN
	| FLD_CLK_USB_PHY_EN
#endif
	| FLD_CLK_MCU_EN
	| FLD_CLK_MAC_EN
//	| FLD_CLK_ADC_EN	// ADC interface
	| FLD_CLK_ZB_EN
		;
	analog_write(0x88, 0x0f);
	// rega_pwdn_setting1:
	analog_write(0x05, 0x62);

	REG_ADDR32(0x70) = 0 // = 0x04000400
	/* reg_fhs_sel [0x70], After reset = 0x00 */
		| ((CLOCK_FHS_TYPE == FHS_SEL_PAD)?1:0)
	/* reg_dcdc_clk [0x71], After reset [0x71] = 0x04 */
		| ((1<<2)<<8)
	/* reg_?? [0x72], After reset [0x72] = 0x00 */
		| (0<<16) // watchdog reset status bit 0x72[0] = 1, manually clear - write '1'
	/* reg_clk_mux_cel [0x73], After reset  = 0x14
	* [0] clk32k select; 0: sel 32k osc 1: 32k pad
	* [1] dmic clock select, 1: select 32k (refer to bit[0] to decide which 32k ; 0: dmic clk div
	* [2] usb phy clock select, 1 : 192M divider 0: 48M pll
	* [7:4] r_lpr_div, decide system clock speed in low power mode	 */
		| ((1<<2)<<24);

	REG_ADDR32(0x64) = 0 // 0x0804108F // 0x00241097
    // reg_clk_en:
	| FLD_CLK_GPIO_EN
	| FLD_CLK_ALGM_EN
	| FLD_CLK_DMA_EN
	| FLD_CLK_UART_EN
//	| FLD_CLK_PWM_EN
//	| FLD_CLK_AES_EN
	| FLD_CLK_PLL_EN
//	| FLD_CLK_32K_TIMER_EN
#if (!SWIRE_OFF)
	| FLD_CLK_SWIRE_EN
#endif
	// reg_clk_en2 (0x65)
//	| FLD_CLK_32K_QDEC_EN
//	| FLD_CLK_AUD_EN
	| FLD_CLK_DIFIO_EN
//	| FLD_CLK_KEYSCAN_EN
	| FLD_CLK_MCIC_EN
//	| FLD_CLK_QDEC_EN
	// reg_clk_sel (0x66):
	| ((((CLOCK_FHS_TYPE == FHS_SEL_RC)? 1:0) << 7) << 16)
    | (MASK_VAL(FLD_CLK_SEL_DIV, CLOCK_PLL_CLOCK/CLOCK_SYS_CLOCK_HZ, FLD_CLK_SEL_SRC, CLOCK_SYS_TYPE) << 16)
    // reg_i2s_step (0x67):
    | (MASK_VAL(FLD_I2S_STEP, 8, FLD_I2S_CLK_EN, 0) << 24)  // i2s clk disable
	;
	// enable system tick ( clock_time() )
	reg_system_tick_ctrl = FLD_SYSTEM_TICK_START; //	REG_ADDR8(0x74f) = 0x01;

#if (USE_USB_CDC)
#if SET_PLL == QUARTZ_12MHZ
	reg_pll_ctrl_a = FLD_PLL_A_CAL_DONE_EN | 0x80;
	analog_write(0x099, 0xb1);
	analog_write(0x082, 0x20);
	analog_write(0x09e, 0xad);
#else // SET_PLL == CLK_QUARTZ
	reg_pll_ctrl_a = FLD_PLL_A_CAL_DONE_EN;
	analog_write(0x099, 0x31);
	analog_write(0x082, 0x34);
	analog_write(0x09e, 0x82);
#endif // SET_PLL
#endif // USE_USB_CDC

	crcInit();
#if	(USE_INT_UART)
	uart_init();
#endif
#if (USE_USB_CDC)
		//-------------------------- USB pins 8269
#if USE_USB
#define PE2_FUNC	AS_USB
#define PE3_FUNC	AS_USB
#define PE2_INPUT_ENABLE	1
#define PE3_INPUT_ENABLE	1
#define PULL_WAKEUP_SRC_PE2           	PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M  // USB DM
#define PULL_WAKEUP_SRC_PE3           	PM_PIN_PULLDOWN_100K // PM_PIN_PULLUP_1M  // USB DP
#endif
#endif // USE_USB_CDC
		// Init GPIO_RESET
		SET_GPIO_POWER();
		BM_SET(reg_gpio_gpio_func(GPIO_RESET), GPIO_RESET & 0xff);
		SET_GPIO_RESET(); // Pin RST = "Z"
		// Init GPIO_POWER
		SET_GPIO_POWER(); // Pin POWER = "1"
		BM_SET(reg_gpio_gpio_func(GPIO_POWER), GPIO_POWER & 0xff);
		BM_CLR(reg_gpio_oen(GPIO_POWER), GPIO_POWER & 0xff);
		// Init GPIO_SWM
		// reg_swire_clk_div = CLOCK_SYS_CLOCK_HZ/5/960000;
		BM_CLR(reg_gpio_gpio_func(GPIO_SWM), GPIO_SWM & 0xff); // Pin SWM disable as gpio
		// usb_dp_pullup_disable();
		// USB-DM: PULL_WAKEUP_SRC_PE2 PM_PIN_PULLDOWN_100K, PULL_WAKEUP_SRC_PE3 PM_PIN_UP_DOWN_FLOAT
		analog_write(0x08, (analog_read (0x08) & 0x0f) | PM_PIN_PULLDOWN_100K<<4);
		// PE2_FUNC	AS_USB, PE3_FUNC	AS_USB
		BM_CLR(reg_gpio_gpio_func(GPIO_DM), (GPIO_DM | GPIO_DP) & 0xff); // disable PE2/PE3 as gpio
		// PE2_INPUT_ENABLE	1, PE3_INPUT_ENABLE	1
		BM_SET(reg_gpio_ie(GPIO_DM), (GPIO_DM | GPIO_DP) & 0xFF);  // enable input
#if	(USE_INT_UART)
		//-------------------------- UART pins 8269
		// UART_GPIO_CFG_PC2_PC3:  CPGIO PC2/PC3 enable uart function and enable input
		analog_write(0x0f, /* (analog_read(0x0f) & 0xf0) | */ PM_PIN_UP_DOWN_FLOAT | (PM_PIN_PULLUP_1M<<2));
		BM_CLR(reg_gpio_gpio_func(GPIO_TX), (GPIO_TX | GPIO_RX) & 0xFF);
		BM_SET(reg_gpio_config_func(GPIO_TX), (GPIO_TX | GPIO_RX) & 0xFF);
		//BM_SET(reg_gpio_ie(GPIO_TX), (GPIO_TX | GPIO_RX) & 0xFF);  //enable input
#endif // USE_INT_UART
#if (SWIRE_OFF)
		reg_gpio_gpio_func(GPIO_SWS) |= GPIO_SWS & 0xff; // GPIO_PB0/SWS set gpio
#endif

#if (USE_USB_CDC)
	/* Initialize usb cdc */
	USB_Init();
	USBCDC_RxBufSet((unsigned char *)&urxb.uc);
#if !(defined(USB_TX_CALLBACK) || defined(USB_RX_CALLBACK))
	USBCDC_CBSet(USBCDC_RxCb, USBCDC_TxCb);
#endif
	usb_dp_pullup_enable();
	reg_irq_en = 1; // irq_enable();
#endif // USE_USB_CDC
	/////////////////////////// app floader /////////////////////////////
#if USE_IO_CRC
	u16 crc16;
#endif
	while(1) {
#if (USE_USB_CDC)
//		if(usb_flg & 1) { // USB-COM Open: DTR = 1
		{
			if(utxb.len == 0 && urxb.len) { // new command?
#endif // USE_USB_CDC
#if	(USE_INT_UART)
		if((reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX) == 0) {
			if(cur_uart_baud != new_uart_baud) {
				cur_uart_baud = new_uart_baud;
				sleep_us(40000000u/115200u + 4); // 10000000/115200+4 = 90 us
				uart_init();
			}
			if(reg_dma_irq_src & FLD_DMA_IRQ_UART_RX) { // new command?
#endif // USE_INT_UART
				union {
					unsigned int  ud;
					unsigned char uw[2];
					unsigned char uc[4];
				}tmp;
				unsigned int rxlen = urxb.len;
				utxb.len = sizeof(utxb.pkt.head);
				utxb.pkt.head.ud = urxb.pkt.head.cmd;	// err = 0, len = 0
				if(rxlen < sizeof(urxb.pkt.head)
#if USE_IO_CRC
					+ 2
#endif // USE_IO_CRC
					) {
					utxb.pkt.head.err = ERR_LEN; // BAD LEN
				} else {
#if USE_IO_CRC
					crc16 = crcFast(urxb.uc, rxlen);
					if(crc16 != 0) {
						utxb.pkt.head.cmd |= ERR_CRC; // BAD CRC
					} else {
#else // USE_IO_CRC
					{
#endif // USE_IO_CRC
						urxb.uc[rxlen] = 0;
						u32 argv = urxb.pkt.head.b.argv;
#if USE_IO_CRC
						rxlen -= sizeof(urxb.pkt.head) + 2;
#else // USE_IO_CRC
						rxlen -= sizeof(urxb.pkt.head);
#endif // USE_IO_CRC

						switch(urxb.pkt.head.cmd) {
							case CMD_SWIRE_READ: // regs read (1024 bytes - 11.4 ms)
								if(rxlen == 0
									|| urxb.pkt.rd.len > sizeof(utxb.pkt.data))
									urxb.pkt.rd.len = sizeof(utxb.pkt.data);
								utxb.pkt.head.count = swire_read_bytes(argv, utxb.pkt.data, urxb.pkt.rd.len);
								if(utxb.pkt.head.count != urxb.pkt.rd.len)
									utxb.pkt.head.err = ERR_READ;
								utxb.len += utxb.pkt.head.count;
								break;
							case CMD_SWIRE_WRITE:
								if(rxlen == 0
									|| rxlen > sizeof(urxb.pkt.data))
									utxb.pkt.head.err = ERR_LEN;
								else
									utxb.pkt.head.count = swire_write_bytes(argv, urxb.pkt.data, rxlen);
								break;
							case CMD_SWIRE_AREAD: // analog register read (3.5 ms fifo read 256 bytes, 434 us - one byte no fifo)
								tmp.uc[0] = (u8)argv;	//[0xb8] =  analog register address
								if(rxlen == 0
									|| urxb.pkt.rd.len > 255)
									urxb.pkt.rd.len = 256;
								if (urxb.pkt.rd.len > 1) { // fifo read?
									// analog fifo read
									tmp.uc[2] = 0x50; //[0xba]=0x50 multiple read
									swire_write_bytes(0x00b8, tmp.uc, 3); //[0xb8..0xba] analog register address, data, ctrl
									// swire fifo read
									utxb.pkt.head.count = swire_fifo_read(0x00b9, utxb.pkt.data, urxb.pkt.rd.len);
									if(utxb.pkt.head.count != urxb.pkt.rd.len)
										utxb.pkt.head.err = ERR_READ;
									utxb.len += utxb.pkt.head.count;
								} else {
									tmp.uc[2] = FLD_ANA_START;	//[0xba] analog register ctrl
									swire_write_bytes(0x00b8, tmp.uc, 3); //[0xb8..0xba] analog register address, data, ctrl
									tmp.ud = 7; // busy count
									while(1) {
										if(swire_read_bytes(0x00b9, utxb.pkt.data, 2) != 2){
											utxb.pkt.head.err = ERR_READ;
											break;
										}
										if((utxb.pkt.data[1] & (FLD_ANA_BUSY<<8)) == 0) {
											utxb.len++;
											utxb.pkt.head.count = 1;
											break;
										}
										if(tmp.ud--) {
											utxb.pkt.head.err = ERR_BUSY;
											break;
										}
									};
								}
								tmp.ud = 0;				//[0xb9] end of access add by ZQ
								swire_write_bytes(0x00ba, tmp.uc, 1); // end of access
								break;
							case CMD_SWIRE_AWRITE: // analog register write
								if(rxlen == 0
									|| rxlen > 256)
									utxb.pkt.head.err = ERR_LEN;
								else {
									unsigned char * p = utxb.pkt.data;
									tmp.uc[0] = (u8)argv;	//[0xb8] analog register address
									tmp.uc[1] = *p++;			//[0xb9] analog register value
									tmp.uc[3] = 0;				//[0xba] end of access add by ZQ
									if (rxlen > 1) { // fifo write?
										swire_write_bytes(0x00b8, tmp.uc, 1); //[0xb8] analog register address
										tmp.uc[2] = 0x70;	//[0xba] set multiple write mode
										while(rxlen--) {
											swire_write_bytes(0x00b9, &tmp.uc[1], 2); //[0xb9, 0xba] analog register value, ctrl
											tmp.uc[1] = *p++;	//[0xb9] analog register value
											utxb.pkt.head.count++;
										}
									} else {
										tmp.uc[2] = FLD_ANA_START;			//[0xba] analog read trigger
										swire_write_bytes(0x00b8, tmp.uc, 3); //[0xb8..0xba] analog register address, data, ctrl
										utxb.pkt.head.count = 1;
									}
									swire_write_bytes(0x00ba, &tmp.uc[3], 1); // end of access
								}
								break;
							case CMD_FLASH_READ: // Flash read (1024 bytes - 267.134 ms)
								if(rxlen == 0
									|| urxb.pkt.rd.len > sizeof(utxb.pkt.data))
									urxb.pkt.rd.len = sizeof(utxb.pkt.data);
								//tmp.ud = 0x05; 			// reset mcu
								//swire_write_bytes(0x0602, tmp.uc, 1); // [0x602]=0x05
								_swire_fcmd_faddr(FLASH_READ_CMD, argv); 		// send flash read command + faddr
								// spi read
								tmp.ud = (FLD_MASTER_SPI_RD | FLD_MASTER_SPI_SDO) << 8;	// [0x0d]=0x0a set auto read mode
								swire_write_bytes(0x000c, tmp.uc, 2); 	//[0x0c] = 0 launch first read, [0x0d]=x set auto read mode

#if 1
								rxlen = urxb.pkt.rd.len;
								if(rxlen == 1)
									utxb.pkt.head.count = swire_read_bytes(0x000c, utxb.pkt.data, rxlen);
								else // swire fifo read
									utxb.pkt.head.count = swire_fifo_read(0x000c, utxb.pkt.data, rxlen);
								_swire_set_fcs_hi(); // flash chip select hi
								if(utxb.pkt.head.count != rxlen)
									utxb.pkt.head.err = ERR_READ;
								utxb.len += utxb.pkt.head.count;
#else
								if(urxb.pkt.rd.len == 1)
									utxb.pkt.head.count = swire_read_bytes(0x000c, utxb.pkt.data, urxb.pkt.rd.len);
								else // swire fifo read
									utxb.pkt.head.count = swire_fifo_read(0x000c, utxb.pkt.data, urxb.pkt.rd.len);
								_swire_set_fcs_hi(); // flash chip select hi
								if(utxb.pkt.head.count != urxb.pkt.rd.len)
									utxb.pkt.head.err = ERR_READ;
								utxb.len += utxb.pkt.head.count;
#endif
								break;
							case CMD_FLASH_WRITE: // Flash write (max 256 bytes)
								if(rxlen == 0
									|| rxlen > 256)
									utxb.pkt.head.err = ERR_LEN;
								else {
									swire_fcmd(FLASH_WRITE_ENABLE_CMD); // chip select low + send write enable command
									_swire_fcmd_faddr(FLASH_WRITE_CMD, argv); // send flash write command + faddr
									if(rxlen == 1)
										utxb.pkt.head.count = swire_write_bytes(0x000c, urxb.pkt.data, rxlen);
									else // fifo write
										utxb.pkt.head.count = swire_fifo_write(0x000c, urxb.pkt.data, rxlen);
									_swire_set_fcs_hi(); // flash chip select hi
									// @TODO Flash flags ready?
									break;
								}
								break;
							case CMD_FLASH_SECT_ERASE: // Flash sector erase
								swire_fcmd(FLASH_WRITE_ENABLE_CMD); // chip select low + send write enable command
								_swire_fcmd_faddr(FLASH_SECT_ERASE_CMD, argv); // send sector erase command + faddr
								// @TODO Flash flags ready?
								_swire_set_fcs_hi(); // flash chip select hi
								break;
							case CMD_FLASH_ALL_ERASE: // Flash sector erase
								swire_fcmd(FLASH_WRITE_ENABLE_CMD); // chip select low + send write enable command
								swire_fcmd(FLASH_CHIP_ERASE_CMD); // send chip erase command
								// @TODO Flash flags ready?
								_swire_set_fcs_hi(); // flash chip select hi
								break;
							case CMD_SWIRE_FIFO_READ: // Swire fifo read (1024 bytes - 11.7 ms)
								if(rxlen == 0
									|| urxb.pkt.rd.len > sizeof(utxb.pkt.data))
									urxb.pkt.rd.len = sizeof(utxb.pkt.data);
								utxb.pkt.head.count = swire_fifo_read(argv, utxb.pkt.data, urxb.pkt.rd.len);
								if(utxb.pkt.head.count != urxb.pkt.rd.len)
									utxb.pkt.head.err = ERR_READ;
								utxb.len += utxb.pkt.head.count;
								break;
							case CMD_SWIRE_FIFO_FWRITE: // Swire fifo write (1024 bytes - 19.4 ms)
								if(rxlen == 0
									|| rxlen > sizeof(urxb.pkt.data))
									utxb.pkt.head.err = ERR_LEN;
								else
									utxb.pkt.head.count = swire_fifo_write(argv, urxb.pkt.data, rxlen);
								break;
							case CMD_FLASH_GET_JEDEC_ID:
								tmp.ud = FLASH_GET_JEDEC_ID;
								_swire_set_fcs_low();	// flash chip select low
								swire_write_bytes(0x000c, tmp.uc, 1); // [0x0c] send command
								tmp.ud = (FLD_MASTER_SPI_RD | FLD_MASTER_SPI_SDO) << 8;	// [0x0d]=x set auto read mode
								swire_write_bytes(0x000c, tmp.uc, 2); // [0x0c] = 0 launch first read, [0x0d]=x set auto read mode
								rxlen = 3;
								utxb.pkt.head.count = swire_fifo_read(0x000c, utxb.pkt.data, rxlen);
								_swire_set_fcs_hi(); // flash chip select hi
								if(utxb.pkt.head.count != rxlen)
									utxb.pkt.head.err = ERR_READ;
								utxb.len+=utxb.pkt.head.count;
								break;
							case CMD_FLASH_GET_STATUS:
								tmp.ud = FLASH_READ_STATUS_CMD;
								_swire_set_fcs_low();	// flash chip select low
								swire_write_bytes(0x000c, tmp.uc, 1); //[0x0c] send command
								// spi read
								tmp.ud = (FLD_MASTER_SPI_RD | FLD_MASTER_SPI_SDO) << 8;	// [0x0d]=x set auto read mode
								swire_write_bytes(0x000c, tmp.uc, 2); 	//[0x0c] = 0 launch first read, [0x0d]=x set auto read mode
								rxlen = 1;
								utxb.pkt.head.count = swire_read_bytes(0x000c, utxb.pkt.data, rxlen);
								_swire_set_fcs_hi(); // flash chip select hi
								if(utxb.pkt.head.count != rxlen)
									utxb.pkt.head.err = ERR_READ;
								utxb.len += utxb.pkt.head.count;
								break;
							case CMD_FLASH_WRRD:
								if(rxlen <= sizeof(urxb.pkt.f.rdcount)
										|| rxlen > 256 + 4 + sizeof(urxb.pkt.f.rdcount))
									utxb.pkt.head.err = ERR_LEN;
								rxlen -= sizeof(urxb.pkt.f.rdcount);
								if(urxb.pkt.f.rdcount > sizeof(utxb.pkt.data))
									urxb.pkt.f.rdcount = sizeof(utxb.pkt.data);
								_swire_set_fcs_low();	// flash chip select low
								if(rxlen == 1)
									utxb.pkt.head.count = swire_write_bytes(0x000c, urxb.pkt.f.wrdata, rxlen);
								else // fifo write
									utxb.pkt.head.count = swire_fifo_write(0x000c, urxb.pkt.f.wrdata, rxlen);
								if(urxb.pkt.f.rdcount) {
									// spi read
									tmp.ud = (FLD_MASTER_SPI_RD | FLD_MASTER_SPI_SDO) << 8;	// [0x0d]=x set auto read mode
									swire_write_bytes(0x000c, tmp.uc, 2); 	//[0x0c] = 0 launch first read, [0x0d]=x set auto read mode
									if(urxb.pkt.f.rdcount == 1)
										tmp.ud = swire_read_bytes(0x000c, utxb.pkt.data, urxb.pkt.f.rdcount);
									else // fifo read
										tmp.ud = swire_fifo_read(0x000c, utxb.pkt.data, urxb.pkt.f.rdcount);
									utxb.pkt.head.count += tmp.ud;
									if(tmp.ud != urxb.pkt.f.rdcount)
										utxb.pkt.head.err = ERR_READ;
									utxb.len += tmp.ud;
								}
								_swire_set_fcs_hi(); // flash chip select hi
								break;
							case CMD_FUNCS: // ext. functions
								argv >>= 8;
								switch(urxb.pkt.head.func) {
									case CMDF_GET_VERSION:
										utxb.pkt.ver.bcd_num = VERSION_BCD;
										utxb.pkt.ver.chip_id = reg_prod_id; //chip id
										utxb.pkt.ver.pwr = (reg_gpio_in(GPIO_RESET) & GPIO_RESET) != 0;
										utxb.pkt.ver.clk_div = reg_swire_clk_div;
										utxb.pkt.ver.addr_len = swire_addr_len;
										memcpy(utxb.pkt.ver.cmd_cpu_stop, swire_cmd_cpu_stop, sizeof(swire_cmd_cpu_stop));
										utxb.pkt.head.count = sizeof(utxb.pkt.ver);
										utxb.len += sizeof(utxb.pkt.ver);
										break;
									case CMDF_MCU_REBOOT:
										REG_ADDR8(0x6f) = 0x20;   // mcu reboot
										break;
									case CMDF_SWIRE_CFG: // Get/Set: reg_swire_clk_div,  swire_addr_len, swire_cmd_cpu_stop
										if(rxlen--) {
											reg_swire_clk_div = urxb.pkt.swcfg.clk_div;
											if(rxlen--) {
												if(urxb.pkt.swcfg.addr_len == 2)
													swire_addr_len = 2;
												else
													swire_addr_len = 3;
												if(rxlen) {
													if(rxlen > sizeof(swire_cmd_cpu_stop))
														rxlen = sizeof(swire_cmd_cpu_stop);
													memcpy(swire_cmd_cpu_stop, urxb.pkt.swcfg.cmd_cpu_stop, rxlen);
												}
											}
										}
										utxb.pkt.swcfg.clk_div = reg_swire_clk_div;
										utxb.pkt.swcfg.addr_len = swire_addr_len;
										memcpy(utxb.pkt.swcfg.cmd_cpu_stop, swire_cmd_cpu_stop, sizeof(swire_cmd_cpu_stop));
										utxb.pkt.head.count = sizeof(utxb.pkt.swcfg);
										utxb.len += sizeof(utxb.pkt.swcfg);
										break;
									case CMDF_EXT_POWER: // External Power On/Off or RST = "1"/"0"
										if(argv) {
											BM_CLR(reg_gpio_gpio_func(GPIO_SWM), GPIO_SWM & 0xff); // Pin SWM disable as gpio
											SET_GPIO_RESET(); // Pin RST = "Z"
											SET_GPIO_POWER(); // Pin POWER = "1"
										}
										else {
											BM_SET(reg_gpio_gpio_func(GPIO_SWM), GPIO_SWM & 0xff); // Pin SWM = "0" (enable as gpio)
											CLR_GPIO_RESET(); // Pin RST = "0"
											CLR_GPIO_POWER(); // Pin POWER = "0"
										}
										utxb.pkt.data[0] = (reg_gpio_in(GPIO_RESET) & GPIO_RESET) != 0;
										utxb.pkt.head.count = 1;
										utxb.len++;
										break;
									case CMDF_SWIRE_ACTIVATE: // Activate
										BM_CLR(reg_gpio_gpio_func(GPIO_SWM), GPIO_SWM & 0xff); // Pin SWM disable as gpio
										SET_GPIO_RESET(); // Pin RST = "Z"
										SET_GPIO_POWER(); // Pin POWER = "1"
										swire_write(0xff, FLD_SWIRE_WR | FLD_SWIRE_CMD); // cmd stop
										swire_write(0xff, FLD_SWIRE_WR | FLD_SWIRE_CMD); // cmd stop
										while(argv--)
											swire_write_bytes((swire_cmd_cpu_stop[1] << 16) | (swire_cmd_cpu_stop[2] << 8) | swire_cmd_cpu_stop[3], &swire_cmd_cpu_stop[5], 1);
										utxb.pkt.head.count = swire_read_bytes(0x06bc, utxb.pkt.data, 4);
										if(utxb.pkt.head.count != 4)
											utxb.pkt.head.err = ERR_READ;
										utxb.len += utxb.pkt.head.count;
										break;
									case CMDF_UART_BAUD:
										if(argv > 8)
											utxb.pkt.head.err = ERR_BAUD;
										else
											new_uart_baud = argv;
										break;
									default: // error subfunction number
										utxb.pkt.head.err = ERR_FUNC;
										break;
								}
								break;
							default:	// error function number
								utxb.pkt.head.err = ERR_FUNC;
								utxb.len = 1;
								break;
						}
					}
				}
#if (USE_USB_CDC)
				urxb.len = 0;
				if(utxb.len) {
#if USE_IO_CRC
					crc16 = crcFast(utxb.uc, utxb.len);
					utxb.uc[utxb.len++] = crc16;
					utxb.uc[utxb.len++] = crc16 >> 8;
#endif // USE_IO_CRC
					USBCDC_DataSend((unsigned char *)&utxb.uc, utxb.len);
				}
#endif // USE_USB_CDC
#if USE_INT_UART
				reg_dma_irq_src = FLD_DMA_IRQ_UART_RX | FLD_DMA_IRQ_UART_TX;
				if(utxb.len) {
#if USE_IO_CRC
					crc16 = crcFast(utxb.uc, utxb.len);
					utxb.uc[utxb.len++] = crc16;
					utxb.uc[utxb.len++] = crc16 >> 8;
#endif // USE_IO_CRC
					reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX; // start tx
				}
#endif	// USE_INT_UART
			}
		}
#if MODULE_WATCHDOG_ENABLE
		WATCHDOG_CLEAR;  //in case of watchdog timeout
#endif
	}
//	REG_ADDR8(0x6f) = 0x20;   // mcu reboot
//	while (1);
}
