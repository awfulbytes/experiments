#include <stdio.h>
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#include <assert.h>
#include "ui.h"
#include "nco.h"

extern volatile void* apply_modulations_callback(struct encoder enc[static 1], struct nco osillator[static 1]);
struct nco l_osc = {.phase = {.accum = 0, .inc = 0x01000000, .pending_update_inc=0, .pending_update=true,},
                    .mode=high,
                    .distortion.amount=64,
                    .distortion.on=false,
                    .distortion.distortion_value=0,
                    .distortion.dante=entrance,
                    .distortion.past_dante=entrance,};

struct button distortion_choice = {.state=0, .flag='D',
                                .exti={.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_12, .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                   .exti_line_conf=LL_EXTI_CONFIG_LINE12},
                                .pin={.pin_id=LL_GPIO_PIN_12, .port_id=GPIOC, .mode=LL_GPIO_MODE_INPUT, .pull=LL_GPIO_PULL_DOWN}};

struct encoder osc_0_pd_enc = {.A = {.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_4, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                               .value={0}, .flag='D',
                               .it_settings = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_4,
                                            .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                            .exti_line_conf=LL_EXTI_CONFIG_LINE4}},
                         .B.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_5, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                         .B.value = {0},
                         .B.it_settings = { },
                         .B.flag = 'D',
                         .virtual_wave_button.state = SINE,
                         /* .increment=0, */ .direction=cw};

void emulate_encoder_cw(struct encoder *distortion_encoder){
    distortion_encoder->A.flag = 0x69;
    distortion_encoder->B.value[0] = 0;
    distortion_encoder->B.value[1] = 1;
}

void emulate_encoder_ccw(struct encoder *distortion_encoder){
    distortion_encoder->A.flag = 0x69;
    distortion_encoder->B.value[0] = 1;
    distortion_encoder->B.value[1] = 0;
}

void test_cw_encoder(void){
    
    emulate_encoder_cw(&osc_0_pd_enc);
    apply_modulations_callback(&osc_0_pd_enc, &l_osc);

    if (!(osc_0_pd_enc.direction == cw)) {
        fprintf(stderr,  "FAIL: Clockwise direction should be true!!\n");
        assert(0);
    }
}

void test_ccw_encoder(void){
    emulate_encoder_ccw(&osc_0_pd_enc);
    apply_modulations_callback(&osc_0_pd_enc, &l_osc);
}


int main(void){
    test_cw_encoder();
    assert(osc_0_pd_enc.A.flag == 'i');
    assert(osc_0_pd_enc.direction == cw);
    assert(osc_0_pd_enc.virtual_wave_button.state == SAWU);

    test_ccw_encoder();
    assert(osc_0_pd_enc.A.flag == 'i');
    /* note: i modify a local copy and do my work with that.
     *       no need to modify the original memory cell also! */
    assert(osc_0_pd_enc.direction == cw);
    assert(osc_0_pd_enc.virtual_wave_button.state == SINE);
}
