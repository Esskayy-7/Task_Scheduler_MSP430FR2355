#include "Scheduler.h"
#include "Application/GPIO_Task.h"
#include "Application/UART_Task.h"
#include "Application/ADC_Task.h"

//  static array with controlled external access through exported pointers.
static Task tasks[] = {
              {0, blink_led_1 , 0},
              {0, blink_led_2 , 0},
              {25, UART_read, 0},
              {50, ADC_read, 0}
};

volatile uint32_t tracker = 0;
unsigned int* T_LED_1 = &tasks[0].period;
unsigned int* T_LED_2 = &tasks[1].period;
unsigned int T_LED_1_MAX = 25;
unsigned int T_LED_2_MAX = 50;



void run_scheduler(void)  {

   while(1) {
       int i;
       for(i = 0; i < NUMBER_OF_TASKS; i++) {
           if(tracker-tasks[i].last_run >= tasks[i].period){
               tasks[i].pf();
               tasks[i].last_run = tracker;
           }
       }
   }
}
