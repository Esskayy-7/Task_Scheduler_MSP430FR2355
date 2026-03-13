/*
 * ADC.h
 *
 *  Created on: Mar 13, 2026
 *      Author: Asus
 */


#ifndef DRIVERS_ADC_H_
#define DRIVERS_ADC_H_

#include <msp430.h>
#include <stdint.h>

#define ADC_RES ADCRES_0 // ADC resolution
#define RES_EXP (8 + (ADC_RES/8)) // exponential value (2^RES_EXP) used in calculating period


void ADC_init(void);
void start_conversion(void);




#endif /* DRIVERS_ADC_H_ */
