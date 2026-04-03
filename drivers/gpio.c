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
    analog_gpio(adc_pin, a, why);
}

void set_gpio(volatile struct gpio *pin){
    LL_GPIO_SetPinMode(pin->port_id, pin->pin_id, pin->mode);
    LL_GPIO_SetPinPull(pin->port_id, pin->pin_id, pin->pull);
}

static void analog_gpio(struct gpio **adc_pins, uint16_t a, uint16_t why){
    for (why=0; why < a; why++){
        set_gpio(adc_pins[why]);
    }
}

static void wave_choice_gpio(volatile struct button **wave_butts, uint16_t c, uint16_t why){
    for (why=0; why < c; why++) {
        set_gpio(&wave_butts[why]->pin);
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

void gpio_logic_high(struct gpio *gpio){
    gpio->port_id->ODR |= (1U << gpio->id);
}

void gpio_logic_low(struct gpio *gpio){
    gpio->port_id->ODR &= ~(1U << gpio->id);
}
