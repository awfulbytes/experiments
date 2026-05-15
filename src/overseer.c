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

void tune(struct overseer *seer, uint8_t osc_idx, struct display *d){
    seer->selected = seer->oscillators[osc_idx];
    register size_t g_major_size = array_size(diatonic_g_major);

    if(seer->selected->phase.pending_update){
        if(seer->sync){
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
                // if(osc_idx == 0)
                d->view[osc_idx] = wave;
                seer->_data->pitch_cv = note;
                break;
            case eq_tempered:
                d->view[osc_idx] = tuning;
                d->tuner_view[osc_idx] = playing; //global shit
                if(seer->selected->tempered.flag){
                    seer->_data->pitch_cv = seer->selected->tempered.first_fundamental;
                    d->tuner_view[osc_idx] = recording; //global shit
                    break;
                }

                seer->_data->pitch_cv = equal_tempered(seer->selected, pitch_raw_digital, d);

                if(osc_idx!=1)
                    octave_recorder(&display, seer->oscillators[osc_idx]->tempered.oct.span, osc_idx);

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

static uint16_t diatonic_lut_search(volatile uint16_t note,
                               volatile const uint16_t *scale_table,
                               size_t g_major_tbl_size){
    /*
     * todo:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     * a. make the table resizable!!
     *    The idea is to have min/max (a range) of octaves or/and frequencies..?
     *    We need to have a first fundamental and the octave span and we can
     *    calculate the last fundamental frequency. (Do not give more than 5-6
     *    octaves.
     * b. using intervals may be more efficient! --> make later -^
     *    ^--this possible first and
     * todo:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
     */
    register uint16_t f = g_major_tbl_size;
    register uint16_t diff_up = 0, diff_down = 0;

    if(note > scale_table[g_major_tbl_size - 1]){
        return scale_table[g_major_tbl_size - 1];
    }

    for(; f > 0; --f){

        if(note < scale_table[f])
            continue;
        else {
            diff_down = note - scale_table[f];
            diff_up   = scale_table[f + 1] - note;
            if(diff_up > diff_down){
                return scale_table[f];
            } else
                return scale_table[f + 1];
        }
    }
    return scale_table[0];
}

#pragma message("semi working")
uint16_t equal_tempered(volatile struct nco *o, uint16_t pitch_raw_dig, struct display *d){
    register uint16_t range = 0;
    volatile uint16_t freq_diff_from_base;
    register uint16_t _semi_tones_in_range = 0, semitone = 0, cv_semitones = 0;
    register uint16_t main_pitch_cv = 0;
    register uint16_t last_to_first_ratio = 0;

the_begining:
    last_to_first_ratio = o->tempered.oct.span << 1;
    if(last_to_first_ratio != 2)
        _semi_tones_in_range = o->tempered.oct.unit * last_to_first_ratio;
    else
        _semi_tones_in_range = o->tempered.oct.unit;

recalculate:
    o->tempered.last_fundamental = o->tempered.first_fundamental * last_to_first_ratio;

    if(o->tempered.last_fundamental > (o->tempered.hard_bounds.max * 2)){
        if(last_to_first_ratio != 2){
            o->tempered.oct.span -= 1;
            //this is not recorded for the display
            // it should be as im recording the span later
            goto the_begining;
        }else{
            // !;
            //ooo i modify this also so i have to get all the states from here
            //          lets say im jumping from last to previews octave.
            //          we have to be sure if we jumped or spanned.
            //    the above is not yet clear to the project so this is firing
            //    issues because sampling the jump before here will ignore the
            //    negative jumping!!!!
            o->tempered.oct.jump = -1;
            o->tempered.oct.shift = true;
        }
        //reduction
        o->tempered.first_fundamental -= (o->tempered.first_fundamental >> 6);
        goto recalculate;
    }

    if(o->tempered.first_fundamental < o->tempered.hard_bounds.min){
        o->tempered.first_fundamental = map_uint(o->tempered.first_fundamental, &o->tempered.hard_bounds);
        goto recalculate;
    }

    if(o->tempered.oct.shift){
        for(;o->tempered.oct.jump > 0;){
            o->tempered.first_fundamental <<= 1;
            --o->tempered.oct.jump;
        }
        for(;o->tempered.oct.jump < 0;){
            //gives us backwards but too much
            d->backwards_jump[0] = (d->backwards_jump[0] == 0) ? 1 : 0;
            o->tempered.first_fundamental >>= 1;
            ++o->tempered.oct.jump;
        }
        o->tempered.oct.shift = false;
        goto recalculate;
    }
    o->tempered.mutable_bounds.min = o->tempered.first_fundamental;
    o->tempered.mutable_bounds.max = o->tempered.last_fundamental;

    main_pitch_cv = map_uint(pitch_raw_dig,
                             &o->tempered.mutable_bounds);

    range    = o->tempered.last_fundamental - o->tempered.first_fundamental;
    /* todo
     * test this actually works and gives us back real numbers...
     *      we might need division before multiplication...
     */
    freq_diff_from_base = (main_pitch_cv - o->tempered.first_fundamental);
    semitone = range / _semi_tones_in_range;
    cv_semitones  = freq_diff_from_base / semitone;
fixup:
    o->tempered.quantized_et = o->tempered.first_fundamental + (cv_semitones * semitone);
    if(o->tempered.quantized_et > (o->tempered.mutable_bounds.max + semitone)){
        cv_semitones -= semitone;
        goto fixup;
    }


    return o->tempered.quantized_et;
}
