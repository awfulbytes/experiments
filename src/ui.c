#include "ui.h"
#include "stm32g071xx.h"
#include "nco.h"
#include <stdint.h>

static void exti_enc_setup(struct encoder_channel channel[static 1]);

void phase_dist_button_callback(struct button *abut){
    switch (abut->state) {
        case 8:
            abut->state >>= abut->state;
            break;
        default:
            ++abut->state;
            break;
    }
    // abut->flag = 'D';
}

inline void wave_button_callback
(struct button *abut) {
    // NOTE:: need some more work but elswhere..
    switch (abut->state) {
        case 3:
            abut->state >>= abut->state;
            break;
        default:
            ++abut->state;
            break;
    }
    abut->flag = 'D';
}

void enc_init(struct encoder *enc){
    LL_GPIO_SetPinMode(enc->A.pin.port_id, enc->A.pin.pin_id, enc->A.pin.mode);
    LL_GPIO_SetPinMode(enc->B.pin.port_id, enc->B.pin.pin_id, enc->B.pin.mode);

    LL_GPIO_SetPinPull(enc->A.pin.port_id, enc->A.pin.pin_id, enc->A.pin.pull);
    LL_GPIO_SetPinPull(enc->B.pin.port_id, enc->B.pin.pin_id, enc->B.pin.pull);

    exti_enc_setup(&enc->A);
}

__attribute__((pure, always_inline)) inline bool read_b_on_falling_a(struct encoder encoder[static 1]){
    register bool clockwise;
    clockwise = (encoder->B.value == 1) ? true : false;
    return clockwise;
}

void increment_encoder(struct encoder encoder[static 1]){
    encoder->direction = read_b_on_falling_a(encoder);
    if (!encoder->direction)
        --encoder->increment;
    else
        ++encoder->increment;
}

void constrain_encoder_to_distortion_level(struct encoder enc[static 1]){
    register uint16_t incr = enc->increment;
    if (incr > hell && incr < hell + 0xfff)
        incr = hell;
    else if (incr > hell + 0xfff)
        incr = entrance;

    enc->increment = incr;
}
