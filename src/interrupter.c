#include "interrupter.h"
#include "sysclk.c"
const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn, sine_high_speed64};
volatile const uint16_t *wave_me_d = sine_wave;
volatile const uint16_t *wave_me_d2 = sine_wave;


void main() {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    struct button *wave_buttons[3] = {&wave_choise_dac1, &wave_choise_dac2, &distortion_choice};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct gpio *adcs[2] = {&pitch_0_cv, &pitch_1_cv};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    struct dma *dma_chans[2] = {&dac_1_dma, &dac_2_dma};
    sys_clock_config();
    gpio_init(wave_buttons, dacs, adcs[0]);

#if defined(DEBUG) || defined(DEBUGDAC)
    debug_tim2_pin31();
#else
#endif
    for (int i=0; i < 2; i++) {
        dma_config(dma_chans[i]);
        tim_init(master_clock, timers[i]);
    }

    tim_init(master_clock, &tim2_settings);
    adc_init_settings(&pitch0cv_in);

    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }
    while (1) {
        if (l_osc.phase_pending_update) {
            stage_pending_inc(prev_value, &l_osc, master_clock);
            stage_pending_inc(prev_value_1, &r_osc, master_clock);
            phase_done_update = true;
        }
        if (wave_choise_dac1.flag == 0x69) {
            wave_me_d = waves_bank[wave_choise_dac1.state];
        }
        if (wave_choise_dac2.flag == 0x69) {
            // wave_me_d2 = waves_bank[wave_choise_dac2.state];
        }
        if (distortion_choice.flag == 0x69) {
            if (!l_osc.distortion.on){
                l_osc.distortion.on = true;
                ++l_osc.distortion.dante;
                if(l_osc.distortion.dante > ninth){
                    l_osc.distortion.dante = first;
                }
            }
            else
                l_osc.distortion.on = false;
            distortion_choice.flag = 'D';
        }
    }
}
