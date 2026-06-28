#include "interrupter.h"
#include "sysclk.c"

void main(void) {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    volatile struct button *nco_buttons[3] = {&freq_mode_but_dac1, &osc_0_mode_choice, &distortion_choice};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct gpio *osc_cvs[5] = {&pitch_0_cv, &dist_amount_0_cv, &tunner_adc_in, &pitch_1_cv, &dist_amount_1_cv};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};

    sys_clock_config();

    uint16_t c, d, a, why;
    c = sizeof nco_buttons / sizeof &nco_buttons[0];
    d = sizeof dacs / sizeof &dacs[0];
    a = sizeof osc_cvs / sizeof &osc_cvs[0];
    why = 0;
    gpio_init(nco_buttons, dacs, osc_cvs, c, d, a, why);

    for(uint8_t u=0; u<2; ++u){
        set_flip_switch(&octave_switch, u);
        set_flip_switch(&switch2_dev_rev0, u);
    }

    enc_init(&osc_0_pd_enc);
    enc_init(&osc_1_pd_enc);
    config_display(&display);

    dma_config(&dac_dma);

    tim_init(cosmos._data->dac1_clock, timers[0]);
    tim_init(1, timers[1]);                      /* ~~Hz*/
    tim_init(10, &tim3_settings);

    tim_init(cosmos._data->adc1_clock, &tim2_settings);
    adc_init_settings(&adc_settings);


    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }

    //
    // like this we dont erase our selfs by default... that's handy...
    // if(read_gpio(&distortion_choice.pin))
    //     perform_erase(&flash_settings);

    while (1) {
        if(l_osc.tempered.flag)
            l_osc.tempered.first_fundamental = map_uint(cosmos._data->tunner_pitch_raw_d, &cosmos.oscillators[0]->tempered.tuner_bounds);

        if(l_osc.tempered.oct.change && debounce(&octave_switch.pins[0], octave_switch._state[0])){
            switch (l_osc.tempered.oct.span) {
                case 1:
                case 2:
                case 3:
                case 4:
                    ++l_osc.tempered.oct.span;
                    break;
                case 5:
                    l_osc.tempered.oct.span = 1;
                    break;
            }
        }

        tune(&cosmos, 0, &display);
        handle_display(&display,
                       cosmos.oscillators[0]->distortion.amount,
                       osc_0_pd_enc.virtual_wave_button.state,
                       0);

        tune(&cosmos, 1, &display);
    }
}
