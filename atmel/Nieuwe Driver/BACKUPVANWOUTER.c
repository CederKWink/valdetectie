/*
 * sendNrf.c
 *
 * Created: 11-11-2020 22:11:31
 * Author : ceder
 */ 

#define  F_CPU   2000000UL     //!< System clock is 32 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "serialF0.h"
#include "registerMapping.h"

#define SPI_PORT	PORTE   // 
#define MISO_bm		PIN2_bm // 
#define MOSI_bm		PIN3_bm // SDI poort D12
#define SCK_bm		PIN1_bm // SKC
#define CSN_bm		PIN4_bm // 

#define SAMPLES 20
#define ONESECOND 31250

volatile uint32_t seconds = 0;
volatile uint32_t minutes = 0;
volatile bool newOfv = false;
volatile bool stepmissed = false;

void init_rtc(){	
		
		PORTD.DIRSET	=	PIN3_bm;
		
		TCD0.CTRLB		=	TC0_CCDEN_bm | TC_WGMODE_SINGLESLOPE_gc;
		TCD0.CTRLA		=	TC_CLKSEL_DIV1_gc;						//one second is 125.000
		TCD0.PER		=   4000;									//frequentie = 10k hz
		TCD0.CCD		=	TCD0.PER/2 - 1 ;							//zet de pwm op een kwart
		
		
		TCD0.INTCTRLA	=	TC_OVFINTLVL_LO_gc;						//enable interrupt
		

}

void spi_init(void){
	
	SPI_PORT.DIR = (MOSI_bm | SCK_bm | CSN_bm) & ~(MISO_bm);
	
	USARTE0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
	USARTE0.CTRLC = USART_CMODE_MSPI_gc;
	
	USARTE0.BAUDCTRLB = 0;
	USARTE0.BAUDCTRLA = 1;
	
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

void tmcWriteReg(uint8_t reg, uint32_t val){
	
	SPI_PORT.OUTCLR = CSN_bm;
	
	spi_transfer(reg | 0x80);
	
	//MSB first
	spi_transfer( (uint8_t) ((val >> 24) & 0xFF) );
	spi_transfer( (uint8_t) ((val >> 16) & 0xFF) );
	spi_transfer( (uint8_t) ((val >> 8) & 0xFF) );
	spi_transfer( (uint8_t) ((val >> 0) & 0xFF) );
	
	SPI_PORT.OUTSET = CSN_bm;

}

uint32_t tmcReadReg(uint8_t reg){
	
	uint32_t result = 0;
	
	SPI_PORT.OUTCLR = CSN_bm;
	spi_transfer(reg);
	spi_transfer(0);
	spi_transfer(0);
	spi_transfer(0);
	spi_transfer(0);
	
	SPI_PORT.OUTSET = CSN_bm;
	
	//_delay_ms(1);
	
	SPI_PORT.OUTCLR = CSN_bm;
	
	spi_transfer(reg);
	
	result  = \
	((((uint32_t) spi_transfer(0) ) << 24) & 0xFF000000) | \
	((((uint32_t) spi_transfer(0) ) << 16) & 0x00FF0000) | \
	((((uint32_t) spi_transfer(0) ) << 8)  & 0x0000FF00) | \
	((((uint32_t) spi_transfer(0) ) << 0)  & 0x000000FF);
	
	SPI_PORT.OUTSET = CSN_bm;

	return result;
}

void init_try1(){
	
	uint32_t spiReturn;
	_delay_ms(100);
	printf("Booted\n");

	tmcWriteReg(TMC2160_GCONF, 0x1 | 0x8);
	spiReturn = tmcReadReg(TMC2160_GCONF);
	
	printf("TMC2160 GCONF:\n");
	readByteBin(spiReturn, sizeof(spiReturn));
	spiReturn = tmcReadReg(TMC2160_DRV_CONF);
	
	tmcWriteReg(TMC2160_GLOBAL_SCALER, 254);
	printf("TMC2160_GLOBAL_SCALER is set\n");
	
	tmcWriteReg(TMC2160_IHOLD_IRUN, 0x0F | 0x0800 | 0x010000);
	printf("TMC2160_IHOLD_IRUN is set\n");
	
	tmcWriteReg(TMC2160_TPOWERDOWN, 0xFF);
	printf("TMC2160_TPOWERDOWN is set\n");
	
	tmcWriteReg(TMC2160_TPOWERDOWN, 0xFF);
	printf("TMC2160_TPOWERDOWN is set\n");
	
	tmcWriteReg(TMC2160_GCONF, 0x2 | 0x4 | 0x8);
	spiReturn = tmcReadReg(TMC2160_GCONF);
	
	printf("TMC2160 GCONF:\n");
	readByteBin(spiReturn, sizeof(spiReturn));
	spiReturn = tmcReadReg(TMC2160_DRV_CONF);
	
	tmcWriteReg(TMC2160_PWMCONF, 0xC40C001E);
	printf("TMC2160_PWMCONF is set\n");
	
	spiReturn = tmcReadReg(TMC2160_PWM_SCALE);
	printf("PWM_SCALE:\n");
	readByteBin(spiReturn, sizeof(spiReturn));
	
	spiReturn = tmcReadReg(TMC2160_CHOPCONF);
	printf("TMC2160_CHOPCONF:\n");
	readByteBin(spiReturn, sizeof(spiReturn));
	//0x10 41 01 50
	
}


int main(void)
{
	PORTD.PIN3CTRL = PORT_OPC_PULLDOWN_gc;
	cli();
	init_stream(F_CPU);
	spi_init();
	init_rtc();
	sei();
	
	_delay_ms(100);
	printf("new booted\n");
	
	tmcWriteReg(TMC2160_GLOBAL_SCALER, 256);
	tmcWriteReg(TMC2160_CHOPCONF, 0x0F0100C3);
	tmcWriteReg(TMC2160_IHOLD_IRUN, 0x000610FF);
	tmcWriteReg(TMC2160_TPOWERDOWN, 0x0000000A);
	tmcWriteReg(TMC2160_GCONF, 0x00000004);
	tmcWriteReg(TMC2160_TPWMTHRS, 0x000001F4);
	
	
	_delay_ms(1000);
	_delay_ms(1000);
	TCD0.PER = 0;
	
	while(1){
		if(stepmissed) ;
	}
}

ISR(TCD0_OVF_vect){
	
	if(seconds < 59){
		seconds++;
	}else {
		minutes++;
		seconds = 0;
	}
	newOfv = true;
}
