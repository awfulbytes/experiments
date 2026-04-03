#include "gpio.h"
#include "nco.h"

enum waves {SINE, SAWU, SAWD, PULSE, WAVE_CTR};
enum direction {cw, ccw};

struct encoder_channel {
    struct   gpio     pin;
    struct   exti     it_settings;
    volatile char     flag;
    volatile uint8_t  val;
    volatile uint32_t value[2];
};

struct encoder {
    struct encoder_channel A, B;
    enum   direction       direction;
    /* volatile uint16_t      increment; */
    volatile struct button virtual_wave_button;
};

struct flip_switch {
    struct gpio pins[2];
    struct exti it[2];
    uint32_t    priority[2];
};

static void enable_rising(uint32_t exti_line){
    LL_EXTI_EnableRisingTrig_0_31(exti_line);
}

static void enable_falling(uint32_t exti_line){
    LL_EXTI_EnableFallingTrig_0_31(exti_line);
}

static void exti_flip_setup(struct flip_switch *f, uint8_t idx){
    LL_EXTI_SetEXTISource(f->it[idx].exti_port_conf, f->it[idx].exti_line_conf);

    // see what we need here!
    enable_rising(f->it[idx].exti_line);
    LL_EXTI_EnableIT_0_31(f->it[idx].exti_line);

    NVIC_SetPriority(f->it[idx].exti_irqn, f->priority[idx]);
    NVIC_EnableIRQ(f->it[idx].exti_irqn);
}

#pragma GCC diagnostic ignored "-Wunused-function"
static void exti_enc_setup(struct encoder_channel channel[static 1]){

    LL_EXTI_SetEXTISource(channel->it_settings.exti_port_conf, channel->it_settings.exti_line_conf);

    enable_falling(channel->it_settings.exti_line);
    enable_rising(channel->it_settings.exti_line);
    LL_EXTI_EnableIT_0_31(channel->it_settings.exti_line);

    NVIC_SetPriority(channel->it_settings.exti_irqn, 0x40);
    NVIC_EnableIRQ(channel->it_settings.exti_irqn);
}

void enc_init(struct encoder *enc);
void button_callback(struct button *abut);
void WaitForUserButtonPress (struct button *button);
volatile void* apply_modulations_callback(struct encoder enc[static 1],
                                          struct nco o[static 1]);
void align_phase(volatile struct nco *o[2]);
void set_flip_switch(struct flip_switch *f, uint8_t pin_idx);
