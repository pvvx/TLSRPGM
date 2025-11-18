/*
 * Poject TlsrTool FLOADER
 * pvvx 09/2019
 *
 * Poject TlsrComProg
 * pvvx 02/2020 * 10/2020 TLSR825x
 */
#include "common.h"
#include "analog.h"
#include "flash.h"
#include "clock.h"
#include "crc.h"
#include "main.h"
#include "swire.h"

#define  SDI_DOUBLE_BUF 1

volatile unsigned char flg_sdi_print; // flag sdi_print enable
/*
 * Calculator CRC-16/MODBUS: https://crccalc.com/
 */
//-------------------------------
typedef struct _uart_tab_baud_t {
	u8 bwpc;
	u8 cdiv;
} uart_tab_baud_t;

unsigned char new_uart_baud = 1; // 230400
unsigned char cur_uart_baud;

uart_tab_baud_t uart_tab_baud[] = {
	{12,15},//0 115200
	{12,7},	//1 230400
	{12,2},	//2 460800
	{7,5},	//3 500000
	{12,1},	//4 921600
	{5,3},	//5 1000000
	{3,3},	//6 1500000
	{3,2},	//7 2000000
	{3,1}	//8 3000000
};
dma_uart_buf_t urxb;
dma_uart_buf_t utxb;

#if SDI_DOUBLE_BUF
dma_uart_sdi_buf_t sdi_txb[2];
#endif

_attribute_ram_code_
//inline
void uart_init(void) {
#if CHIP_TYPE == MCU_CORE_825x
	u32 uartCLKdiv = uart_tab_baud[new_uart_baud].cdiv;
	u32 bwpc = uart_tab_baud[new_uart_baud].bwpc;
	cur_uart_baud = new_uart_baud;
	// reg_uart_clk_div/reg_uart_ctrl0
	REG_ADDR32(0x094) = MASK_VAL(FLD_UART_CLK_DIV, uartCLKdiv, FLD_UART_CLK_DIV_EN, 1)
		|	((MASK_VAL(FLD_UART_BWPC, bwpc) // set bit width
			| MASK_VAL(FLD_UART_STOP_BIT, 1) // 00: 1 bit, 01: 1.5bit 1x: 2bits;
			| FLD_UART_RX_DMA_EN | FLD_UART_TX_DMA_EN) // enable UART DMA mode
		<< 16);
	reg_uart_rx_timeout = MASK_VAL(FLD_UART_TIMEOUT_BW, (bwpc+1)*12) | FLD_UART_BW_MUL2;
	// reg_dma0_addr/reg_dma0_ctrl
	REG_ADDR32(0xC00) = (unsigned short)((u32)(&urxb)) //set receive buffer address
		| (MASK_VAL(FLD_DMA_BUF_SIZE, UART_RX_BUFF_SIZE>>4) << 16)  //set rx buffer size
		| (FLD_DMA_WR_MEM << 24);  // set DMA0 mode to 0x01 for receive.write to memory
	REG_ADDR32(0xC04) = (unsigned short)((u32)(&utxb)) //set tx buffer address
		| 	(MASK_VAL(FLD_DMA_BUF_SIZE, UART_TX_BUFF_SIZE>>4)<< 16); //set tx buffer size
	// reg_dma0_addrHi, reg_dma1_addrHi, reg_dma2_addrHi, reg_dma3_addrHi = 0x04
	REG_ADDR32(0xc40) = 0x04040404;
	//	reg_dma_chn_en = (FLD_DMA_CHN0 | FLD_DMA_CHN1); reg_dma_chn_irq_msk = (FLD_DMA_CHN0 | FLD_DMA_CHN1);
	REG_ADDR16(0xc20) = 0x0303;
#else // CHIP_TYPE
#error "Only TLSR825x!"
#endif
}

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

_attribute_ram_code_ unsigned int read_flash(unsigned int faddr, unsigned char *pbuf, unsigned int cnt) {

	unsigned int rxlen;

	_swire_fcmd_faddr(FLASH_READ_CMD, faddr); 		// send flash read command + faddr
	// spi read
	rxlen = (FLD_MASTER_SPI_RD | FLD_MASTER_SPI_SDO) << 8;	// [0x0d]=0x0a set auto read mode
	swire_write_bytes(0x000c, (unsigned char *)&rxlen, 2); 	//[0x0c] = 0 launch first read, [0x0d]=x set auto read mode

	if(cnt == 1)
		rxlen = swire_read_bytes(0x000c, pbuf, cnt);
	else // swire fifo read
		rxlen = swire_fifo_read(0x000c, pbuf, cnt);

	_swire_set_fcs_hi(); // flash chip select hi
	return rxlen;
}


_attribute_ram_code_ unsigned int task_sdi_print(unsigned int argv, unsigned char *pbuf) {
	unsigned int rxlen = 0;
	unsigned char out = 0;
	if(swire_read_bytes(argv, &out, 1)) {
		if(out != 0 && out != 0xff){
			rxlen = swire_read_bytes(argv+1, pbuf, out);
			if(rxlen == out) {
				out = 0;
				swire_write_bytes(argv, &out, 1);
			} else
				rxlen = 0;
		}
	}
	return rxlen;
}

_attribute_ram_code_ int main (void) {
	union {
		unsigned int  ud;
		unsigned char uw[2];
		unsigned char uc[4];
	} tmp = {
		.ud = 0
	};
	unsigned int rxlen = 0;
	unsigned int argv = 0;
#if SDI_DOUBLE_BUF
	dma_uart_sdi_buf_t *psdi_txb = &sdi_txb[0];
	dma_uart_sdi_buf_t *psdi_rxb = &sdi_txb[0];
	dma_uart_sdi_buf_t *psdi_tmp;;
#endif
	reg_irq_en = 0;
	// Open clk for MCU running
	REG_ADDR32(0x60) = 0xff000000;
	REG_ADDR16(0x64) = 0xffff;
#if CHIP_TYPE == MCU_CORE_825x
		analog_write(0x82,0x64);	//areg_clk_setting
		analog_write(0x52,0x80);
		analog_write(0x0b,0x38);
		analog_write(0x8c,0x02);
		analog_write(0x02,0xa2);	//rega_vol_ldo_ctrl

#if 0 		// GPIO wake up disable
		analog_write(0x27,0x00);	//PA wake up disable, rega_wakeup_en_val0
		analog_write(0x28,0x00);	//PB wake up disable, rega_wakeup_en_val1
		analog_write(0x29,0x00);	//PC wake up disable, rega_wakeup_en_val2
		analog_write(0x2a,0x00);	//PD wake up disable, rega_wakeup_en_val3
			// DMA channels disable
		REG_ADDR8(0xc20) = 0;	// reg_dma_chn_en = 0
		REG_ADDR8(0xc21) = 1;	// reg_dma_chn_irq_msk = FLD_DMA_CHN_UART_RX
			// Set DMA buffers hi address
		REG_ADDR32(0xc40) = 0x04040404; // reg_dma0_addrHi, reg_dma1_addrHi, reg_dma2_addrHi, reg_dma3_addrHi = 0x04
		REG_ADDR32(0xc44) = 0x04040404; // reg_dma4_addrHi, reg_dma5_addrHi, reg_dma_ta_addrHi, reg_dma_a3_addrHi = 0x04
		REG_ADDR8(0xc48) = 0x04;	// reg_dma7_addrHi = 0x04

		REG_ADDR16(0x750) = 0x1F40;
#else
		REG_ADDR8(0xc20) = 0;	// reg_dma_chn_en = 0
#endif
		//analog_write(0x01,((REG_ADDR8(0x7D) == 1)? 0x3c : 0x4c)); // rega_xtal_ctrl

		//reg_gpio_wakeup_irq |= FLD_GPIO_WAKEUP_EN | FLD_GPIO_INTERRUPT_EN; // [0x5b5]|=0x0c

		// select 24M as the system clock source.
		// clock.c : rc_24m_cal
		analog_write(0xc8, 0x80);
		analog_write(0x30, analog_read(0x30) | BIT(7));
		analog_write(0xc7, 0x0e);
		analog_write(0xc7, 0x0f);
		while((analog_read(0xcf) & 0x80) == 0);
		analog_write(0x33, analog_read(0xcb));		//write 24m cap into manual register
		analog_write(0x30, analog_read(0x30) & (~BIT(7)));
		analog_write(0xc7, 0x0e);
//	}
	reg_rst_clk = 0
	| FLD_CLK_SPI_EN
//	| FLD_CLK_I2C_EN
	| FLD_CLK_UART_EN
//	| FLD_CLK_USB_EN
//	| FLD_CLK_PWM_EN
//	| FLD_CLK_QDEC_EN
	| FLD_CLK_SWIRE_EN
	;
    reg_clk_en = 0
	| FLD_CLK_ZB_EN
	| FLD_CLK_SYS_TIMER_EN
	| FLD_CLK_DMA_EN
	| FLD_CLK_ALGM_EN
//	| FLD_CLK_AES_EN
//	| BIT(5)
//	| BIT(6)
//	| BIT(7)
//	| FLD_CLK_AIF_EN
//	| FLD_CLK_AUD_EN
//	| FLD_CLK_DFIFO_EN
	| FLD_CLK_MC_EN
	| FLD_CLK_MCIC_EN
//	| BIT(13)
//	| BIT(14)
//	| BIT(15)
	;
    reg_clk_sel = 0
    | MASK_VAL(FLD_SCLK_DIV, 6)
	// [6:5] select system clock source:
	//0: RC_24M from RC oscillator
	//1: FHS
	//2: HS divider (see 0x66[4:0])
	//3: 32M clock (48M * 2 /3 divider)
    | MASK_VAL(FLD_SCLK_SEL, 0)
    //if reg_fhs_sel = 0:
	//0: 48M clock doubled from 24M crystal, 1: RC_24M from RC oscillator,
	///if reg_fhs_sel = 1: Pad_24M from crystal oscillator
    | FLD_SCLK_HS_SEL
    ;
	reg_fhs_sel = 0
//	| FLD_FHS_SELECT	//0: 48M clock doubled from 24M crystal or RC_24M from RC oscillator, 1: Pad_24M from crystal oscillator
	;
	// Pin SWM
	reg_mux_func_d2 &= 0xfc; // Pad Function Mux = SWM
	BM_SET(reg_gpio_pd_setting1, GPIO_PD4 & 0xff);
	BM_CLR(reg_gpio_func(GPIO_PD4), GPIO_PD4 & 0xff); // GPIO_D4/SWM disable as gpio (0x586[7]=0, 0x586[0]=0)
	BM_CLR(reg_gpio_out(GPIO_PD4), GPIO_PD4 & 0xff); // pin SWM output lo level
	BM_CLR(reg_gpio_oen(GPIO_PD4), GPIO_PD4 & 0xFF); // pin SWM enable output

	// Pin Power Output = "1"
	BM_SET(reg_gpio_out(GPIO_POWER), GPIO_POWER & 0xff); // Pin POW = "1"
	//BM_SET(reg_gpio_func(GPIO_POWER), GPIO_POWER & 0xFF); // enable as gpio (default = enable)
	BM_CLR(reg_gpio_oen(GPIO_POWER), GPIO_POWER & 0xFF); // enable output
	BM_SET(reg_gpio_ds(GPIO_POWER), GPIO_POWER & 0xFF); // hi Drive strength

	// Pin RST Output = "1"
	// BM_SET(reg_gpio_func(GPIO_RESET), GPIO_RESET & 0xFF); // enable as gpio (default = enable)
	BM_SET(reg_gpio_out(GPIO_RESET), GPIO_RESET & 0xff); // output hi level
	BM_CLR(reg_gpio_oen(GPIO_RESET), GPIO_RESET & 0xFF); // enable output
	BM_SET(reg_gpio_ds(GPIO_RESET), GPIO_RESET & 0xFF); // hi Drive strength

#else // CHIP_TYPE
#error "Only TLSR825x!"
#endif // CHIP_TYPE
	// enable system tick ( clock_time() )
	reg_system_tick_ctrl = FLD_SYSTEM_TICK_START; //	REG_ADDR8(0x74f) = 0x01;
#if USE_IO_CRC
	crcInit();
#endif // USE_IO_CRC
	uart_init();
	// sws off and enable uart function and enable input
//	if(reg_mcu_tid == MCU_PROD_TID__825x) {
		// CPGIO PA0/PB1 enable uart function and enable input
		// PA0=RX
		analog_write(0x0e, (analog_read(0x0e) & 0xfc) | PM_PIN_PULLUP_1M);
#if SWIRE_OFF
		reg_gpio_func(GPIO_PA0) = ((~(GPIO_PA0)) & 0xff) | GPIO_PA7; // GPIO_PA7/SWS set gpio (0x586[7]=0)
		//BM_SET(reg_gpio_gpio_func(GPIO_PA7), GPIO_PA7 & 0xFF); // set PA7 as gpio (0x586[7]=0)
#else // SWIRE_OFF
		reg_gpio_func(GPIO_PA0) = (~(GPIO_PA7 | GPIO_PA0)) & 0xff; // GPIO_PA7/SWS & GPIO_PA0/RX disable as gpio (0x586[7]=0, 0x586[0]=0)
#endif // SWIRE_OFF
		BM_SET(reg_gpio_ie(GPIO_PA0), GPIO_PA0 & 0xFF);  // PA0 enable input (0x581[0]=1)
		reg_gpio_config_func(GPIO_PA0) = 2; // PA0 mux uart rx function (0x5a8[1:0] = 2)
		//  PB1=TX
		BM_CLR(reg_gpio_func(GPIO_PB1), GPIO_PB1 & 0xFF); // disable PB1 as gpio (0x58e[1] = 0)
		analog_write(areg_gpio_pb_ie, analog_read(areg_gpio_pb_ie) | (GPIO_PB1 & 0xFF)); // PB1 enable input afe_0xbd[1]=1
		reg_gpio_config_func(GPIO_PB1) = 0x04; // (0x5aa[3:2] = 1)
//	}
	/////////////////////////// app floader /////////////////////////////
#if USE_IO_CRC
	u16 crc16;
#endif // USE_IO_CRC
	while(1) {
#if SDI_DOUBLE_BUF
		if(flg_sdi_print) {
			if((reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX) == 0) {
				reg_dma_irq_src |= FLD_DMA_IRQ_UART_TX;
				while((reg_uart_status1 & FLD_UART_TX_DONE)==0);
				while(flg_sdi_print) {
					if((reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX) == 0) {
						if(psdi_txb->busy) {
							psdi_txb->busy = 0;
							if(psdi_txb == &sdi_txb[0])
								psdi_txb = &sdi_txb[1];
							else
								psdi_txb = &sdi_txb[0];
							reg_dma_irq_src = FLD_DMA_IRQ_UART_TX;
							if(psdi_txb->busy) {
								while((reg_uart_status1 & FLD_UART_TX_DONE)==0);
								reg_dma1_addr = (unsigned short)((u32)(psdi_txb)); //set tx buffer address
								reg_dma_tx_rdy0 = FLD_DMA_CHN_UART_TX; // start tx
							}
						}
					}
					if(psdi_rxb->busy == 0) {
						psdi_rxb->len = task_sdi_print(argv, psdi_rxb->data);
						if(psdi_rxb->len) {
							psdi_rxb->busy = 1;
							psdi_tmp = psdi_rxb;
							if(psdi_rxb == &sdi_txb[0])
								psdi_rxb = &sdi_txb[1];
							else
								psdi_rxb = &sdi_txb[0];
							if(psdi_rxb->busy == 0) {
								reg_dma_irq_src = FLD_DMA_IRQ_UART_TX;
								while((reg_uart_status1 & FLD_UART_TX_DONE)==0);
								reg_dma1_addr = (unsigned short)((u32)(psdi_tmp)); //set tx buffer address
								reg_dma_tx_rdy0 = FLD_DMA_CHN_UART_TX; // start tx
							}
						}
					}
					if(reg_dma_irq_src & FLD_DMA_IRQ_UART_RX) {
						if(urxb.len) { // new command?
							flg_sdi_print = 0; // clear flag sdi_print
							break;
						} else {
							reg_dma_irq_src = FLD_DMA_IRQ_UART_RX;
						}
					}
				}
			}
		}
#endif
		if((reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX) == 0) {
			if(cur_uart_baud != new_uart_baud) {
				cur_uart_baud = new_uart_baud;
				sleep_us(40000000u/115200u + 4); // 10000000/115200+4 = 90 us
				uart_init();
			}
			if(reg_dma_irq_src & FLD_DMA_IRQ_UART_RX) { // new command?
				rxlen = urxb.len;
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
						argv = urxb.pkt.head.b.argv;
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
#if 0
								_swire_fcmd_faddr(FLASH_READ_CMD, argv); 		// send flash read command + faddr
								// spi read
								tmp.ud = (FLD_MASTER_SPI_RD | FLD_MASTER_SPI_SDO) << 8;	// [0x0d]=0x0a set auto read mode
								swire_write_bytes(0x000c, tmp.uc, 2); 	//[0x0c] = 0 launch first read, [0x0d]=x set auto read mode

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
								rxlen = urxb.pkt.rd.len;
								utxb.pkt.head.count = read_flash(argv, utxb.pkt.data, rxlen);
								if(utxb.pkt.head.count != rxlen)
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
							case CMD_FLASH_RDCRC: // Flash read CRC (max 1024 bytes)
								if(rxlen == 0
									|| urxb.pkt.rd.len > sizeof(utxb.pkt.data))
									urxb.pkt.rd.len = sizeof(utxb.pkt.data);
								rxlen = urxb.pkt.rd.len;
								if(read_flash(argv, utxb.pkt.data, rxlen) != rxlen)
									utxb.pkt.head.err = ERR_READ;
								else
									utxb.pkt.head.count = crcFast(utxb.pkt.data, rxlen);
								break;
							case CMD_SDI_PRINT:
								if(rxlen > sizeof(urxb.pkt.data))
									utxb.pkt.head.err = ERR_LEN;
								else {
									if(rxlen != 0)
										utxb.pkt.head.count = swire_write_bytes(argv, urxb.pkt.data, rxlen);
									flg_sdi_print = 1;
								}
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
											BM_CLR(reg_gpio_func(GPIO_PD4), GPIO_PD4 & 0xff); // Pin SWM disable as gpio
											BM_SET(reg_gpio_out(GPIO_RESET), GPIO_RESET & 0xff); // Pin RST = "1"
											BM_SET(reg_gpio_out(GPIO_POWER), GPIO_POWER & 0xff); // Pin POW = "1"
										}
										else {
											BM_SET(reg_gpio_func(GPIO_PD4), GPIO_PD4 & 0xff); // Pin SWM = "0" (enable as gpio)
											BM_CLR(reg_gpio_out(GPIO_RESET), GPIO_RESET & 0xff); // Pin RST = "0"
											BM_CLR(reg_gpio_out(GPIO_POWER), GPIO_POWER & 0xff); // Pin POW = "0"
										}
										utxb.pkt.data[0] = (reg_gpio_in(GPIO_RESET) & GPIO_RESET) != 0;
										utxb.pkt.head.count = 1;
										utxb.len++;
										break;
									case CMDF_SWIRE_ACTIVATE: // Activate
										BM_CLR(reg_gpio_func(GPIO_PD4), GPIO_PD4 & 0xff); // Pin SWM disable as gpio
										BM_SET(reg_gpio_out(GPIO_RESET), GPIO_RESET & 0xff); // Pin RST = "1"
										BM_SET(reg_gpio_out(GPIO_POWER), GPIO_POWER & 0xff); // Pin POW = "1"
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
				reg_dma_irq_src = FLD_DMA_IRQ_UART_RX;
				if(utxb.len) {
#if USE_IO_CRC
					crc16 = crcFast(utxb.uc, utxb.len);
					utxb.uc[utxb.len++] = crc16;
					utxb.uc[utxb.len++] = crc16 >> 8;
#endif // USE_IO_CRC
					while((reg_uart_status1 & FLD_UART_TX_DONE)==0);
					reg_dma1_addr = (unsigned short)((u32)(&utxb)); //set tx buffer address
					reg_dma_tx_rdy0 = FLD_DMA_CHN_UART_TX; // start tx
				}
#if !SDI_DOUBLE_BUF
			} else { // no new command
				if(flg_sdi_print) {
					utxb.len = task_sdi_print(argv, (unsigned char *)&utxb.pkt.head);
					if(utxb.len) {
						reg_dma_irq_src = FLD_DMA_IRQ_UART_TX;
						reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX; // start tx
					}
				}
#endif
			}
		}
	}
//	REG_ADDR8(0x6f) = 0x20;   //mcu reboot
	while (1);
}
