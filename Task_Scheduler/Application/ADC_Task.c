#include "Drivers/ADC.h"
#include "ADC_Task.h"

void run_ADC_once(void){
    ADC_init();
}

void ADC_read(void) {
    start_conversion();
}
