#include "overseer.h"

volatile const uint16_t diatonic_g_major[] = {
  24,   27,   30,   32,   36,   40,   45,
  48,   54,   60,   64,   72,   80,   90,
  96,  108,  120,  128,  144,  160,  180,
 192,  216,  240,  256,  288,  320,  360,
 384,  432,  480,  512,  576,  640,  720,
 768,  864,  960, 1024, 1152, 1280, 1440,
1536, 1728, 1920, 2048, 2304, 2560, 2880,
3072, 3456, 3840, 4096, 4608, 5120, 5760};

static inline void modify_osc_tempering(volatile struct nco *o){
    switch (o->tempered.type) {
        case none:
            o->mode = tracking;
            o->tempered.type = diatonic_major_g;
            break;
        case diatonic_major_g:
            o->mode = free;
            o->tempered.type = eq_tempered;

            /* protect the hearing of people... */
            if(!o->tempered.rec_history)
                o->tempered.first_fundamental = o->tempered.absolute.min;
            break;
        case eq_tempered:
            o->mode = free;
            o->tempered.type = none;
            break;
    }
    o->tempered.type_change = false;
}

void tune(struct overseer *seer, uint8_t osc_idx, struct display *d){
    seer->selected = seer->oscillators[osc_idx];
    register size_t g_major_size = array_size(diatonic_g_major);

    if(seer->selected->tempered.type_change)
        modify_osc_tempering(seer->selected);

    if(seer->selected->phase.pending_update){
        if(seer->sync && osc_idx==1){
            seer->_data->_cv_1_pitch = seer->_data->_cv_0_pitch;
        }
        register uint16_t pitch_raw_digital =
            (seer->selected->in_the_house.report == 0)
            ? seer->_data->_cv_0_pitch
            : seer->_data->_cv_1_pitch;

        register uint16_t note =
            (seer->selected->mode == free)
            ? map_uint(pitch_raw_digital, &seer->selected->bandwidth.free)
            : map_uint(pitch_raw_digital, &seer->selected->bandwidth.tracking);

        switch(seer->selected->tempered.type) {
            case none:
                d->view[osc_idx] = wave;
                seer->_data->pitch_cv = note;
                break;
            case eq_tempered:
                d->view[osc_idx] = tuning;
                if(seer->selected->tempered.flag){
                    d->tuner_view[osc_idx] = recording;
                    seer->_data->pitch_cv = seer->selected->tempered.first_fundamental;
                    break;
                }else{
                    d->tuner_view[osc_idx] = playing;
                }
                seer->_data->pitch_cv = equal_tempered(seer->selected, pitch_raw_digital);
                octave_recorder(d, seer->oscillators[osc_idx]->tempered.oct.span, osc_idx);
                break;
            case diatonic_major_g:
                d->view[osc_idx] = diatonic;
                seer->_data->pitch_cv = diatonic_lut_search(note, diatonic_g_major, g_major_size);
                break;
        }

        if(seer->selected->distortion.on){
            d->view[osc_idx] = dist;
            tune_distortion(seer->selected, seer->_data);
        }

        seer->selected->phase.done_update =
            stage_pending_inc(seer->_data->pitch_cv, seer->selected,
                              seer->_data->dac1_clock);

        seer->selected->phase.pending_update = !seer->selected->phase.done_update;
    }
}

static inline void tune_distortion(volatile struct nco osc[static 1],
                                   volatile struct   overworld *data){
    uint16_t tmp =
        (osc->in_the_house.report == 0)
        ? data->osc_0_cv_distortion_amount
        : data->osc_1_cv_distortion_ammount;
    osc->distortion.amount = map_uint(tmp, &osc->distortion.level_range);
}

void merge_signals_dual_dac_mode(volatile struct nco *o[2], uint32_t dual_buffer[128], uint8_t table_size){
    for(uint8_t z=0; z < table_size; ++z){
        dual_buffer[z] = o[1]->data_buff[z] << 16U |\
                         o[0]->data_buff[z];
    }
}

static uint16_t diatonic_lut_search(volatile uint16_t note, volatile const uint16_t *scale_table, size_t g_major_tbl_size){
    register uint16_t f = g_major_tbl_size;

    if(note > scale_table[g_major_tbl_size - 1]){
        return scale_table[g_major_tbl_size - 1];
    }

    for(; f > 0; --f){
        if(note < scale_table[f])
            continue;
        else {
            if((scale_table[f + 1] - note) > (note - scale_table[f]))  /* (diff_up > diff_down) */
                return scale_table[f];
            else
                return scale_table[f + 1];
        }
    }
    return scale_table[0];
}

uint16_t equal_tempered(volatile struct nco *o, uint16_t pitch_raw_dig){
    register uint16_t main_pitch_cv = 0;

    /*
     * procced to tempered note calculation if nothing has changed or this is
     * a first run (checking for valid _semi_tones_in_range).
     */
    if(!o->tempered.oct.change && !o->tempered.oct.shift &&
       !o->tempered.just_reced && o->tempered._semi_tones_in_range != 0)
        goto compute_eq;

    if(o->tempered.oct.change){
        switch(o->tempered.oct.span){
            case 1:
            case 2:
            case 3:
            case 4:
                ++o->tempered.oct.span;
                break;
            case 5:
                o->tempered.oct.span = 1;
                break;
        }
        o->tempered.oct.change = false;
    }
    o->tempered.just_reced = false;

    if(o->tempered.oct.shift){
        o->tempered.first_fundamental <<= 1;
        o->tempered.oct.jump >>= 1;
        o->tempered.oct.shift = false;
    }

    /**
     * Mutate the octave span if we extend beyond 12 kHz.
     * If the octave span is one to avoid underflow, or if the base note is so
     * high that nomatterwhat we end up at higher than 12k, the fundamental is
     * modified after the octave span is at the lowest possible value.
     */
    o->tempered.last_to_first_ratio = o->tempered.oct.span << 1;

    while((o->tempered.first_fundamental * o->tempered.last_to_first_ratio) >
          o->tempered.absolute.max){
        if(o->tempered.oct.span != 1){
            o->tempered.oct.span -= 1;
        }else{
            o->tempered.first_fundamental = o->tempered.absolute.max >> 1;
        }
        o->tempered.last_to_first_ratio = o->tempered.oct.span << 1;
    }

    if(o->tempered.last_to_first_ratio != 2){
        o->tempered._semi_tones_in_range = (o->tempered.oct.unit * o->tempered.last_to_first_ratio) >> 1U;
        o->tempered.last_fundamental = o->tempered.first_fundamental * o->tempered.last_to_first_ratio;
    }else{
        o->tempered._semi_tones_in_range = o->tempered.oct.unit;
        o->tempered.last_fundamental = o->tempered.first_fundamental << 1U;
    }

    o->tempered.mutable_bounds.min = o->tempered.first_fundamental;
    o->tempered.mutable_bounds.max = o->tempered.last_fundamental;

    o->tempered.semitone = (o->tempered.last_fundamental - o->tempered.first_fundamental) / o->tempered._semi_tones_in_range;
    o->tempered.drift_error = o->tempered.semitone << 1U;

compute_eq:
    main_pitch_cv = map_uint(pitch_raw_dig, &o->tempered.mutable_bounds);
    o->tempered.cv_semitones =
        (main_pitch_cv - o->tempered.first_fundamental) / o->tempered.semitone;
fixup:
    o->tempered.quantized_et = o->tempered.first_fundamental + (o->tempered.cv_semitones * o->tempered.semitone);

    if(o->tempered.quantized_et > (o->tempered.mutable_bounds.max + (o->tempered.drift_error))){
        o->tempered.cv_semitones -= o->tempered.semitone;
        goto fixup;
    }

    return o->tempered.quantized_et;
}
