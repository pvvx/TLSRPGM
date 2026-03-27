/*
 * ttool.c
 *
 *  Created on: 21.11.2020
 *      Author: pvvx
 */

#include "common.h"

#define GPIO_PIN_RESET GPIO_PD5

#define CLR_GPIO_PIN_RESET() BM_CLR(reg_gpio_out(GPIO_PIN_RESET), GPIO_PIN_RESET & 0xff)
#define SET_GPIO_PIN_RESET() BM_SET(reg_gpio_out(GPIO_PIN_RESET), GPIO_PIN_RESET & 0xff);

uint32_t swm_rxlen = 0;
uint32_t swm_txlen = 0;
uint32_t swm_txlen_tst = 0;
uint16_t cnt_activate = 0;

uint8_t sw_cmd_len = 3;
uint8_t sw_cpu_stop[7]   = { 0x5a, 0x06, 0x02, 0x00, 0x05, 0xff, 0xff };
uint8_t sw_reg_clkdiv[7] = { 0x5a, 0x00, 0xb2, 0x80, 0xff, 0xff, 0xff };


uint32_t swm_send_blk(uint8_t * tx_buf, uint8_t * rx_buf, uint32_t txlen) {
	uint32_t cnt = txlen;
	while(cnt) {
		*rx_buf = *tx_buf;
		reg_swire_data = *tx_buf;

		reg_swire_ctrl1
	}
}

uint32_t swbuf() {

}

uint32_t swm_cmd_parser(uint8_t * Buf, uint32_t *Len) {
  uint32_t reg;
//  HAL_UART_Transmit_DMA(&UartHandle, Buf, *Len);
  // принят блок Buf из USB
  swm_rxlen = (*Len) & 0x7F;
  if(swm_rxlen) {
      // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	  if(Buf[0] == 0x55 && swm_rxlen > 1) {
		  switch(Buf[1]) {
			case 0: // Pull RST pin to GND
				CLR_GPIO_PIN_RESET();
			    swm_rxlen = 2;
				break;
			case 1: // Release pin RST
				SET_GPIO_PIN_RESET();
			    swm_rxlen = 2;
				break;
			case 2: // Function 'Activate'
			    if (swm_rxlen == 4) {
					tst_cnt = (Buf[2]<<8) | Buf[3];
					// Release pin RST
					SET_GPIO_PIN_RESET();
					swm_rxlen = sw_cmd_len + 3;
					swm_txlen = swm_rxlen;
					swm_txlen_tst = swm_rxlen;
					//swm_send_cmd(swm_tx_buf, swm_rx_buf, swm_txlen);
					swm_rxlen = 0;
				}
				break;
			case 3: // Break 'Activate'
				cnt_activate = 0;
				swm_txlen_tst = 0;
				swm_txlen = 2;
				break;
/*
			case 4: // Get version && GPIO
				Buf[2] = 0x00; // version hi
				Buf[3] = 0x00; // version lo
				swm_rxlen = 10;
				break;
*/
			case 5: // Set Swire Speed
				if (swm_rxlen == 3) {
					Buf[2] &= 0x7f;
					reg_swire_clk_div = Buf[2];
					swm_rxlen = 3;
				} else swm_rxlen = 0;
				break;
			case 6: // Set sw_cpu_stop
				if (swm_rxlen == sizeof(sw_cpu_stop) + 2) {
					memcpy(sw_cpu_stop, &Buf[2], swm_rxlen);
				} else if(swm_rxlen == 2) {
					memcpy(&Buf[2], sw_cpu_stop, sizeof(sw_cpu_stop));
					swm_rxlen = sizeof(sw_cpu_stop) + 2;
				}
				break;
			case 7: // Set sw_reg_clkdiv
				if (swm_rxlen == sizeof(sw_reg_clkdiv) + 2) {
					memcpy(sw_reg_clkdiv, &Buf[2], swm_rxlen);
				} else if(swm_rxlen == 2) {
					memcpy(&Buf[2], sw_reg_clkdiv, sizeof(sw_reg_clkdiv));
					swm_rxlen = sizeof(sw_reg_clkdiv) + 2;
				}
				break;
			case 8:	// Set sw_cmd_len
				if (swm_rxlen == 3) {
					if(Buf[2] == 4)	sw_cmd_len = 4;
					else sw_cmd_len = 3;
					swm_rxlen = 3;
				} else if(swm_rxlen == 2) {
					Buf[2] = sw_cmd_len;
					swm_rxlen = 3;
				}
				break;
//			default:
//				break;
		  };
		  if(swm_rxlen) {
			//USBD_CDC_SetTxBuffer(&USBD_Device, Buf, swm_rxlen);
			//USBD_CDC_TransmitPacket(&USBD_Device);
			/* Initiate next USB packet transfer once UART completes transfer (transmitting data over Tx line) */
			//USBD_CDC_ReceivePacket(&USBD_Device);
			//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		  }
	  }
	  else {
		swm_txlen = 0;
		swm_send_blk(swm_tx_buf, Buf, swm_rxlen);
	  }
  }
  return (USBD_OK);
}
