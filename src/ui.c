#include "ui.h"
#include "nco.h"
#include <stdint.h>

static void exti_enc_setup(struct encoder_channel channel[static 1]);

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

__attribute((pure, always_inline)) inline enum freq_modes change_osc_pitch_mode(struct nco oscillator[static 1]){
    return (oscillator->mode == free) ? v_per_octave : free;
}

static void increment_encoder(struct encoder encoder[static 1]){
    encoder->direction = read_b_on_falling_a(encoder);
    if (!encoder->direction)
        --encoder->increment;
    else
        ++encoder->increment;
}

static void constrain_encoder_to_distortion_level(struct encoder encoder[static 1]){
    register uint16_t incr = encoder->increment;
    if (incr > hell && incr < hell + 0xfff)
        incr = hell;
    else if (incr > hell + 0xfff)
        incr = entrance;

    encoder->increment = incr;
}

static void bit_bang_encoder(struct encoder enc[static 1]){
    increment_encoder(enc);
    constrain_encoder_to_distortion_level(enc);
}


void scan_and_apply_oscillator_modulations(struct encoder enc[static 1], struct nco osillator[static 1]){
    if (enc->A.flag == 0x69 && osillator->phase_pending_update){
        if(osillator->distortion.on){
            bit_bang_encoder(enc);
        }
        else
            osillator->mode = change_osc_pitch_mode(osillator);
        switch (osillator->distortion.past_dante) {

            case hell:
                osillator->distortion.dante = enc->increment = ninteenth;
                break;
            default:
                osillator->distortion.dante = enc->increment;
                break;
        }
        enc->A.flag = 'D';
        osillator->distortion.past_dante = osillator->distortion.dante;
    }
}
