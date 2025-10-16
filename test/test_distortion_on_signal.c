#include "nco_general_stuff.h"
#include <stdio.h>
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#include "ui.h"
#include <assert.h>

uint16_t double_buffer[256] = {0};

extern void scan_and_apply_current_modulations(struct encoder enc[static 1],
                                               struct nco osillator[static 1]);

void emulate_distortion_on(struct nco *nco){
    nco->distortion.on = true;
    nco->phase_pending_update = true;
}

void emulate_distortion_change(struct encoder *enc){
    enc->A.flag = 'i';
    enc->B.value = 1U;
    /* enc->increment = ninth; */
}

void emulate_dac_interrupt(){

    generate_half_signal(sine_wave, 128, &l_osc);
    update_data_buff(l_osc.data_buff.ping_buff, double_buffer, 128);
    update_data_buff(l_osc.data_buff.ping_buff, double_buffer + 128, 128);
}


int main(){
    struct   encoder dummy_enc;
    uint64_t         initialized_incremet_value = l_osc.phase_inc;
    uint16_t         adc_raw_cv_pitch           = 0x000;
    uint16_t         adc_raw_cv_dist            = 0xfff;

    for ( ;dummy_enc.increment < hell; ) {
        printf("iteration-number:\t%d\n", dummy_enc.increment);
        emulate_distortion_on(&l_osc);
        emulate_distortion_change(&dummy_enc);

        scan_and_apply_current_modulations(&dummy_enc, &l_osc);
        // emulate_dac_interrupt();

        assert(l_osc.mode != free);
        assert(initialized_incremet_value != l_osc.phase_pending_update_inc);
        /* assert(l_osc.phase_done_update == 1); */

        stage_modulated_signal_values(&l_osc,
                                      adc_raw_cv_dist,
                                      adc_raw_cv_pitch,
                                      master_clock);
        assert(l_osc.phase_done_update != l_osc.phase_pending_update);
        emulate_dac_interrupt();

        assert(l_osc.mode != free);
        /* assert(initialized_incremet_value > l_osc.phase_pending_update_inc); */
        assert(l_osc.phase_done_update != l_osc.phase_pending_update);
        assert(l_osc.phase_done_update == true);
        /* ++dummy_enc.increment; */
        adc_raw_cv_dist += dummy_enc.increment;
        adc_raw_cv_pitch -= 200;
    }
}
