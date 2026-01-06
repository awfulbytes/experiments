#include "nco.h"
#include "nco_general_stuff.h"
#include "overseer.h"
#include <stdio.h>
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#include "ui.h"
#include <assert.h>

#define dac_clk       192000
#define adc_clk        44000
uint32_t double_buffer[256] = {0};

extern volatile void* apply_modulations_callback(struct encoder enc[static 1],
                                                 struct nco o[static 1]);
extern inline void stage_modulated_values(volatile struct nco osc[static 1],
                                                 volatile struct   overworld *data);

void emulate_distortion_on(struct nco *nco){
    nco->distortion.on = true;
    nco->phase.pending_update = true;
}

void emulate_distortion_change(struct encoder *enc){
    enc->A.flag = 'i';
    enc->B.value[0] = 0;
    enc->B.value[1] = 1;
}

void emulate_dac_interrupt(){

    generate_half_signal(sine_wave, 128, &l_osc);
    update_data_buff((uint32_t*)l_osc.data_buff, double_buffer, 128);
    update_data_buff((uint32_t*)l_osc.data_buff, double_buffer + 128, 128);
}


int main(){
    struct   encoder dummy_enc;
    uint64_t         initialized_incremet_value = l_osc.phase.inc;
    struct overworld data = {
        .pitch_cv                                      = 0xfff,
        .current_value_cv_0_pitch                      = 0x000,
        .osc_0_cv_distortion_amount                    = 0xfff,
        .dac1_clock                                    = dac_clk,
    };
    struct overseer seer = { .oscillators[0]=&l_osc, .universe_data = &data };
    /* dummy_enc.increment = entrance; */
    l_osc.distortion.distortion_value = entrance;
    for ( ;l_osc.distortion.distortion_value < hell; ) {
        printf("hell-level:\t%d\n", l_osc.distortion.distortion_value);
        emulate_distortion_on(&l_osc);
        emulate_distortion_change(&dummy_enc);

        apply_modulations_callback(&dummy_enc, &l_osc);
        /* emulate_dac_interrupt(); */

        assert(l_osc.mode != high);
        assert(initialized_incremet_value != l_osc.phase.pending_update_inc);

        tune(&seer, 0);
        assert(l_osc.phase.done_update != l_osc.phase.pending_update);
        emulate_dac_interrupt();

        printf("%b\n", l_osc.phase.done_update);
        assert(l_osc.mode != high);
        assert(initialized_incremet_value > l_osc.phase.pending_update_inc);
        assert(l_osc.phase.done_update    == false);
        assert(l_osc.phase.pending_update == false);
        ++l_osc.distortion.distortion_value;
    }
}
