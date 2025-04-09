#include <stdatomic.h>
#include <stdint.h>
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#include "nco.h"
// include "stm32g071xx.h"
// #define TEST
// DDS variables (phase accumulator technique)
inline static uint32_t compute_1cycle_lut_index(struct nco nco[static 1]);

void generate_half_signal(volatile const uint16_t data[static 128],
                          // volatile const uint16_t dither_source[static 128],
                          uint16_t sectionLength, struct nco nco[static 1]){

    for (uint16_t i = 0; i < sectionLength; ++i) {
        uint32_t index = compute_1cycle_lut_index(nco);
        uint32_t n_index = index + 1;
        uint16_t a = data[index];
        uint16_t b = data[n_index & 0x7f];
        int16_t diff = (int16_t) (b-a);
        // diff = ((diff<0) ? -diff : diff);
        uint32_t fract = ((uint64_t)((nco->phase_accum) * (1<<7)) >> 16) & 0xffff;

#ifdef TEST
#include <stdio.h>
        printf("fract:\t%d\n", fract);
        printf("interp-factor:\t%d\n", (diff * fract)>>16);
#endif // TEST

        nco->data_buff.ping_buff[i] = (a + (((diff * fract) >> 16)));
        nco->phase_accum += nco->phase_inc;
    }
}

void compute_nco_increment(atomic_ushort note, struct nco *nco, const uint_fast16_t sample_rate){

    atomic_uint_fast32_t tmp = ((note * (1<<16))/sample_rate);
    nco->phase_pending_update_inc = (tmp<<16);
}

uint32_t map_12b_to_hz(uint16_t value) {
    uint16_t in_max = 0xfff;
    uint32_t min = 110;
    uint32_t max = 440; // NOTE:: this measures 830.{38-61} [Hz]
    uint32_t range = max - min;
    return min + (value * range) / in_max;
}

void stage_pending_inc(volatile uint16_t adc_raw_value, struct nco *nco, const uint_fast16_t sample_rate){
    atomic_ushort note = map_12b_to_hz(adc_raw_value);
    compute_nco_increment(note, nco, sample_rate);
    nco->phase_pending_update = false;
}

inline void update_ping_pong_buff(volatile const uint16_t data[static 128],
                           atomic_ushort bufferSection[static 128],
                           uint16_t sectionLength) {
    memcpy(bufferSection, data, sizeof(uint16_t) * sectionLength);
}
