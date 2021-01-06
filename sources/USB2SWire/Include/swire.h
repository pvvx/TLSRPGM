/*
 * swire.h
 *
 *  Created on: 22.11.2020
 *      Author: pvvx
 */

#ifndef SWIRE_H_
#define SWIRE_H_

unsigned int swire_read(unsigned char *data, unsigned int len);
void swire_write(unsigned char data, unsigned char ctrl);

unsigned int swire_read_bytes(unsigned int addr, unsigned char *pdata, unsigned int len);
unsigned int swire_write_bytes(unsigned int addr, unsigned char * pdata, unsigned int len);
unsigned int swire_fifo_read(unsigned int addr, unsigned char *pdata, unsigned int len);
unsigned int swire_fifo_write(unsigned int addr, unsigned char *pdata, unsigned int len);

/* Length Address in bytes: TLSR826x = 2, TLSR825x = 3 */
extern unsigned char swire_addr_len; // = 3;
/* Swire config (byte0 - start, byte4 - id), Activate */
extern unsigned char swire_cmd_cpu_stop[6]; //   = { 0x5a, 0x00, 0x06, 0x02, 0x00, 0x05 };

#endif /* SWIRE_H_ */
