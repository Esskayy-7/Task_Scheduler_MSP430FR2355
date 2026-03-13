#include "Scheduler/Scheduler.h"
#include "Timer.h"
#include <msp430.h>
#define COUNTS_PER_TICK 328 // 328 counts = 1 tick in tracker variable


void timer_init(void) {
        // setup timers
       TB0CTL |= TBCLR; // clear back to default values
       TB0CTL |= MC__CONTINUOUS; // put timer in continuous mode
       TB0CTL |= TBSSEL__ACLK; //Use ACLK(~32kHz)
       TB0CCR1 = TB0R + COUNTS_PER_TICK;


       // setup Capture Compare IRQs
       __enable_interrupt(); // global enable
       TB0CCTL1 |= CCIE;     // enable timer interrupt
       TB0CCTL1 &= ~CCIFG;   // Clear timer interrupt flag
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_TB0_CCR1(void)
{
    tracker++;
    TB0CCR1 += COUNTS_PER_TICK;
    TB0CCTL1 &= ~CCIFG;

}
