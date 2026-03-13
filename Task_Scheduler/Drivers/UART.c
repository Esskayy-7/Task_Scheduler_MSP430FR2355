#include <msp430.h>
#include "UART.h"

void UART_init(void) {
    //--Setup UART
        UCA1CTLW0  |= UCSWRST; // put UART A1 into SW reset

        UCA1CTLW0 |= UCSSEL__SMCLK; // Choose SMCLK for UART A1
        UCA1BRW = 8; // Set Prescalar to 8
        UCA1MCTLW |= 0xD600; // COnfigure modulation settings + low frequency

        //--setup ports
        P4SEL1 &= ~BIT3;  P4SEL0 |= BIT3;  //P4SEL1.3 : P4SEL0.3 = 01 and Puts UART Tx on P4.3

        UCA1CTLW0 &= ~UCSWRST;  // put UART A1 into SW reset
}

void transmit_character(void) {
    UCA1TXBUF = ADCMEM0; // Put ADC value into TX buffer
}
