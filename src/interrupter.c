#include "interrupter.h"
#include "stm32g0xx_ll_utils.h"
#include "sysclk.c"
const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn, pulse};
volatile const uint16_t *wave_me_d = sine_wave;
volatile const uint16_t *wave_me_d2 = sine_wave;


void main() {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    struct button *wave_buttons[3] = {&wave_choise_dac1, &wave_choise_dac2, &distortion_choice};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct gpio *adcs[2] = {&pitch_0_cv, &dist_amount_0_cv};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    struct dma *dma_chans[2] = {&dac_1_dma, &dac_2_dma};
    sys_clock_config();
    gpio_init(wave_buttons, dacs, adcs);

    enc_init(&pd_enc);

#if defined(DEBUG) || defined(DEBUGDAC)
    debug_tim2_pin31();
#else
#endif
    for (int i=0; i < 2; i++) {
        dma_config(dma_chans[i]);
        tim_init(master_clock, timers[i]);
    }

    tim_init(44000, &tim2_settings);
    adc_init_settings(&adc_settings);


    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }

    do {
        if (pd_enc.A.flag == 0x69 && l_osc.phase_pending_update) {
            if (l_osc.distortion.on){
                increment_encoder(&pd_enc);
                constrain_encoder_to_distortion_level(&pd_enc);
            }
            else
                l_osc.mode = (l_osc.mode == free) ? v_per_octave : free;

            switch (l_osc.distortion.past_dante) {
                case hell:
                    l_osc.distortion.dante = pd_enc.increment = ninteenth;
                    break;
                default:
                    l_osc.distortion.dante = pd_enc.increment;
                    break;
            }
            pd_enc.A.flag = 'D';
            l_osc.distortion.past_dante = l_osc.distortion.dante;
        }

        if (l_osc.phase_pending_update) {
            // && !l_osc.phase_done_update
            l_osc.distortion.amount = map_12b_to_distortion_amount(prev_value_cv_distortion_amount);
            bool staged = false;
            staged = stage_pending_inc(prev_value_cv_0_pitch, &l_osc, master_clock);
            l_osc.phase_done_update = staged;
            l_osc.phase_pending_update = !staged;
        }
        if (r_osc.phase_pending_update) {
            bool staged = false;
            // todo::: make this separate from distortion amount
            staged = stage_pending_inc(prev_value_cv_distortion_amount, &r_osc, master_clock);
            r_osc.phase_done_update = staged;
            r_osc.phase_pending_update = !staged;
        }
        if (wave_choise_dac1.flag == 0x69) {
            wave_me_d = waves_bank[wave_choise_dac1.state];
            wave_choise_dac1.flag = 'D';
        }
        if (wave_choise_dac2.flag == 0x69) {
            wave_me_d2 = waves_bank[wave_choise_dac2.state];
            wave_choise_dac2.flag = 'D';
        }
    } while (1);
}
