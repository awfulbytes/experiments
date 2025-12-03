#include "overseer.h"
#include "nco.h"

uint16_t tune_to_bandwidth(struct overseer *overseer, uint8_t osc_idx) {
    register uint16_t raw_val = overseer->universe_data->current_value_cv_0_pitch;
    struct nco selected_oscillator = *overseer->oscillators[osc_idx];
    uint16_t note =
        (selected_oscillator.mode == free) ?
        map_uint(raw_val, &selected_oscillator.bandwidth.free) :
        map_uint(raw_val, &selected_oscillator.bandwidth.tracking);
    /* overseer->oscillators[0]->phase.pending_update_inc = compute_nco_increment(note, overseer->universe_data->adc1_clock); */
    return note;
}

#pragma GCC diagnostic ignored "-Wunused-variable"
void stage_modulated_signal_values(struct   nco      osc[static 1],
                                   register uint16_t note,
                                   struct overworld *data){
    if(osc->phase.pending_update){
        if (osc->distortion.on) {
            osc->distortion.amount = map_uint(data->distortion_amount_cv,
                                              &osc->distortion.level_range);
        } else {
            register uint16_t tmp =
                (osc->mode == v_per_octave) ?
                map_uint(data->distortion_amount_cv, &osc->bandwidth.tracking) :
                map_uint(data->distortion_amount_cv, &osc->bandwidth.free);
            /* todo test later
             * 0b000111111111100 mask */
            /* data->current_value_cv_0_pitch |= (tmp >> 9); */
            uint16_t mask_p = 0xff0;
            data->current_value_cv_0_pitch &= mask_p;
            tmp &= 0x00f;
            data->current_value_cv_0_pitch |= tmp;
            /* todo test later */
        }
        bool staged = stage_pending_inc(note, osc, data->dac1_clock);
        osc->phase.done_update = staged;
        osc->phase.pending_update = !osc->phase.done_update;
    }
}
