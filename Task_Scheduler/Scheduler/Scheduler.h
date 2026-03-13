/*
 * Scheduler.h
 *
 *  Created on: Mar 11, 2026
 *      Author: Asus
 */

#ifndef SCHEDULER_SCHEDULER_H_
#define SCHEDULER_SCHEDULER_H_

#include <msp430.h>
#include <stdint.h>

#define NUMBER_OF_TASKS 4

extern volatile uint32_t tracker; // tracks the ticks in the system
extern unsigned int* T_LED_1; // LED1 period
extern unsigned int* T_LED_2; // LED2 period
extern unsigned int T_LED_1_MAX; // Maximum Period length for LED1
extern unsigned int T_LED_2_MAX; // MAximum Period length for LED2

typedef struct {
    unsigned int period; // number of ticks before calling task function (tracker - last_run)
    void (*pf)(void); // pointer to function that performs task
    uint32_t last_run; // last tick when it ran
}Task;


void run_scheduler(void);



#endif /* SCHEDULER_SCHEDULER_H_ */

