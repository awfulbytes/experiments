#include "gpio.h"

static void exti_setup(volatile struct button *p){
    LL_EXTI_SetEXTISource(p->exti.exti_port_conf, p->exti.exti_line_conf);
    LL_EXTI_EnableRisingTrig_0_31(p->exti.exti_line);
    LL_EXTI_EnableIT_0_31(p->exti.exti_line);
    NVIC_SetPriority(p->exti.exti_irqn, 0x00);
    NVIC_EnableIRQ(p->exti.exti_irqn);
}

static void wave_choice_gpio(volatile struct button **wave_butts, uint16_t c, uint16_t why);
static void analog_gpio(struct gpio **adc_pins, uint16_t a, uint16_t why);
static void dac_gpio(struct gpio **dac_pins, uint16_t d, uint16_t why);

void gpio_init(volatile struct button **choice_buttons,
               struct gpio   **dacs,
               struct gpio   **adc_pin,
               uint16_t c, uint16_t d, uint16_t a, uint16_t why){
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA |
                            LL_IOP_GRP1_PERIPH_GPIOB |
                            LL_IOP_GRP1_PERIPH_GPIOC);

    wave_choice_gpio(choice_buttons, c, why);
    dac_gpio(dacs, d, why);
    // for (why=0; why < d; why++){
    //     LL_GPIO_SetPinMode(dacs[why]->port_id, dacs[why]->pin_id, dacs[why]->mode);
    // }
    analog_gpio(adc_pin, a, why);
}

static void analog_gpio(struct gpio **adc_pins, uint16_t a, uint16_t why){
    for (why=0; why < a; why++){
        LL_GPIO_SetPinMode(adc_pins[why]->port_id,
                           adc_pins[why]->pin_id,
                           adc_pins[why]->mode);

        LL_GPIO_SetPinPull(adc_pins[why]->port_id,
                           adc_pins[why]->pin_id,
                           adc_pins[why]->pull);
    }
}

static void wave_choice_gpio(volatile struct button **wave_butts, uint16_t c, uint16_t why){
    for (why=0; why < c; why++) {
        LL_GPIO_SetPinMode(wave_butts[why]->pin.port_id,
                           wave_butts[why]->pin.pin_id,
                           wave_butts[why]->pin.mode);

        LL_GPIO_SetPinPull(wave_butts[why]->pin.port_id,
                           wave_butts[why]->pin.pin_id,
                           wave_butts[why]->pin.pull);
        exti_setup(wave_butts[why]);
    }
}
static void dac_gpio(struct gpio **dac_pins, uint16_t d, uint16_t why){
    for(why=0; why < d; why++){
        LL_GPIO_SetPinMode(dac_pins[why]->port_id, dac_pins[why]->pin_id, dac_pins[why]->mode);
    }
}

char read_gpio(struct gpio *pin) {
    return (pin->port_id->IDR & (1U << pin->id) ? 1U : 0);
}

void write_gpio(struct gpio *pin, unsigned char state) {
    if (state > 1)
        state = 1;
    pin->port_id->ODR &= (state << pin->pin_id);
}
