
/*
 * driverDriver.c
 *
 * Created: 19-11-2020 15:11:26
 *  Author: ceder
 */ 

#include "driverDriver.h"

void init_clock(){
	
	TCD0.CTRLB		=	TC0_CCDEN_bm | TC_WGMODE_SINGLESLOPE_gc;
	TCD0.CTRLA		=	TC_CLKSEL_DIV1_gc;						//one second is 125.000
	TCD0.PER		=   PERVal1KHz;									//frequentie = 10k hz
	TCD0.CCD		=	0;							//zet de pwm op een kwart
	

	TCD0.INTCTRLA	=	TC_OVFINTLVL_LO_gc;						//enable interrupt
	

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
	((((uint32_t) spi_transfer(0) ) << 0)  & 0x000000FF) ;
	
	SPI_PORT.OUTSET = CSN_bm;

	return result;
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


void setDriverSettings(){
	
	tmcWriteReg(TMC2160_GLOBAL_SCALER, 256);
	tmcWriteReg(TMC2160_CHOPCONF, 0x080100C3);
	tmcWriteReg(TMC2160_IHOLD_IRUN, 0x00061F10);
	tmcWriteReg(TMC2160_TPOWERDOWN, 0x0000000A);
	tmcWriteReg(TMC2160_GCONF, 0x00000004);
	tmcWriteReg(TMC2160_TPWMTHRS, 0x000001F4);
}

void init_driver(){
	
	spi_init();
	
	setDriverSettings();
	
	DRIVER_PORT.DIR = ENABLE_bm | STEP_bm | DIR_bm ;
	DRIVER_PORT.OUTSET = ENABLE_bm;
	DRIVER_PORT.OUTCLR = STEP_bm | DIR_bm;
	
	DRIVER_PORT.PIN0CTRL = PORT_OPC_PULLDOWN_gc;
	DRIVER_PORT.PIN1CTRL = PORT_OPC_PULLDOWN_gc;
	DRIVER_PORT.PIN2CTRL = PORT_OPC_PULLDOWN_gc;
	
	
}