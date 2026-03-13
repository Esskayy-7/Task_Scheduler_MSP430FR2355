#include "Application/ADC_Task.h"
#include <msp430.h> 
#include "Application/GPIO_Task.h"
#include "Scheduler/Scheduler.h"
#include "Application/Timer_Task.h"
#include "Application/UART_Task.h"
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	run_GPIO_once();
	run_TIMER_once();
	run_UART_once();
	run_ADC_once();
	
	run_scheduler();
	return 0;
}
