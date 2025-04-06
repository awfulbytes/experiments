#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#include "nco.h"
#ifndef USE_FULL_ASSERT
#include "stm32_assert_template.h"
#include <assert.h>
#endif
// include "stm32g071xx.h"
// #define TEST
// DDS variables (phase accumulator technique)

void generate_half_signal(volatile const uint16_t data[static 128],
                          volatile const uint16_t dither_source[static 128],
                          uint16_t sectionLength, struct nco nco[static 1]){

    for (uint16_t i = 0; i < sectionLength; i++) {
        uint32_t index = compute_1cycle_lut_index(nco);
        nco->data_buff.ping_buff[i] = data[index] + dither_source[i];
        nco->phase_accum += nco->phase_inc;
    }
}

void update_ping_pong_buff(volatile const uint16_t data[static 128],
                           atomic_ushort bufferSection[static 128],
                           uint16_t sectionLength) {
    memcpy(bufferSection, data, sizeof(uint16_t) * sectionLength);
}

uint32_t map_12b_to_hz(uint16_t value) {
    uint16_t in_max = 0xfff;
    uint32_t min = 440;
    uint32_t max = 832; // NOTE:: this measures 830.{38-61} [Hz]
    uint32_t range = max - min;
    return min + (value * range) / in_max;
}
