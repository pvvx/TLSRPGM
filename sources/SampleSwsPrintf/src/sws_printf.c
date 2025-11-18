/**************************************************
 * @file     sws_printf.c
 *
 * @brief    This is the source file for TLSR8258
 *
 * @author	 pvvx
 *
 *************************************************/
#include "app_config.h"
#include "sws_printf.h"

/* SWS send buffer */
_sws_buffer_in_retention_ram_
sws_buffer_t sws_buffer;

/* Checking and waiting waiting for the transfer to end (buffer to be ready to fill) */
int sws_ready(void) {
	unsigned int tt;
	if (sws_buffer.len == SWS_BUF_CLOSED)
		// Wait for the next opening
		return 0;
	tt = clock_time();
	while (sws_buffer.len != 0) {
		// The transfer is not finished
		if (clock_time() - tt > SWS_WAITING_TIMEOUT) {
			// SDI Timeout, Closed due to timeout
			sws_buffer.len = SWS_BUF_CLOSED;
			return 0;
		}
	}
	// The buffer has been transmitted or is ready to be filled.
	return 1;
}

/* Write a character to the send buffer */
void sws_putchar(char c) {

	if (sws_ready()) { // The buffer is waiting for new data?
		// Put in buffer
		sws_buffer.data[sws_buffer.cur_len] = c;
		sws_buffer.cur_len++;
		// End of line or maximum transmit buffer size reached?
		if (c == '\n' || sws_buffer.cur_len >= SWS_BUF_MAX_LEN) {
			// Set the length of the new data transfer
			sws_buffer.len = sws_buffer.cur_len;
			sws_buffer.cur_len = 0;
		}
	}
}

/* Waiting for the transfer to end. (Use before sleep) */
void sws_buffer_flush(void) {

	// Checking and waiting waiting for the transfer to end (buffer to be ready to fill)
	if (sws_ready()) {
		// Buffer to be ready to fill.
		if(sws_buffer.cur_len) { // All data transferred?
			// Set the length of the new data transfer
			sws_buffer.len = sws_buffer.cur_len;
			sws_buffer.cur_len = 0;
			// Waiting for buffer to be transmitted to SWS
			sws_ready();
		}
	}
}

/* Write a string to the send buffer */
void sws_puts(char *s)
{
    while ((*s != '\0')) {
        sws_putchar(*s++);
    }
}

//------ printf ----------------------------------------------------

typedef char *va_list;

#define _INTSIZEOF(n)       ((sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, v)     (ap = (va_list)&v + _INTSIZEOF(v))
#define va_arg(ap, t)       (*(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))
#define va_end(ap)          (ap = (va_list)0)

#define DECIMAL_OUTPUT      10
#define OCTAL_OUTPUT        8
#define HEX_OUTPUT          16

static void puti(unsigned int num, int base, int w)
{
    char re[] = "0123456789ABCDEF";
    char buf[50];
    int cnt = 0;

    char *addr = &buf[49];
    *addr = '\0';

    do {
        *--addr = re[num % base];
        num /= base;
        cnt++;
    } while (num != 0 && cnt < 49);

    for ( ; w > cnt; --w) {
        *--addr = '0';
    }

    sws_puts(addr);
}

/* printf */
int sws_printf(const char *format, ...)
{
    char span;
    unsigned long j;
    char *s;
    long m;
    int w;

    va_list arg_ptr;

    if (sws_buffer.len == SWS_BUF_CLOSED)
    	// Wait for the next opening
		return 0;

    va_start(arg_ptr, format);

    while ((span = *(format++))) {
        if (span != '%') {
            sws_putchar(span);
        } else {
            span = *(format++);

            w = 0;
            for ( ; span >= '0' && span <= '9'; span = *(format++)) {
                w *= 10;
                w += span - '0';
            }

            if (span == 'c') {
                j = va_arg(arg_ptr, int);//get value of char
                sws_putchar(j);
            } else if (span == 'd') {
                m = va_arg(arg_ptr, int);//get value of char
                if (m < 0) {
                    sws_putchar('-');
                    m = -m;
                }
                puti(m, DECIMAL_OUTPUT, w);
            } else if (span == 's') {
                s = va_arg(arg_ptr, char *);//get string value
                sws_puts(s);
            } else if (span == 'o') {
                j = va_arg(arg_ptr, unsigned int);//get octal value
                puti(j, OCTAL_OUTPUT, w);
            } else if (span == 'x') {
                j = va_arg(arg_ptr, unsigned int);//get hex value
                puti(j, HEX_OUTPUT, w);
            } else if (span == 0) {
                break;
            } else {
                sws_putchar(span);
            }
        }
    }

    va_end(arg_ptr);

    return 0;
}
