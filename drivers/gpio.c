#include "gpio.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_bus.h"
/* #include "stm32g0xx_nucleo.h" */
void gpio_init(void){
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA |
                            LL_IOP_GRP1_PERIPH_GPIOC);

    /* Configure GPIO for BUTTON */
    LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_DOWN);

    /* Connect External Line to the GPIO */
    USER_BUTTON_SYSCFG_SET_EXTI();

    USER_BUTTON_EXTI_FALLING_TRIG_ENABLE();

    USER_BUTTON_EXTI_LINE_ENABLE();
    /* NOTE:: You set and then enable bro... */
    NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 0x02);
    NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);

    LL_GPIO_SetPinMode(GPIOA, LED4_PIN, LL_GPIO_MODE_OUTPUT);
}
