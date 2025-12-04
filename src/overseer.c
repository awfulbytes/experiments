#include "overseer.h"
#include "nco.h"

struct nco* select_osc(struct overseer *overseer, uint8_t osc_idx){
    struct nco *select = overseer->oscillators[osc_idx];
    select->in_the_house.report = osc_idx;
    return select;
}

uint16_t tune_to_bandwidth(struct overseer *overseer, uint8_t osc_idx) {
    register uint16_t raw_val =
        (osc_idx == 0)
        ? overseer->universe_data->current_value_cv_0_pitch
        : overseer->universe_data->current_value_cv_1_pitch;
    struct nco *selected_oscillator = select_osc(overseer, osc_idx);
    uint16_t note =
        (selected_oscillator->mode == free) ?
        map_uint(raw_val, &selected_oscillator->bandwidth.free) :
        map_uint(raw_val, &selected_oscillator->bandwidth.tracking);
    return note;
}

#pragma GCC diagnostic ignored "-Wunused-variable"
void stage_modulated_signal_values(struct   nco      osc[static 1],
                                   register uint16_t note,
                                   struct overworld *data){
    if(osc->phase.pending_update){
        if (osc->distortion.on) {
            /* check linked tasks */
            uint16_t tmp =
                (osc->in_the_house.report == 0) ?
                data->osc_0_cv_distortion_amount :
                data->distortion_amount_cv;

            osc->distortion.amount = map_uint(tmp,
                                              &osc->distortion.level_range);
        } else {
            /* who tf knows */
        }
        bool staged = stage_pending_inc(note, osc, data->dac1_clock);
        osc->phase.done_update = staged;
        osc->phase.pending_update = !osc->phase.done_update;
        osc->in_the_house.report = false;
    }
}
