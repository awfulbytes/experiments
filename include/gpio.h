#pragma once
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_bus.h"

// #define DEBUGDAC
#if defined(DEBUG) || defined(DEBUGDAC) || defined(DEBUGDAC1)
void debug_tim2_pin31(void){
    LL_GPIO_InitTypeDef gpio_init = {0};
#if defined(DEBUG) || defined(DEBUGDAC)
    gpio_init.Pin = LL_GPIO_PIN_3;
#endif // DEBUG
    gpio_init.Mode = LL_GPIO_MODE_OUTPUT;
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &gpio_init);
}
#else
#endif // DEBUG || DEBUGDAC

struct gpio {
    uint32_t pin_id;
    uint32_t mode;
    uint32_t pull;
    GPIO_TypeDef *port_id;
};
struct exti {
    volatile uint32_t exti_line;
    volatile uint32_t exti_irqn;
    volatile uint32_t exti_port_conf;
    volatile uint32_t exti_line_conf;
    EXTI_TypeDef *exti;
};
struct button {
    struct gpio pin;
    struct exti exti;
    volatile uint16_t state;
    volatile uint8_t flag;
};

void gpio_init(struct button **p, struct gpio **led, struct gpio *adc_pin);
