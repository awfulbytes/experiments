#include "overseer.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
void stage_modulated_signal_values(struct   nco      osc[static 1],
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
        bool staged = stage_pending_inc(data->current_value_cv_0_pitch, osc, data->dac1_clock);
        osc->phase.done_update = staged;
        osc->phase.pending_update = !osc->phase.done_update;
    }
}
