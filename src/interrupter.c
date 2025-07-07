#include "interrupter.h"
#include "sysclk.c"


void main() {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    struct button *wave_buttons[3] = {&wave_choise_dac1, &wave_choise_dac2, &distortion_choice};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct gpio *osc_cvs[3] = {&pitch_0_cv, &dist_amount_0_cv, &pitch_1_cv};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    struct dma *dma_chans[2] = {&dac_1_dma, &dac_2_dma};
    sys_clock_config();
    gpio_init(wave_buttons, dacs, osc_cvs);

    enc_init(&osc_0_pd_enc);

#if defined(DEBUG) || defined(DEBUGDAC)
    debug_tim2_pin31();
#else
#endif
    for (int i=0; i < 2; i++) {
        dma_config(dma_chans[i]);
        tim_init(dac1_clock, timers[i]);
    }

    tim_init(adc1_clock, &tim2_settings);
    adc_init_settings(&adc_settings);


    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }

    do {
        scan_and_apply_current_modulations(&osc_0_pd_enc, &l_osc);
        stage_modulated_signal_values(&l_osc, prev_value_cv_distortion_amount, prev_value_cv_0_pitch, dac1_clock);

        // todo::: hook up independent encoder
        //         nxt make more controlled loop...
        // todo:::
        // (nxt) we seem to fuck the values when used twice for the second
        //       oscillator. This seems to lag the whole program and skipping
        //       some of the first oscillator selection... from a step and beyond
        //       there is no hope!
        // make @osc_1_pd_enc do the thing lets say.
        // scan_and_apply_current_modulations(&osc_0_pd_enc, &r_osc);
        // stage_modulated_signal_values(&r_osc, 0, prev_value_cv_1_pitch, dac1_clock);

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
