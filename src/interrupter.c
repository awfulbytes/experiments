#include "interrupter.h"
#include "sysclk.c"

void main(void) {
    struct timer *timers[1] = {&tim6_settings};
    volatile struct button *nco_buttons[3] = {&freq_mode_but_dac1, &freq_mode_but_dac2, &distortion_choice};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct gpio *osc_cvs[5] = {&pitch_0_cv, &dist_amount_0_cv, &tunner_adc_in, &pitch_1_cv, &dist_amount_1_cv};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    /* struct dma *dma_chans[1] = {&dac_dma}; */
    sys_clock_config();
    uint16_t c, d, a, why;
    c = sizeof nco_buttons / sizeof &nco_buttons[0];
    d = sizeof dacs / sizeof &dacs[0];
    a = sizeof osc_cvs / sizeof &osc_cvs[0];
    why = 0;
    gpio_init(nco_buttons, dacs, osc_cvs, c, d, a, why);

    for(uint8_t u=0; u<2; ++u){
        /* this needs more work to assign pins */
        set_flip_switch(&octave_switch, u);
    }

    enc_init(&osc_0_pd_enc);
    enc_init(&osc_1_pd_enc);
    config_display(&display);

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

    /* todo: test (timer 7 is not used by the dac any more and this should be ok */
    tim_init(cosmos._data->dac1_clock, timers[0]);
    tim_init(cosmos._data->adc1_clock, &tim2_settings);
    adc_init_settings(&adc_settings);


    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }

////////////////////////////////////////////////////////////////////////////////
// fucking really????? POR you mother fucker....
////////////////////////////////////////////////////////////////////////////////
    // l_osc.curr_wave_ptr = sine_wave;
    // r_osc.curr_wave_ptr = sine_wave;

    //
    // like this we dont erase our selfs by default... that's handy...
    // if(read_gpio(&distortion_choice.pin))
    //     perform_erase(&flash_settings);

    while (1) {

        if(osc_0_pd_enc.virtual_wave_button.flag == 0x69){
            l_osc.curr_wave_ptr = waves_bank[osc_0_pd_enc.virtual_wave_button.state];
            osc_0_pd_enc.virtual_wave_button.flag = 'D';
        }

        if(l_osc.tempered.rec & read_gpio(&octave_switch.pins[1])){
            l_osc.tempered.flag = 0x1;
            display.tuner_view[0] = recording;
            l_osc.tempered.first_fundamental = map_uint(cosmos._data->tunner_pitch_raw_d, &cosmos.oscillators[0]->tempered.tuner_bounds);

        }else if(l_osc.tempered.rec && !read_gpio(&octave_switch.pins[1])){
            l_osc.tempered.flag = 0x0;
            display.tuner_view[0] = playing;
            l_osc.tempered.rec = false;
        }

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
            l_osc.tempered.oct.change = false;
        }
        if(button_press(&freq_mode_but_dac2) && freq_mode_but_dac2.flag == 0x69){
            switch (l_osc.tempered.type) {
                case none:
                    l_osc.mode = tracking;
                    l_osc.tempered.type = diatonic_major_g;
                    break;
                case diatonic_major_g:
                    l_osc.mode = free;
                    l_osc.tempered.type = eq_tempered;
                    break;
                case eq_tempered:
                    l_osc.mode = free;
                    l_osc.tempered.type = none;
                    break;
            }
            freq_mode_but_dac2.flag = 'D';
        }
        tune(&cosmos, 0, &display);
        handle_display(&display,
                       cosmos.oscillators[0]->distortion.amount,
                       osc_0_pd_enc.virtual_wave_button.state,
                       0);

        if (osc_1_pd_enc.virtual_wave_button.flag == 0x69) {
            r_osc.curr_wave_ptr = waves_bank[osc_1_pd_enc.virtual_wave_button.state];
            osc_1_pd_enc.virtual_wave_button.flag = 'D';
        }
        tune(&cosmos, 1, &display);
    }
}
