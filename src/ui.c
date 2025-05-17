#include "ui.h"
#include "stm32g071xx.h"

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
        case 2:
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

// void decode_enc(struct encoder *enc){
//     enc->A.value = (enc->A.pin.port_id->IDR) & (GPIO_IDR_ID4);
//     enc->B.value = ((enc->B.pin.port_id->IDR) & (1<<5)) << 1;
// }
