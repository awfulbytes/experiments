#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#include "nco.h"
#ifndef USE_FULL_ASSERT
#include "stm32_assert_template.h"
#include <assert.h>
#endif
// include "stm32g071xx.h"
// #define TEST
// DDS variables (phase accumulator technique)


void update_ping_pong_buff(volatile const uint16_t data[static 128],
                           uint16_t bufferSection[static 128],
                           uint16_t sectionLength,
                           struct nco *nco) {
    for (uint16_t i = 0; i < sectionLength; ++i) {
        uint32_t index = compute_1cycle_lut_index(nco);
        memcpy(&bufferSection[i], &data[index], sizeof(uint16_t));

        nco->phase_accum += nco->phase_inc;
#ifdef TEST
        #include "stdio.h"
        printf("accum: %d\n", nco->phase_accum);
#endif // TEST
    }
}

uint32_t map_12b_to_hz(uint16_t value) {
    uint16_t in_max = 0xfff;
    uint32_t min = 440;
    uint32_t max = 830; // NOTE:: off by 1
    uint32_t range = max - min;
    return min + (value * range) / in_max;
}
