/*
 * swire.c
 *
 *  Created on: 21.11.2020
 *      Author: pvvx
 */
#include "common.h"
#include "swire.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */
#define REG_SWIRE_CTRL_ADD 0 // FLD_SWIRE_USB_EN if USB enable or 0
/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * GLOBAL VARIABLES
 */

/* Length Address in bytes: TLSR826x = 2, TLSR825x = 3 */
unsigned char swire_addr_len = 3;
/* Swire config (byte0 - start, byte4 - id), Activate */
unsigned char swire_cmd_cpu_stop[6]   = { 0x5a, 0x00, 0x06, 0x02, 0x00, 0x05 };

/* swire write */
_attribute_ram_code_ void swire_write(unsigned char data, unsigned char ctrl) {
	unsigned int t = reg_swire_clk_div << 4;
	reg_swire_data = data;
	reg_swire_ctrl1 = ctrl | REG_SWIRE_CTRL_ADD;
	while(reg_swire_ctrl1 & FLD_SWIRE_WR) {
		if(!t--) {
			reg_swire_ctrl1 = REG_SWIRE_CTRL_ADD;
			break;
		}
	}
}

/* swire read */
_attribute_ram_code_ unsigned int swire_read(unsigned char *data, unsigned int len) {
	unsigned int i;
	unsigned int t;
	for (i = 0; i < len; i++) {
		t = reg_swire_clk_div << 4; // 500 = ~ 190 us
		reg_swire_ctrl1 = FLD_SWIRE_RD | REG_SWIRE_CTRL_ADD;
		while(reg_swire_ctrl1 & FLD_SWIRE_RD) {
			if(!t--) {
				reg_swire_ctrl1 = REG_SWIRE_CTRL_ADD;
				return i;
			}
		}
		data[i] = reg_swire_data; //*data++ = reg_swire_data;
	}
	return i;
}

/* swire write bytes */
_attribute_ram_code_ unsigned int swire_write_bytes(unsigned int addr, unsigned char * pdata, unsigned int len) {
	unsigned int cnt = len;
	unsigned char bid = (swire_cmd_cpu_stop[swire_addr_len+1] & 0x7f);
	swire_write(swire_cmd_cpu_stop[0], FLD_SWIRE_WR | FLD_SWIRE_CMD); // 0x5a
	if (swire_addr_len == 3) {
		swire_write(addr>>16, FLD_SWIRE_WR);	// addrh
	}
	swire_write(addr>>8, FLD_SWIRE_WR);	// addrm
	swire_write(addr, FLD_SWIRE_WR); // addrl
	swire_write(bid, FLD_SWIRE_WR); // id
	while(cnt--)
		swire_write(*pdata++, FLD_SWIRE_WR); // data
	swire_write(0xff, FLD_SWIRE_WR | FLD_SWIRE_CMD); // stop
	return len;
}

/* swire read bytes */
_attribute_ram_code_ unsigned int swire_read_bytes(unsigned int addr, unsigned char *pdata, unsigned int len) {
	unsigned int cnt;
	unsigned char bid = (swire_cmd_cpu_stop[swire_addr_len+1] & 0x7f) | 0x80;
	swire_write(swire_cmd_cpu_stop[0], FLD_SWIRE_WR | FLD_SWIRE_CMD); // 0x5a
	if (swire_addr_len == 3) {
		swire_write(addr>>16, FLD_SWIRE_WR);	// addrh
	}
	swire_write(addr>>8, FLD_SWIRE_WR);	// addrm
	swire_write(addr, FLD_SWIRE_WR); // addrl
	swire_write(bid, FLD_SWIRE_WR); // id
	cnt = swire_read(pdata, len); // data
	swire_write(0xff, FLD_SWIRE_WR | FLD_SWIRE_CMD); // stop
	return cnt;
}

/* swire fifo write */
_attribute_ram_code_ unsigned int swire_fifo_write(unsigned int addr, unsigned char *pdata, unsigned int len) {
	unsigned int cnt;
	unsigned char tmp = 0x80;		    //[0xb3]=0x80 fifo mode
	swire_write_bytes(0x00b3, &tmp, 1); // fifo mode
	cnt = swire_write_bytes(addr, pdata, len);
	tmp = 0x00;
	swire_write_bytes(0x00b3, &tmp, 1); //[0xb3]=0x00 ram mode
	return cnt;
}

/* swire fifo read */
_attribute_ram_code_ unsigned int swire_fifo_read(unsigned int addr, unsigned char *pdata, unsigned int len) {
	unsigned int cnt;
	unsigned char tmp = 0x80;		    //[0xb3]=0x80 fifo mode
	swire_write_bytes(0x00b3, &tmp, 1); // fifo mode
	cnt = swire_read_bytes(addr, pdata, len);
	tmp = 0x00;
	swire_write_bytes(0x00b3, &tmp, 1); //[0xb3]=0x00 ram mode
	return cnt;
}

