#include "nco.h"
#include "string.h"
// #define TEST
__attribute__((pure, always_inline)) inline static uint32_t compute_lut_index(struct nco nco[static 1]);
__attribute__((pure, always_inline)) inline static uint64_t compute_nco_increment(atomic_ushort note, const uint_fast32_t sample_rate);

void apply_pd_alg(struct nco nco[static 1]){
    nco->distortion.distortion_value = nco->phase_inc << nco->distortion.dante;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t sectionLength, struct nco nco[static 1]){

    register uint32_t index, n_index, fract;
    register uint16_t a, b;
    register int16_t  diff;

    for (uint16_t i = 0; i < sectionLength; ++i) {
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
        if (i == nco->distortion.amount
            && nco->distortion.on){
            apply_pd_alg(nco);
            // hack:: make additive or subtractive it makes nice sounds
            nco->phase_inc -= (nco->distortion.distortion_value);
        }
        nco->data_buff.ping_buff[i] = (a + ((uint16_t)(((diff * fract) >> 16))));
        nco->phase_accum += nco->phase_inc;
    }
}

__attribute__((pure)) atomic_ushort map_12b_to_distortion_amount(uint16_t value) {
    atomic_ushort in_max = 0xfff;
    atomic_ushort min;
    atomic_ushort max;
    min = 63;
    max = 127;
    atomic_ushort range = max - min;
    return (atomic_ushort)(min + (value * range) / in_max);
}

__attribute__((pure)) atomic_ushort map_12b_to_hz(uint16_t value, enum freq_modes mode) {
    atomic_ushort in_max = 0xfff;
    atomic_ushort min;
    atomic_ushort max;
    switch (mode) {
        case free:
            min = 110;
            max = 16000;
            break;
        case v_per_octave:
            min = 220;
            max = 1661;
            break;
        default:
            min = 2;
            max = 5;
    }
    atomic_ushort range = max - min;
    return (atomic_ushort)(min + (value * range) / in_max);
}

__attribute__((pure)) bool stage_pending_inc(volatile uint16_t adc_raw_value, struct nco nco[static 1], const uint_fast32_t sample_rate){
    atomic_ushort note = map_12b_to_hz(adc_raw_value, nco->mode);
    nco->phase_pending_update_inc = compute_nco_increment(note, sample_rate);
    return true;
}

inline void update_data_buff(const uint16_t data[static 128],
                                  atomic_ushort bufferSection[static 128],
                                  uint16_t sectionLength) {
    memcpy(bufferSection, data, sizeof(uint16_t) * sectionLength);
}

void stage_modulated_signal_values(struct nco osc[static 1], atomic_ushort distortion_cv, volatile uint16_t pitch_cv, uint32_t master_clock){
    if(osc->phase_pending_update){
        osc->distortion.amount = map_12b_to_distortion_amount(distortion_cv);
        bool staged = stage_pending_inc(pitch_cv, osc, master_clock);
        osc->phase_done_update = staged;
        osc->phase_pending_update = !staged;
    }
}
