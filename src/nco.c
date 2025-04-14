#include <stdint.h>
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "nco.h"
#include "string.h"
// #define TEST
inline static uint32_t compute_lut_index(struct nco nco[static 1]);

void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t sectionLength, struct nco nco[static 1]){

    for (uint16_t i = 0; i < sectionLength; ++i) {
        uint32_t index = compute_lut_index(nco);
        uint32_t n_index = index + 1;
        uint16_t a = data[index];
        uint16_t b = data[n_index & 0x7f];
        int16_t diff = (int16_t) (b-a);
        uint32_t fract = ((uint64_t)((nco->phase_accum) * (1<<7)) >> 16) & 0xffff;

#ifdef TEST
#include <stdio.h>
        printf("fract:\t%d\n", fract);
        printf("interp-factor:\t%d\n", (diff * fract)>>16);
#endif // TEST

        nco->data_buff.ping_buff[i] = (a + ((uint16_t)(((diff * fract) >> 16))));
        nco->phase_accum += nco->phase_inc;
    }
}

uint64_t compute_nco_increment(atomic_ushort note, const uint_fast16_t sample_rate){
    atomic_uint_fast32_t tmp = ((note * (1<<16))/sample_rate);
    // TODO:: test on chip
    return (tmp<<16);
}

atomic_ushort map_12b_to_hz(uint16_t value, enum modes pitch_mode) {
    atomic_ushort in_max = 0xfff;
    atomic_ushort min;
    atomic_ushort max;
    switch (pitch_mode) {
        case free:
            min = 20;
            max = 16646;
            break;
        case single_octave:
            min = 220;
            max = 415;
            break;
        default:
            min = 100;
            max = 300;
    }
    atomic_ushort range = max - min;
    return (atomic_ushort)(min + (value * range) / in_max);
}

void stage_pending_inc(volatile uint16_t adc_raw_value, struct nco nco[static 1], const uint_fast16_t sample_rate){
    atomic_ushort note = map_12b_to_hz(adc_raw_value, nco->mode);
    nco->phase_pending_update_inc = compute_nco_increment(note, sample_rate);
    nco->phase_pending_update = false;
}

inline void update_ping_pong_buff(const uint16_t data[static 128],
                                  atomic_ushort bufferSection[static 128],
                                  uint16_t sectionLength) {
    memcpy(bufferSection, data, sizeof(uint16_t) * sectionLength);
}
