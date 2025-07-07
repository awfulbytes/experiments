#include "nco.h"
#include "string.h"
// #define TEST

void apply_pd_alg(struct nco nco[static 1]){
    nco->distortion.distortion_value = nco->phase_inc << nco->distortion.dante;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t sector_length,
                          struct nco nco[static 1]){
    register uint32_t index, n_index, fract;
    register uint16_t a, b, y;
    register int16_t  diff;

    for (y = 0; y < sector_length; ++y) {
        index   = compute_lut_index(nco);
        n_index = index + 1;
        a       = data[index];
        b       = data[n_index & 0x7f];
        diff    = (int16_t) (b-a);
        fract   = ((uint64_t)((nco->phase_accum) * (1<<7)) >> 16) & 0xffff;

#ifdef TEST
#include <stdio.h>
        printf("fract:\t%d\n", fract);
        printf("interp-factor:\t%d\n", (diff * fract)>>16);
#endif // TEST
        if (y == nco->distortion.amount
            && nco->distortion.on){
            apply_pd_alg(nco);
            // hack:: make additive or subtractive it makes nice sounds
            nco->phase_inc -= (nco->distortion.distortion_value);
        }
        nco->data_buff.ping_buff[y] = (a + ((uint16_t)(((diff * fract) >> 16))));
        nco->phase_accum += nco->phase_inc;
    }
}

__attribute__((pure))
uint16_t map_12b_to_distortion_amount(uint16_t value,
                                      struct limits *level_range) {
    uint16_t range = level_range->max - level_range->min;
    return (uint16_t)(level_range->min + (value * range) / level_range->cv_raw_max);
}

__attribute__((pure))
uint16_t map_12b_to_hz(uint16_t value,
                       struct limits freq_bounds[static 1]) {
    uint16_t range = freq_bounds->max - freq_bounds->min;
    return (uint16_t)(freq_bounds->min + (value * range) / freq_bounds->cv_raw_max);
}

__attribute__((pure))
bool stage_pending_inc(volatile uint16_t adc_raw_value,
                       struct nco nco[static 1],
                       const uint_fast32_t sample_rate){
    uint16_t note =
        (nco->mode == free) ?
        map_12b_to_hz(adc_raw_value, &nco->bandwidth.free) :
        map_12b_to_hz(adc_raw_value, &nco->bandwidth.tracking);
    nco->phase_pending_update_inc = compute_nco_increment(note, sample_rate);
    return true;
}

__attribute__((always_inline))
inline void update_data_buff(const uint16_t data[static 128],
                             uint16_t buffer_sector[static 128],
                             uint16_t sector_length) {
    memcpy(buffer_sector, data, sizeof(uint16_t) * sector_length);
}

void stage_modulated_signal_values(struct   nco      osc[static 1],
                                   volatile uint16_t pitch_cv,
                                   uint16_t          distortion_cv,
                                   uint32_t          master_clock){
    if(osc->phase_pending_update){
        osc->distortion.amount = map_12b_to_distortion_amount(distortion_cv, &osc->distortion.level_range);
        bool staged = stage_pending_inc(pitch_cv, osc, master_clock);
        osc->phase_done_update = staged;
        osc->phase_pending_update = !staged;
    }
}
