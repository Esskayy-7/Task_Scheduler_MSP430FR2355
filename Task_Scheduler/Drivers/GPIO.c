#include <msp430.h>
#include "GPIO.h"
#include "Scheduler/Scheduler.h"


static uint32_t last_timer_value = 0;

void GPIO_init(void){
    //set up ports
      P1DIR |= BIT0;    // Set P1.0(LED1) to output
      P6DIR |= BIT6;    // Set P6.6(LED2) to output

      P2DIR &= ~BIT3; // set P2.3 to input
      P2REN |= BIT3; // enable resistor
      P2OUT |= BIT3; // set resistor polarity to pull-up
      P2IES |= BIT3; // set sensitivity from high to low

      // Enable IRQs
      P2IE |= BIT3; // Enable interrupt on P2.3
      P2IFG &= ~BIT3; // Clear Flag

    //Turn on GPIO system
      PM5CTL0 &= ~LOCKLPM5; // clear low power mode 5
}


void GPIO_ToggleLED_1(void){
    P1OUT ^= BIT0;
}

void GPIO_ToggleLED_2(void){
    P6OUT ^= BIT6;
}

/*
 * The function returns 0 if it's a double click. i.e. When the time between two button clicks
 is less than 500ms. Else it returns 1*/
static int set_polarity(void)
{

    if((tracker-last_timer_value) <DOUBLE_PRESS_TIME_LIMIT){
        return 0;
    }
    return 1;
}

/*
 * when the button is pressed once, each of the LED's prtiod is increased by 250ms and when double clicked,
 * each is reduced by 500ms since on a double click, The period is first increased by 250ms, so we have to
 * decrease by 500ms on the second click to get (250ms - 500ms = -250ms)
 * */
static void change_T(int val) {
    if(val) {
        T_LED_1_MAX += 25;
    }  else{
        if(T_LED_1_MAX >= 25){
            T_LED_1_MAX -= 50;
        }
    }
    T_LED_2_MAX = 2 * T_LED_1_MAX; // LED2 is always 2x slower than LED1



}

//------------------ISRs-----------------------
#pragma vector = PORT2_VECTOR
__interrupt void ISR_Port2_S2(void)
{

    change_T(set_polarity());
    last_timer_value = tracker; // the last current tracker value is used to determine if it's a double click
                                // the next time the button is pressed
    P2IFG &= ~BIT3;

}
