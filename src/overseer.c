#include "overseer.h"
#include "nco.h"

void tune(struct overseer *overseer,
          uint8_t          osc_idx){
    overseer->selected = overseer->oscillators[osc_idx];
    register uint16_t pitch_raw_digital =
        (overseer->selected->in_the_house.report == 0)
        ? overseer->universe_data->current_value_cv_0_pitch
        : overseer->universe_data->current_value_cv_1_pitch;

    struct bandwidth band = overseer->selected->bandwidth;
    overseer->universe_data->pitch_cv =
        (overseer->selected->mode == high)
        ? map_uint(pitch_raw_digital, &band.high)
        : map_uint(pitch_raw_digital, &band.low);

    stage_modulated_values(overseer->selected, overseer->universe_data);
    /* return note; */
}

#pragma GCC diagnostic ignored "-Wunused-variable"
static inline void stage_modulated_values(volatile struct nco osc[static 1],
                                          volatile struct   overworld *data){
    if(osc->phase.pending_update){
        if (osc->distortion.on) {
            /* check linked tasks */
            uint16_t tmp =
                (osc->in_the_house.report == 0)
                ? data->osc_0_cv_distortion_amount
                : data->osc_1_cv_distortion_ammount;

            osc->distortion.amount = map_uint(tmp, &osc->distortion.level_range);
        } else {
            /* who tf knows */
        }
        osc->phase.done_update = stage_pending_inc(data->pitch_cv, osc, data->dac1_clock);
        osc->phase.pending_update = !osc->phase.done_update;
    }
}
