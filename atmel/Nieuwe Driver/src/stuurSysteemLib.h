#ifndef _AUTO_LIB_
#define _AUTO_LIB_
	
	#define F_CPU 32000000UL
	
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <util/delay.h>
  #include <stdio.h>
  #include <stdint.h>
  #include <stdbool.h>
  #include <stdlib.h>
  #include <math.h>
  
  #include "serial.h"
  
  #define MSTEP						32
  
  #define CLOCKSPER360DEG			200 * MSTEP
  #define DEGREE_PER_CLOCK			1.8 / MSTEP
  #define DEGREERATIO				40
  #define MAXANGLE					32
  #define LEFT						0
  #define RIGHT						1
  #define CENTER_OFFSET				309
  

  #define STOP_CLK()  {		TCC0.CTRLB	&=	~TC0_CCAEN_bm;}
  #define START_CLK() {		TCC0.CTRLB	|=	TC0_CCAEN_bm;}


  typedef enum {
    ENABLED = 1,
    DISABLED = 0
  } t_StepperState;
	
	extern volatile bool steering;
	extern volatile bool calibrating;

	
	extern volatile uint32_t clockTicks;
	extern volatile uint32_t degrees;
	extern volatile uint32_t absoluteRadius;
	extern volatile int32_t currentHeading;

	extern volatile bool endLeft;
	extern volatile bool endRight;
	
	extern FILE gCtrl_IO;

	void calibrate();
	int32_t setAbsoluteHeading(int32_t curHead, int32_t newHead);
	int32_t calcClocks2Degrees(uint32_t clocks, bool direction);


  
  



#endif