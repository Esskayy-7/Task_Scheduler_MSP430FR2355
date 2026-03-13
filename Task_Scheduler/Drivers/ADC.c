#include "ADC.h"
#include "Scheduler/Scheduler.h"

void ADC_init(void) {
    P1SEL1 |= BIT2; P1SEL0 |= BIT2;

    //---configure the ADC
            ADCCTL0 &= ~ADCSHT; // set clock cycles = 16(01)
            ADCCTL0 |= ADCSHT_2;
            ADCCTL0 |= ADCON;  // turn on ADC

            ADCCTL1 |= ADCSSEL_2; // choose SMCLK
            ADCCTL1 |= ADCSHP;  // sample signal source = sampling timer

            ADCCTL2 &= ~ADCRES;  // clear resolution
            ADCCTL2 |= ADC_RES; // bit resolution(8-bit here)

            ADCMCTL0 |= ADCINCH_2; // ADC input = A2(P1.2)

            //--- Setup IRQ
            ADCIE |= ADCIE0;
}

void start_conversion(void){
    ADCCTL0 |= ADCENC | ADCSC; // start conversion
}

//---ISR
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void) {

    //period of LED = max period * fraction of ADC value(ranging from ~0 to ~1)
    *T_LED_1 = T_LED_1_MAX * (ADCMEM0) / (1 << RES_EXP);
    *T_LED_2 = T_LED_2_MAX * ((1 << RES_EXP) - ADCMEM0) / (1 << RES_EXP);
}
