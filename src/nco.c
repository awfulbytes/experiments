#include "stdatomic.h"
#include "nco.h"
// #include "stm32g071xx.h"
#include <stdbool.h>
#include <stdint.h>
// #define TEST
// DDS variables (phase accumulator technique)
extern volatile uint32_t phase_accum;
extern volatile uint64_t phase_inc;
extern volatile bool phase_pending_update;
extern volatile uint64_t phase_pending_update_inc;

void update_ping_pong_buff(volatile const uint16_t *data, uint16_t *bufferSection, uint16_t sectionLength) {
    for (uint16_t i = 0; i < sectionLength; i++) {
        uint32_t index = (uint32_t) (((uint64_t) (phase_accum << 8)) >> 32);
        // uint32_t index = phase_accum << 24;
        bufferSection[i] = (uint16_t) data[index];

        phase_accum += phase_inc;
#ifdef TEST
        #include "stdio.h"
        printf("accum: %d\n", phase_accum);
#endif // TEST
    }
}

uint16_t map_12b_to_hz(uint16_t value) {
    uint16_t min = 174;
    uint16_t max = 291; // NOTE:: off by 1
    return min + (value * (max - min)) / 0xfff;
}


void alter_wave_frequency (uint32_t output_freq){
    // NOTE::: i may excede the cpu cycles on hand....
    //         i have 64 MHz / 44100 = ~1454 cycles ....
    //         i need a HACK
    // HACK::::::::::::::::::::::::::::::::::::::::::::::::
    // one idea is to  make numbers fixed...
    // another one is to create a range as the mapper does.
    // HACK::::::::::::::::::::::::::::::::::::::::::::::::
    //
    // FIXME:: will not work on 32 bit interrupt systems
    //         i need os lock for this and i dont want to deal with RTOS...
    // atomic_uint_least64_t freq = (atomic_uint_least64_t) 0x001000000;
    // atomic_load(&freq);
    phase_pending_update_inc =
        ((((uint64_t) output_freq) << 32)) / 44000;
    return;
}
