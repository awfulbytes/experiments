#include "overseer.h"
#include "nco.h"

void tune(struct overseer *overseer,
          uint8_t          osc_idx){
    overseer->selected = overseer->oscillators[osc_idx];

    if(overseer->selected->phase.pending_update){
        if(overseer->sync){
            /* hack::
             *        this makes a nice distortion value following the 0th oscillator!! test if i can manipulate it further
             */
            overseer->universe_data->current_value_cv_1_pitch = overseer->universe_data->current_value_cv_0_pitch;
        }
        register uint16_t pitch_raw_digital =
            (overseer->selected->in_the_house.report == 0)
            ? overseer->universe_data->current_value_cv_0_pitch
            : overseer->universe_data->current_value_cv_1_pitch;

        overseer->universe_data->pitch_cv =
            (overseer->selected->mode == high)
            ? map_uint(pitch_raw_digital, &overseer->selected->bandwidth.high)
            : map_uint(pitch_raw_digital, &overseer->selected->bandwidth.low);

        if (overseer->selected->distortion.on)
            tune_distortion(overseer->selected, overseer->universe_data);

        overseer->selected->phase.done_update =
            stage_pending_inc(overseer->universe_data->pitch_cv, overseer->selected, overseer->universe_data->dac1_clock);
        overseer->selected->phase.pending_update = !overseer->selected->phase.done_update;
    }
}

static inline void tune_distortion(volatile struct nco osc[static 1],
                                   volatile struct   overworld *data){
    uint16_t tmp =
        (osc->in_the_house.report == 0)
        ? data->osc_0_cv_distortion_amount
        : data->osc_1_cv_distortion_ammount;
    osc->distortion.amount_2 = map_uint(data->osc_0_cv_2_distortion_amount, &osc->distortion.level_range);
    osc->distortion.amount = map_uint(tmp, &osc->distortion.level_range);
}

void merge_signals_dual_dac_mode(volatile struct nco *o[2], uint32_t dual_buffer[128], uint8_t table_size){
    for(uint8_t z=0; z < table_size; ++z){
        dual_buffer[z] = o[1]->data_buff[z] << 16U |\
                         o[0]->data_buff[z];
    }
}

void sync_fcw(volatile struct nco *o[2]){
    o[1]->phase.inc = o[0]->phase.inc;
}
