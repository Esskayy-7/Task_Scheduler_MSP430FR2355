/*
 * GPIO.h
 *
 *  Created on: Mar 11, 2026
 *      Author: Asus
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include <stdint.h>


void GPIO_ToggleLED_1(void);
void GPIO_ToggleLED_2(void);
void GPIO_init(void);

#define DOUBLE_PRESS_TIME_LIMIT 50 // number of ticks for double click to be valid


#endif /* DRIVERS_GPIO_H_ */
