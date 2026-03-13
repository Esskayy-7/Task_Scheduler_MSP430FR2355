#include "GPIO_Task.h"
#include "Drivers/GPIO.h"


void blink_led_1(void){
    GPIO_ToggleLED_1();
}

void blink_led_2(void){
    GPIO_ToggleLED_2();
}


void run_GPIO_once(void){
    GPIO_init();
}
