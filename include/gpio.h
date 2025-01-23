#pragma once
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_bus.h"

#define LED4_PIN                           LL_GPIO_PIN_5
#define LED4_GPIO_PORT                     GPIOA
/* #define LED4_GPIO_CLK_ENABLE()             LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA) */

#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW  500
#define LED_BLINK_ERROR 1000

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
};
struct button {
    struct gpio pin;
    struct exti exti;
    volatile uint8_t state;
    volatile uint8_t flag;
};

void gpio_init(struct button *p, struct gpio *led, struct gpio *adc_pin);

