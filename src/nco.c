#include "nco.h"
#include <string.h>

void apply_pd_alg(struct nco nco[static 1]){
    nco->distortion.distortion_value = nco->phase.inc << nco->distortion.dante;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t                sector_length,
                          struct   nco            nco[static 1]){
    register uint32_t index, n_index, fract;
    register uint16_t a, b, y;
    register int16_t  diff;

    for (y = 0; y < sector_length; ++y) {
        index   = compute_lut_index(nco);
        n_index = index + 1;
        a       = data[index];
        b       = data[n_index & 0x7f];
        diff    = (int16_t) (b-a);
        fract   = ((uint64_t)((nco->phase.accum) * (1<<7)) >> 16) & 0xffff;

#ifdef TEST
        dbg_info_nco(nco, fract, diff);
#endif // TEST
        if (y == nco->distortion.amount
            && nco->distortion.on) {
            apply_pd_alg(nco);
            // hack:: make additive or subtractive it makes nice sounds
            nco->phase.inc -= (nco->distortion.distortion_value);
        }
        nco->data_buff.ping_buff[y] = (a + ((uint16_t)(((diff * fract) >> 16))));
        nco->phase.accum += nco->phase.inc;
    }
}

__attribute__((pure))
uint16_t map_uint(uint16_t      value,
                  struct limits boundaries[static 1]) {
    uint16_t range = boundaries->max - boundaries->min;
    return (uint16_t)(boundaries->min + (value * range) / boundaries->cv_raw_max);
}

__attribute__((pure))
bool stage_pending_inc(volatile uint16_t      adc_raw_value,
                       struct   nco           nco[static 1],
                       const    uint_fast32_t sample_rate){

    /*
     * todo(nxt) use different bandwidths to support the hole audio spectrum.
     */

    uint16_t note =
        (nco->mode == free) ?
        map_uint(adc_raw_value, &nco->bandwidth.free) :
        map_uint(adc_raw_value, &nco->bandwidth.tracking);
    nco->phase.pending_update_inc = compute_nco_increment(note, sample_rate);

    return true;
}

__attribute__((always_inline))
inline void update_data_buff(const uint16_t data[static 128],
                             uint16_t       buffer_sector[static 128],
                             uint16_t       sector_length) {
    memcpy(buffer_sector, data, sizeof(uint16_t) * sector_length);
}

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
