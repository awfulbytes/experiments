#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_bus.h"
void gpio_init(void){
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
}
