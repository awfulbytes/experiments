#include "ui.h"

static void exti_enc_setup(struct encoder_channel channel[static 1]);

inline void button_callback(struct button *abut){
    // nxt:: need some more work but elswhere..
    switch (abut->state) {
    case 3:
        abut->state = 0;
        break;
    default:
        ++abut->state;
        break;
    }
    abut->flag = 'i';
    return;
}

void enc_init(struct encoder *enc){
    LL_GPIO_SetPinMode(enc->A.pin.port_id, enc->A.pin.pin_id, enc->A.pin.mode);
    LL_GPIO_SetPinMode(enc->B.pin.port_id, enc->B.pin.pin_id, enc->B.pin.mode);

    LL_GPIO_SetPinPull(enc->A.pin.port_id, enc->A.pin.pin_id, enc->A.pin.pull);
    LL_GPIO_SetPinPull(enc->B.pin.port_id, enc->B.pin.pin_id, enc->B.pin.pull);

    exti_enc_setup(&enc->A);
}

static inline void decode_quad_enc(struct encoder *enc){
    enc->B.val = (enc->B.value[0] << 1) | enc->B.value[1];
}

static void bit_bang_encoder(struct encoder enc[static 1]){

    decode_quad_enc(enc);
    enc->direction = (enc->B.val == 1) ? cw : ccw;
    switch (enc->direction) {
    case cw:
        ++enc->increment;
        break;
    case ccw:
        --enc->increment;
        break;
    }
}


volatile void* apply_modulations_callback(struct encoder enc[static 1],
                                          struct nco osillator[static 1]){
    register cyrcles_e absurd_dante_floor = hell + 0xe;

    bit_bang_encoder(enc);
    if (osillator->distortion.on) {
        osillator->distortion.dante = enc->increment;
        if (osillator->distortion.dante > hell && osillator->distortion.dante < absurd_dante_floor){
            osillator->distortion.dante = hell;
        }
        else if (osillator->distortion.dante > absurd_dante_floor){
            osillator->distortion.dante = entrance;
        }
        osillator->distortion.past_dante = osillator->distortion.dante;
    } else {
        enc->virtual_wave_button.state = enc->increment & (WAVE_CTR - 1);
        enc->virtual_wave_button.flag = 'i';
    }
    osillator->phase.pending_update = true;
    return (void*)0;
}
