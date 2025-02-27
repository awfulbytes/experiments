#include "stdatomic.h"
#include "nco.h"
// #include "stm32g071xx.h"
#include <stdbool.h>
#include <stdint.h>
// DDS variables (phase accumulator technique)
extern volatile uint32_t phase_accum;
extern volatile uint64_t phase_inc;
extern volatile bool phase_pending_update;
extern volatile uint64_t phase_pending_update_inc;

void update_ping_pong_buff(volatile const uint16_t *data, uint16_t *bufferSection, uint16_t sectionLength) {
    for (uint16_t i = 0; i < sectionLength; i++) {
        uint32_t index = (uint32_t) (((uint64_t) phase_accum * (1 << 7)) >> 32);
        // uint32_t index = phase_accum >> 24;
        bufferSection[i] = (uint16_t) data[index];

        phase_accum += phase_inc;
    }
}

uint16_t map_12b_to_hz(uint16_t value) {
    uint16_t min = 173;
    uint16_t max = 290;
    return min + (value * (max - min)) / 0xfff;
}


void alter_wave_frequency (uint32_t output_freq){
    // NOTE::: i may excede the cpu cycles on hand....
    //         i have 64 MHz / 44100 = ~1454 cycles ....
    //         i need a HACK
    //
    // FIXME:: will not work on 32 bit interrupt systems
    //         i need os lock for this and i dont want to deal with RTOS...
    // atomic_uint_least64_t freq = (atomic_uint_least64_t) 0x001000000;
    // atomic_load(&freq);
    phase_pending_update_inc =
        ((uint64_t)((output_freq) * (1UL << 32))) / 44000;
    return;
}
