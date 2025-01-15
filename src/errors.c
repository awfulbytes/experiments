#include "errors.h"

void handle_it_enter(void){
    while(1) {
        LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        LL_mDelay(4000);
        break;
    }
}

void handle_it(void){
    while(1) {
        LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        LL_mDelay(200);
    }
}
