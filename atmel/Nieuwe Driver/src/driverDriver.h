
/*
 * driverDriver.h
 *
 * Created: 19-11-2020 15:11:00
 *  Author: ceder
 */ 

#define  F_CPU   2000000UL     //!< System clock is 32 MHz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "registerMapping.h"

#define SPI_PORT	PORTE
#define SCK_bm		PIN1_bm			//pin d8	
#define MISO_bm		PIN2_bm			//pin d9	//SDO
#define MOSI_bm		PIN3_bm			//pin d10	//SDI
#define CSN_bm		PIN0_bm								///LETOPPPPPPPPPPPPSCHEMA

#define STARTCLOCK()	TCD0.CCD		=	TCD0.PER/2 - 1							//zet de PWM op een kwart
#define STOPCLOCK()		TCD0.CCD		=	0										//zet PWM uit

#define DRIVER_PORT		PORTD
#define ENABLE_bm		PIN1_bm
#define DIR_bm			PIN2_bm
#define STEP_bm			PIN3_bm
//////////////////////////////////////////////////////////////////////////
#define DIAG0_bm		PIN4_bm
#define DIAG1_bm		PIN5_bm

#define	PERVal1KHz	3999

#define ENABLEDRIVER()	DRIVER_PORT.OUTCLR = ENABLE_bm

void init_clock();
uint8_t spi_transfer(uint8_t input);
void tmcWriteReg(uint8_t reg, uint32_t val);
uint32_t tmcReadReg(uint8_t reg);
void spi_init(void);
void setDriverSettings();
void init_driver();