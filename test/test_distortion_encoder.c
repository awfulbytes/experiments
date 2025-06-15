#include <stdio.h>
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#include <assert.h>
#include "ui.h"
#include "nco.h"

extern void scan_and_apply_current_modulations(struct encoder enc[static 1], struct nco osillator[static 1]);
struct nco l_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=true, .phase_pending_update_inc=0,
                    .mode=free,
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
                               .value=0, .flag='D',
                               .it_settings = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_4,
                                            .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                            .exti_line_conf=LL_EXTI_CONFIG_LINE4}},
                         .B.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_5, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                         .B.value = 0,
                         .B.it_settings = { },
                         .B.flag = 'D',
                         .increment=0, .direction=false};

void emulate_encoder_cw(struct encoder *distortion_encoder){
    distortion_encoder->A.flag = 0x69;
    distortion_encoder->B.value = 1U;
}

void emulate_encoder_ccw(struct encoder *distortion_encoder){
    distortion_encoder->A.flag = 0x69;
    distortion_encoder->B.value = 0U;
}

void emulate_distortion_enable_button(struct button *abut){
    if (!l_osc.distortion.on) {
        l_osc.distortion.on = true;
        abut->flag = 'i';
    }
    else {
        l_osc.distortion.on = false;
        abut->flag = 'D';
    }
}

void test_cw_encoder(void){
    emulate_encoder_cw(&osc_0_pd_enc);
    scan_and_apply_current_modulations(&osc_0_pd_enc, &l_osc);

    assert(osc_0_pd_enc.A.flag == 'D');
    assert(osc_0_pd_enc.direction == true);
    assert(osc_0_pd_enc.increment == 1);
    assert(l_osc.distortion.past_dante == first);
}

void test_ccw_encoder(void){
    emulate_encoder_ccw(&osc_0_pd_enc);
    scan_and_apply_current_modulations(&osc_0_pd_enc, &l_osc);

    assert(osc_0_pd_enc.direction == false);
    assert(osc_0_pd_enc.increment == 0);
    assert(l_osc.distortion.past_dante == entrance);
}


int main(void){
    emulate_distortion_enable_button(&distortion_choice);
    test_cw_encoder();
    test_ccw_encoder();
}
