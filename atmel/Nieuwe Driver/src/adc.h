/*
 * adc.h
 *
 * Created: 4-5-2018 21:11:25
 *  Author: Ceder Wink
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

#define ADC_MAX 2000
#define ADC_MIN 00


void init_adc();
int16_t read_adc();
int16_t read_adc_mv();


#endif /* ADC_H_ */