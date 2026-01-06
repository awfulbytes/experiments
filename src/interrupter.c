#include "interrupter.h"
#include "dbg.h"
#include "overseer.h"
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
    volatile struct button *nco_buttons[3] = {&freq_mode_but_dac1, &freq_mode_but_dac2, &distortion_choice};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct gpio *osc_cvs[5] = {&pitch_0_cv, &dist_amount_0_cv, &dist_amount_0_2_cv, &pitch_1_cv, &dist_amount_1_cv};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    /* struct dma *dma_chans[1] = {&dac_dma}; */
    sys_clock_config();
    uint16_t c, d, a, why;
    c = sizeof nco_buttons / sizeof &nco_buttons[0];
    d = sizeof dacs / sizeof &dacs[0];
    a = sizeof osc_cvs / sizeof &osc_cvs[0];
    why = 0;
    gpio_init(nco_buttons, dacs, osc_cvs, c, d, a, why);

    enc_init(&osc_0_pd_enc);
    enc_init(&osc_1_pd_enc);

#if defined(DEBUG) || defined(DEBUGDAC)
    /*
     * todo:: ^ that shit
     * this is not a debug any-more...
     * used as the encoder leds which are removed as I use the magnetic encoder
     */
    debug_tim2_pin31();
#else
#endif
    dma_config(&dac_dma);
    for (int i=0; i < 2; i++) {
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

        if (osc_0_pd_enc.virtual_wave_button.flag == 0x69) {
            l_osc.curr_wave_ptr = waves_bank[osc_0_pd_enc.virtual_wave_button.state];
            osc_0_pd_enc.virtual_wave_button.flag = 'D';
        }
        tune(&cosmos, 0);

        if (osc_1_pd_enc.virtual_wave_button.flag == 0x69) {
            r_osc.curr_wave_ptr = waves_bank[osc_1_pd_enc.virtual_wave_button.state];
            osc_1_pd_enc.virtual_wave_button.flag = 'D';
        }
        tune(&cosmos, 1);
    }
}
