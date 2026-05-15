#include "ui.h"
#include <stdint.h>

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

volatile void* apply_modulations_callback(struct encoder enc[static 1]){
    handle_wave_selection(enc, extract_encoder_direction(enc));
    return (void*)0;
}

void set_flip_switch(struct flip_switch *f, uint8_t pin_idx){
    set_gpio(f->pins + pin_idx);
    exti_flip_setup(f, pin_idx);
}

static void config_led(struct led *l){
    set_gpio(&l->pin);
    l->state = false;
}

void toggle_blink_state(struct led *l){
    l->state = !(l->state);
}

void config_display(struct display *d){
    uint8_t led_am = 6;
    for(uint8_t a = 0; a < led_am; ++a){
        config_led(&d->leds[a]);
    }
}

static bool extract_bit(uint8_t number, uint8_t bit){
    return ((number & (1U<<bit)) != 0);
}

#define discard_upper_3b_of_8b     (~0xfe0U)
#define span_in_binary(z)          ((~(0xffU<<z)) & (0x1fU))
void octave_recorder(struct display *d, uint8_t span_amount, uint8_t osc){
    d->shift_registers[osc] = (
        (uint16_t)(span_in_binary(span_amount) << (d->octave_shifts[osc] - d->backwards_jump[osc]))) & discard_upper_3b_of_8b;
}

void handle_display(struct display *d, uint8_t distortion_level, uint8_t current_wave, uint8_t osc){
    for(uint8_t k=0; k < 6; ++k){
        d->leds[k].state = false;
    }
    switch(d->view){
        case tuning:
            if(d->tuner_view == recording){
                d->leds[5].state = true;
                d->octave_shifts[0] = d->backwards_jump[0] = 0;
            }
            for(uint8_t j=0; j<5; ++j){
                d->leds[j].state = extract_bit(d->shift_registers[osc], j);
            }
            break;

        case dist:
            if(distortion_level > d->distortion.twenty_percent)
                d->distortion.change = U8DIVBY25(distortion_level);

            for(uint8_t k=0; k < d->distortion.change; ++k){
                d->leds[k].state = true;
            }
            break;

        case wave:
            d->leds[current_wave].state = true;
            break;

        case diatonic:
            d->leds[4].state = true;
            break;
    }
    for(uint8_t k=0; k < 6; ++k){
        write_gpio(&d->leds[k].pin, d->leds[k].state);
    }
}
