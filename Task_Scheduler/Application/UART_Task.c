#include "Drivers/UART.h"
#include "UART_Task.h"

void run_UART_once(void){
    UART_init();
}

void UART_read(void){
    transmit_character();
}
