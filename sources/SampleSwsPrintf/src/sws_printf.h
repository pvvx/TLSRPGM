/******************************************************
 * @file    sws_printf.h
 *
 * @brief   This is the header file for SWS printf
 *
 * @author	 pvvx
 *
 *****************************************************/
#ifndef _INC_SWS_PRINTF_H
#define _INC_SWS_PRINTF_H


/* The maximum transmit buffer size is 254 bytes.
   Size reduction is possible. */
#define	SWS_BUF_MAX_LEN	254

/* SDI Off or Closed due to timeout, Wait for the next opening */
#define	SWS_BUF_CLOSED	255

/* Waiting timeout - 22 ms (proportional to 230400 UART baud rate)
  It is advisable to set the COM connection speed
  in TlsrPgm.py to 500000 or 921600 Baud.*/
#define SWS_WAITING_TIMEOUT	(30 * CLOCK_16M_SYS_TIMER_CLK_1MS)

/* SWS send buffer */
typedef struct {
	/* if len = 255 -> SWS transmission is disabled,
	   Timeout in SWS transmission, Wait for the next opening
	   if len = 0 -> The buffer is ready to be filled
	   if len = 1..MAX_SWS_BUF_LEN ->
	   Number of characters for translation via SWS - data buffer busy */
	volatile unsigned char len;
	/* Character send buffer */
	unsigned char data[SWS_BUF_MAX_LEN];
	/* Number of characters in the buffer */
	unsigned char cur_len;
} sws_buffer_t;

/* SWS send buffer */
/* set a fixed address in memory:
  __attribute__((at(address))) or __attribute__((section("name")))
 Or find the address of "sws_buffer" in the *.lst file */
#define _sws_buffer_in_retention_ram_ _attribute_session_(".retention_data")
sws_buffer_t sws_buffer;

/* Write a string to the send buffer */
void sws_puts(char *s);
/* Write a character to the send buffer */
void sws_putchar(char c);
/* Checking and waiting waiting for the transfer to end (buffer to be ready to fill) */
int sws_ready(void);
/* Waiting for the transfer to end. (Use before sleep) */
void sws_buffer_flush(void);
/* printf */
int sws_printf(const char *format, ...);

#define printf  sws_printf

#endif //_INC_SWS_PRINTF_H
