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
    volatile uint16_t      increment;
    volatile struct button virtual_wave_button;
};

#pragma GCC diagnostic ignored "-Wunused-function"
static void exti_enc_setup(struct encoder_channel channel[static 1]){

    LL_EXTI_SetEXTISource(channel->it_settings.exti_port_conf, channel->it_settings.exti_line_conf);

    LL_EXTI_EnableFallingTrig_0_31(channel->it_settings.exti_line);
    LL_EXTI_EnableRisingTrig_0_31(channel->it_settings.exti_line);
    LL_EXTI_EnableIT_0_31(channel->it_settings.exti_line);

    NVIC_SetPriority(channel->it_settings.exti_irqn, 0x40);
    NVIC_EnableIRQ(channel->it_settings.exti_irqn);
}

void enc_init(struct encoder *enc);

void wave_button_callback(struct button *abut);
void WaitForUserButtonPress (struct button *button);

volatile void* scan_and_apply_current_modulations(struct encoder enc[static 1],
                                                  struct nco osillator[static 1]);
