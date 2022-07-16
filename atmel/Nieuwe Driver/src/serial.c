/*
 * serial.c
 *
 * Created: 4/24/2013 9:05:29 PM
 *  Author: bakker
 */ 

#define F_CPU 32000000UL

#include "serial.h"

#include <stdio.h>
#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


#define UART_230K4_BSEL_VALUE		245
#define UART_230K4_BSCALE_VALUE		0x0B
#define UART_230K4_CLK2X			0

#define UART_38K4_BSEL_VALUE		51
#define UART_38K4_BSCALE_VALUE		0

#define PD_UART_D1_RXD_CTRL			PIN6_bm
#define PD_UART_D1_TXD_CTRL			PIN7_bm
	
#define TXBUF_DEPTH_CTRL	250
#define RXBUF_DEPTH_CTRL	80


static int Ctrl_putchar(char c, FILE *stream);

FILE gCtrl_IO = FDEV_SETUP_STREAM(Ctrl_putchar, NULL, _FDEV_SETUP_WRITE);    
//usart E0 vervangen voor usart D1


static volatile uint8_t tx_ctrl_wridx, tx_ctrl_rdidx, tx_ctrl_buf[TXBUF_DEPTH_CTRL];
static volatile uint8_t rx_ctrl_wridx, rx_ctrl_rdidx, rx_ctrl_buf[RXBUF_DEPTH_CTRL];


void InitSerial(void)
{
	/* Shut down UARTs which might be left running by the bootloader */
	
	cli();
	
	USARTC0.CTRLB = 0;
 	USARTC1.CTRLB = 0;
 	USARTD0.CTRLB = 0;
 	USARTD1.CTRLB = 0;
 	USARTE0.CTRLB = 0;
 	USARTE1.CTRLB = 0;
 	USARTF0.CTRLB = 0;
	
	/* Pull-up on RX ports */
	PORTD.PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTD.PIN6CTRL = PORT_OPC_PULLUP_gc;
	
	/* Activate ports for RS485 (USARTE1) and serial comms (USARTE0), disable RS485 TX driver */
	PORTD.OUTSET = PD_UART_D1_TXD_CTRL; 
	PORTD.DIRSET = PD_UART_D1_TXD_CTRL;
	PORTD.DIRCLR = PD_UART_D1_RXD_CTRL; 

	PORTD.PIN0CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
	PORTD.PIN4CTRL = PORT_OPC_PULLDOWN_gc;
	
	USARTD1.BAUDCTRLA = (UART_230K4_BSEL_VALUE & USART_BSEL_gm);
	USARTD1.BAUDCTRLB = ((UART_230K4_BSCALE_VALUE << USART_BSCALE_gp) & USART_BSCALE_gm) | ((UART_230K4_BSEL_VALUE >> 8) & ~USART_BSCALE_gm);
	/*
	USARTF0.BAUDCTRLA = 123;
	USARTF0.BAUDCTRLB = 192;
	*/
 	USARTD1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
	USARTD1.CTRLA = USART_RXCINTLVL_MED_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_OFF_gc;
	
	PMIC.CTRL |= PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	
} /* InitSerial */


uint8_t CanRead_Ctrl(void) {
	uint8_t wridx = rx_ctrl_wridx, rdidx = rx_ctrl_rdidx;
	
	if(wridx >= rdidx)
		return wridx - rdidx;
	else
		return wridx - rdidx + RXBUF_DEPTH_CTRL;
	
} /* CanRead_Ctrl */


uint8_t ReadByte_Ctrl(void) {
	uint8_t res, curSlot, nextSlot;
	
	curSlot = rx_ctrl_rdidx;
	/* Busy-wait for a byte to be available. Should not be necessary if the caller calls CanRead_xxx() first */
	while(!CanRead_Ctrl()) ;
	
	res = rx_ctrl_buf[curSlot];

	nextSlot = curSlot + 1;
	if(nextSlot >= RXBUF_DEPTH_CTRL)
		nextSlot = 0;
	rx_ctrl_rdidx = nextSlot;
	
	return res;
} /* ReadByte_Ctrl */


uint8_t CanWrite_Ctrl(void) {
	uint8_t wridx1 = tx_ctrl_wridx + 1, rdidx = tx_ctrl_rdidx;
	
	if(wridx1 >= TXBUF_DEPTH_CTRL)
		wridx1 -= TXBUF_DEPTH_CTRL;
	if(rdidx >= wridx1)
		return rdidx - wridx1;
	else
		return rdidx - wridx1 + TXBUF_DEPTH_CTRL;
	
} /* CanWrite_Ctrl */


void WriteByte_Ctrl(uint8_t data) {
	uint8_t curSlot, nextSlot, savePMIC;
	
	/* Busy-wait for a byte to be available. Should not be necessary if the caller calls CanWrite_xxx() first */
	while(!CanWrite_Ctrl()) 
		USARTD1.CTRLA = USART_RXCINTLVL_MED_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_LO_gc;
	
	curSlot = tx_ctrl_wridx;
	tx_ctrl_buf[curSlot] = data;
	
	nextSlot = curSlot + 1;
	if(nextSlot >= TXBUF_DEPTH_CTRL)
		nextSlot = 0;

	savePMIC = PMIC.CTRL;
	PMIC.CTRL = savePMIC & ~PMIC_LOLVLEN_bm;
	tx_ctrl_wridx = nextSlot;
	USARTD1.CTRLA = USART_RXCINTLVL_MED_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_LO_gc;
	PMIC.CTRL = savePMIC;

} /* WriteByte_Ctrl */


#define TOHEX(x) (((x) & 0x0F) <= 9 ? ('0' + ((x) & 0x0F)) : ('A' + ((x) & 0x0F) - 10))

static int Ctrl_putchar(char c, FILE *stream) {
	
	static uint8_t checksum;
	
	WriteByte_Ctrl((uint8_t) c);
	
	if(c == '>') {
		checksum = 0;
	}
	else if(c == '<') {
		WriteByte_Ctrl(TOHEX((uint8_t) checksum >> 4));
		WriteByte_Ctrl(TOHEX((uint8_t) checksum));
	}
	else {
		checksum ^= (uint8_t) c;
	}
	
	return 0;
	
} /* Ctrl_putchar */


ISR(USARTD1_RXC_vect) {
	
	uint8_t curSlot, nextSlot;
	
	curSlot = rx_ctrl_wridx;
	rx_ctrl_buf[curSlot] = USARTD1.DATA;
	
	nextSlot = curSlot + 1;
	if(nextSlot >= RXBUF_DEPTH_CTRL)
		nextSlot = 0;
		
	if(nextSlot != rx_ctrl_rdidx)
		rx_ctrl_wridx = nextSlot;
	
} /* ISR(USARTE0_RXC_vect) */


ISR(USARTD1_DRE_vect) {
	
	uint8_t curSlot, nextSlot, lastSlot;
	
	nextSlot = curSlot = tx_ctrl_rdidx;
	lastSlot = tx_ctrl_wridx;
	
	if(curSlot != lastSlot) {
		USARTD1.DATA = tx_ctrl_buf[curSlot];
		nextSlot = curSlot + 1;
		if(nextSlot >= TXBUF_DEPTH_CTRL)
			nextSlot = 0;
	}
	if(nextSlot == lastSlot)
		USARTD1.CTRLA = USART_RXCINTLVL_MED_gc | USART_TXCINTLVL_OFF_gc | USART_DREINTLVL_OFF_gc;
	
	tx_ctrl_rdidx = nextSlot;
	
} /* ISR(USARTE0_DRE_vect) */


