/*
 * sendNrf.c
 *
 * Created: 11-11-2020 22:11:31
 * Author : Ceder Wink
 */ 

#define  F_CPU   32000000UL     //!< System clock is 32 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "src/serial.h"
#include "src/clock.h"
#include "registers.h"

//#include "src/pinout.h"

#define SPI_PORT	PORTE   // 
#define SDO_bm		PIN2_bm // 
#define SDI_bm		PIN3_bm // SDI poort D12
#define SCK_bm		PIN1_bm // SKC
#define CSN_bm		PIN0_bm			


void init_spi(void){
	
	SPI_PORT.DIR = (SDI_bm | SCK_bm | CSN_bm) & ~(SDO_bm);
	SPI_PORT.OUTSET = CSN_bm;
	
	USARTE0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	USARTE0.CTRLC = USART_CMODE_MSPI_gc;
	
	USARTE0.BAUDCTRLB = 16;
	USARTE0.BAUDCTRLA = 17;
	
	SPI_PORT.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
	SPI_PORT.PIN2CTRL = PORT_OPC_PULLDOWN_gc;
	SPI_PORT.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
	SPI_PORT.PIN4CTRL = PORT_OPC_PULLDOWN_gc;
	
}

void readByteBin(uint32_t data, uint8_t size){
	for(uint32_t i = 0x80000000 ; i > 0; i >>= 1){
		if(data & i ) printf("1");
		else printf("0");
	}
	printf("\n");
}

uint8_t spi_transfer(uint8_t input){
	
	USARTE0.DATA  = input;
	while( !(USARTE0.STATUS & USART_TXCIF_bm));
	USARTE0.STATUS |= USART_TXCIF_bm;
	
	return USARTE0.DATA;
	
}

void AccelWriteReg(uint8_t reg, uint8_t val){
	
	SPI_PORT.OUTCLR = CSN_bm;
	
	spi_transfer(reg);
	spi_transfer(val);
	
	SPI_PORT.OUTSET = CSN_bm;

}

uint8_t AccelReadReg(uint8_t reg){
	
	SPI_PORT.OUTCLR = CSN_bm;
	
	spi_transfer(reg | 0x80);
	int value = spi_transfer(0x00);
	
	SPI_PORT.OUTSET = CSN_bm;
	return value;
}



int main(void)
{
	cli();
	
	Config32MHzClock_Ext16M();
	InitSerial();
	init_spi();
	
	sei();

	_delay_ms(300);

	int val = AccelReadReg(WHO_AM_I_REG);


	fprintf(&gCtrl_IO, "hello world");
	fprintf(&gCtrl_IO, "Value: %d", val);
	while(1){
	

		
	};
}

