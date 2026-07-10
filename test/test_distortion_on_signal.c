#pragma GCC diagnostic ignored "-Woverflow"
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

#define TEST
// #define wtf

#include "nco.h"
#include "nco_general_stuff.h"
#include "overseer.h"
#include <stdio.h>
#include "ui.h"
#include <assert.h>

struct display display = {0};

#define dac_clk       144000
#define adc_clk        44000
uint32_t double_buffer[256] = {0};

void emulate_distortion_on(struct nco *nco){
    nco->distortion.on = true;
    nco->phase.pending_update = true;
}

void emulate_dac_interrupt(){
    l_osc.phase.inc = l_osc.phase.pending_update_inc;
    generate_half_signal(128, &l_osc);
    uint32_t merged_dual_buffer[128] = {0};
    for(uint8_t u=0; u < 128; ++u){
        merged_dual_buffer[u] =
            l_osc.data_buff[u] << 16U | l_osc.data_buff[u];
    }
    update_data_buff(merged_dual_buffer, double_buffer, 128);
    update_data_buff(merged_dual_buffer, double_buffer + 128, 128);
}


int main(){
    struct overworld data = {
        .pitch_cv                                      = 0xfff,
        ._cv_0_pitch                                   = 0x000,
        .osc_0_cv_distortion_amount                    = 0xfff,
        .dac1_clock                                    = dac_clk,
    };
    struct overseer seer = { .oscillators[0]=&l_osc, ._data = &data };
    uint32_t accum = 0;
    uint32_t indexed = 0x0; /* controlles the pitch attribute of overworld */

    for( ;; ){
        if(indexed == 0x7fff)
            break;

        emulate_distortion_on(&l_osc);
        assert(l_osc.distortion.on == true);

        data._cv_0_pitch = indexed;

        tune(&seer, 0, &display);
        assert(l_osc.phase.done_update == true);
        assert(l_osc.phase.pending_update == false);

        if(l_osc.phase.inc == l_osc.phase.pending_update_inc){
            accum += 1;
#ifdef wtf
            printf("\npending and applyed increments are the same!\n\n");
            printf("distortion:\t\t%x \naccumulator:\t%x\n\n", l_osc.distortion.distortion_value, l_osc.phase.accum);
#endif
        }
        emulate_dac_interrupt();
        assert(l_osc.phase.done_update == false);

        ++indexed;
    }
    printf("how many times we got equal increments:\t%d/%d", accum, indexed);
}
