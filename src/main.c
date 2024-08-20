/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
#include "../include/main.h"

/* void SysInit(void) { */
/* } */

int main (){
        /* printf("Hey arm1!!"); */
        HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
        int my_shit = 5;
        return 0;
}
