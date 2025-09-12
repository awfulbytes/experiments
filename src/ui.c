#include "ui.h"
#include "nco.h"
#include <stdint.h>

static void exti_enc_setup(struct encoder_channel channel[static 1]);

inline void wave_button_callback(struct button *abut){
    // nxt:: need some more work but elswhere..
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

__attribute((pure, always_inline))
inline freq_modes_e change_pitch_mode(struct nco oscillator[static 1]){
    return (oscillator->mode == free) ? v_per_octave : free;
}

static void bit_bang_encoder(struct encoder enc[static 1]){
    /*
     * determine the irection that the encoder was turned.
     * you already know that channel a is falling so only the second channel is
     * needed for the direction change (clockwise and counter-c-w).
     */
    enc->direction = (enc->B.value == 1) ? cw : ccw;
    switch (enc->direction) {
        case cw:
            ++enc->increment;
            break;
        case ccw:
            --enc->increment;
            break;
    }

    /*
     * constrain the encoder increment value to a reasonable range i.e. cyrcles.
     */
    register cyrcles_e absurd_dante_floor = hell + 0xf;
    if (enc->increment > hell && enc->increment < absurd_dante_floor)
        enc->increment = hell;
    // todo dont be so stupid the next time!!
    else if (enc->increment > absurd_dante_floor)
        enc->increment = entrance;
}


void scan_and_apply_current_modulations(struct encoder enc[static 1],
                                        struct nco osillator[static 1]){
    if (enc->A.flag == 0x69 && osillator->phase_pending_update){

        if (osillator->distortion.on)
            bit_bang_encoder(enc);
        else
            osillator->mode = change_pitch_mode(osillator);

        switch (osillator->distortion.past_dante) {
            case hell:
                osillator->distortion.dante = enc->increment = ninth;
                break;
            default:
                osillator->distortion.dante = enc->increment;
                break;
        }
        enc->A.flag = 'D';
        osillator->distortion.past_dante = osillator->distortion.dante;
    }
}
