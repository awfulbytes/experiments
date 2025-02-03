#include "gpio.h"

static void exti_setup(struct button *p){
    LL_EXTI_SetEXTISource(p->exti.exti_port_conf, p->exti.exti_line_conf);
    LL_EXTI_EnableRisingTrig_0_31(p->exti.exti_line);
    LL_EXTI_EnableIT_0_31(p->exti.exti_line);
    NVIC_SetPriority(p->exti.exti_irqn, 0x05);
    NVIC_EnableIRQ(p->exti.exti_irqn);
}

void gpio_init(struct button **dac1_ch1_2_wave_buttons, struct gpio **dacs, struct gpio *adc_pin){
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA |
                            LL_IOP_GRP1_PERIPH_GPIOB |
                            LL_IOP_GRP1_PERIPH_GPIOC);

    for (int i=0; i<2; i++) {
        LL_GPIO_SetPinMode(dac1_ch1_2_wave_buttons[i]->pin.port_id,
                           dac1_ch1_2_wave_buttons[i]->pin.pin_id,
                           dac1_ch1_2_wave_buttons[i]->pin.mode);
        LL_GPIO_SetPinPull(dac1_ch1_2_wave_buttons[i]->pin.port_id,
                           dac1_ch1_2_wave_buttons[i]->pin.pin_id,
                           dac1_ch1_2_wave_buttons[i]->pin.pull);
        exti_setup(dac1_ch1_2_wave_buttons[i]);
        LL_GPIO_SetPinMode(dacs[i]->port_id, dacs[i]->pin_id, dacs[i]->mode);
    }

    LL_GPIO_SetPinMode(adc_pin->port_id,
                       adc_pin->pin_id,
                       adc_pin->mode);
    LL_GPIO_SetPinPull(adc_pin->port_id,
                       adc_pin->pin_id,
                       adc_pin->pull);
}
