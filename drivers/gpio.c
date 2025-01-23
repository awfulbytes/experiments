#include "gpio.h"
#include "stm32g071xx.h"

static void exti_setup(struct button *p){
    LL_EXTI_SetEXTISource(p->exti.exti_port_conf, p->exti.exti_line_conf);
    LL_EXTI_EnableRisingTrig_0_31(p->exti.exti_line);
    LL_EXTI_EnableIT_0_31(p->exti.exti_line);
    NVIC_SetPriority(p->exti.exti_irqn, 0x05);
    NVIC_EnableIRQ(p->exti.exti_irqn);
}

void gpio_init(struct button *wave_button, struct gpio *led, struct gpio *adc_pin){
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA |
                            LL_IOP_GRP1_PERIPH_GPIOC);
    wave_button->pin.port_id = GPIOC;
    wave_button->pin.pin_id = LL_GPIO_PIN_10;
    wave_button->pin.mode = LL_GPIO_MODE_INPUT;
    wave_button->pin.pull = LL_GPIO_PULL_NO; // test
    wave_button->exti.exti_irqn = EXTI4_15_IRQn;
    wave_button->exti.exti_line = LL_EXTI_LINE_10;
    wave_button->exti.exti_port_conf = LL_EXTI_CONFIG_PORTC;
    wave_button->exti.exti_line_conf = LL_EXTI_CONFIG_LINE10;
    LL_GPIO_SetPinMode(wave_button->pin.port_id,
                       wave_button->pin.pin_id,
                       wave_button->pin.mode);
    LL_GPIO_SetPinPull(wave_button->pin.port_id,
                       wave_button->pin.pin_id,
                       wave_button->pin.pull);
    led->port_id = GPIOA;
    led->pin_id = LL_GPIO_PIN_5;
    led->mode = LL_GPIO_MODE_ANALOG;
    LL_GPIO_SetPinMode(led->port_id, led->pin_id, led->mode);

    adc_pin->port_id = GPIOA;
    adc_pin->pin_id = LL_GPIO_PIN_0;
    adc_pin->mode = LL_GPIO_MODE_ANALOG;
    adc_pin->pull = LL_GPIO_PULL_NO;
    LL_GPIO_SetPinMode(adc_pin->port_id,
                       adc_pin->pin_id,
                       adc_pin->mode);
    LL_GPIO_SetPinPull(adc_pin->port_id,
                       adc_pin->pin_id,
                       adc_pin->pull);
    exti_setup(wave_button);
}
