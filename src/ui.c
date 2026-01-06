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

static inline uint8_t decode_quad_enc(struct encoder *enc){
    return (enc->B.value[0] << 1) | enc->B.value[1];
}

static inline enum direction extract_encoder_direction(struct encoder enc[static 1]){
    return (decode_quad_enc(enc) == 1) ? cw : ccw;
}

static void handle_distortion(struct nco *o, volatile enum direction dir){
    const cyrcles_e absurd_dante_floor = hell + 0xe;
    switch (dir) {
    case cw:
        ++o->distortion.dante;
        break;
    case ccw:
        --o->distortion.dante;
        break;
    }

    if (o->distortion.dante > hell && o->distortion.dante < absurd_dante_floor) {
        o->distortion.dante = hell;
    } else if (o->distortion.dante > absurd_dante_floor) {
        o->distortion.dante = entrance;
    }
    o->distortion.past_dante = o->distortion.dante;
}

static void handle_wave_selection(struct encoder *e, volatile enum direction dir){
    switch (dir) {
    case cw:
        ++e->virtual_wave_button.state;
        break;
    case ccw:
        --e->virtual_wave_button.state;
        break;
    }
    e->virtual_wave_button.state &= (WAVE_CTR - 1);
    e->virtual_wave_button.flag = 'i';
}

volatile void* apply_modulations_callback(struct encoder enc[static 1],
                                          struct nco o[static 1]){
    if (o->distortion.on) {
        handle_distortion(o, extract_encoder_direction(enc));
    } else {
        handle_wave_selection(enc, extract_encoder_direction(enc));
    }
    o->phase.pending_update = true;
    return (void*)0;
}

void align_phase(volatile struct nco *o[2]){
    o[1]->phase.accum = o[0]->phase.accum;
}

void sync_1_to_0(volatile struct nco *o0, volatile struct nco *o1){
    o1->phase.inc = o0->phase.inc;
}
