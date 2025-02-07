#include "nco.h"
#include "stm32g071xx.h"
#include <stdbool.h>
#include <stdint.h>
// DDS variables (phase accumulator technique)
extern volatile uint32_t phase_accum;
extern volatile uint64_t phase_inc;
extern volatile bool phase_pending_update;
extern volatile uint64_t phase_pending_update_value;

void update_ping_pong_buff(volatile const uint16_t *data, uint16_t *bufferSection, uint16_t sectionLength) {
    for (uint16_t i = 0; i < sectionLength; i++) {
        uint32_t index = (uint32_t) (((uint64_t) phase_accum * (1 << 7)) >> 32);
        bufferSection[i] = (uint16_t) data[index];

        // Advance the phase accumulator by the phase increment.
        phase_accum += phase_inc;
    }
}

static uint16_t map_12b_to_hz(uint16_t value) {
    uint16_t min = 440;
    uint16_t max = 830;
    return min + (value * (max - min)) / 0xfff;
}


ErrorStatus alter_wave_frequency (uint32_t output_freq){
    output_freq = (output_freq == 0) ? 1 : output_freq;
    phase_pending_update_value = ((((uint64_t)output_freq) * ((uint64_t)1UL << 32)) / 44100);
    return SUCCESS;
}
