#include "interrupter.h"
#include "sysclk.c"
volatile const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn};
volatile const uint16_t *wave_me_d = sine_wave;
volatile const uint16_t *wave_me_d2 = sine_wave;


void main() {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    struct button *wave_buttons[2] = {&wave_choise_dac1, &wave_choise_dac2};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    struct dma *dma_chans[2] = {&dac_1_dma, &dac_2_dma};
    sys_clock_config();
    gpio_init(wave_buttons, dacs, &pitch_0_cv);

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
            atomic_ushort note = map_12b_to_hz(prev_value);
            atomic_uint_fast32_t tmp = ((note * (1<<16)) / master_clock);
            l_osc.phase_pending_update_inc = tmp << 16;
            l_osc.phase_pending_update = false;
            phase_done_update = true;
        }
        if (wave_choise_dac1.flag == 0x69) {
            wave_me_d = waves_bank[wave_choise_dac1.state];
        }
        if (wave_choise_dac2.flag == 0x69) {
            wave_me_d2 = waves_bank[wave_choise_dac2.state];
        }
    }
}
