#include "interrupter.h"
#include "dbg.h"
#include "sysclk.c"

/*
#define hell_jump
#define walk
*/
#ifdef walk
volatile uint32_t hell_walk_counter = 0;
volatile int go_front = 0;
#endif // walk

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
    /*
     * todo:: ^ that shit
     * this is not a debug any-more...
     * used as the encoder leds which are removed as I use the magnetic encoder
     */
    debug_tim2_pin31();
#else
#endif
    for (int i=0; i < 2; i++) {
        dma_config(dma_chans[i]);
        tim_init(cosmos.universe_data->dac1_clock, timers[i]);
    }

    tim_init(cosmos.universe_data->adc1_clock, &tim2_settings);
    adc_init_settings(&adc_settings);


    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }

#ifdef hell_jump
    #define floor_on_hell             fifth
    land_on_hell_floor()
#endif

    while (1) {

#ifdef  walk
      hell_walking()
#endif

        for (uint8_t i=0; i < 1; ++i) {
            scan_and_apply_current_modulations(&osc_0_pd_enc, cosmos.oscillators[i]);
            stage_modulated_signal_values(cosmos.oscillators[i], cosmos.universe_data);
        }
        /*
         * hook up independent encoder for cosmos.osclillator[2]
         */

        if (wave_choise_dac1.flag == 0x69) {
            wave_me_d = *(waves_bank + wave_choise_dac1.state);
            wave_choise_dac1.flag = 'D';
        }
        if (wave_choise_dac2.flag == 0x69) {
            wave_me_d2 = *(waves_bank + wave_choise_dac2.state);
            // wave_me_d2 = waves_bank[wave_choise_dac2.state];
            wave_choise_dac2.flag = 'D';
        }
    }
}
