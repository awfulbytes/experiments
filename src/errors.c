#include "stm32g0xx_ll_utils.h"


void handle_it_enter(void){
  while(1) {
    /* BSP_LED_Toggle(LED4); */
    LL_mDelay(4000);
  }
}

void handle_it(void){
  while(1) {
    /* BSP_LED_Toggle(LED4); */
    LL_mDelay(200);
  }
}
