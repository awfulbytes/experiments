#include "gpio.h"

struct encoder_channel {
    struct gpio pin;
    struct exti it_settings;
    char flag;
    uint32_t value;
};

struct encoder {
    struct encoder_channel A, B;
    uint16_t increment;
    bool  direction;
};

#pragma GCC diagnostic ignored "-Wunused-function"
static void exti_enc_setup(struct encoder_channel channel[static 1]){
    LL_EXTI_SetEXTISource(channel->it_settings.exti_port_conf, channel->it_settings.exti_line_conf);

    LL_EXTI_EnableFallingTrig_0_31(channel->it_settings.exti_line);

    LL_EXTI_EnableIT_0_31(channel->it_settings.exti_line);

    NVIC_SetPriority(channel->it_settings.exti_irqn, 0x00);
    NVIC_EnableIRQ(channel->it_settings.exti_irqn);
}

void enc_init(struct encoder *enc);
static void increment_encoder(struct encoder encoder[static 1]);
static void constrain_encoder_to_distortion_level(struct encoder enc[static 1]);

void wave_button_callback(struct button *abut);
void WaitForUserButtonPress (struct button *button);
