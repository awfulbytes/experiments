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
        uint32_t index = (uint32_t) (((uint64_t) phase_accum * (1 << 7)) >> 32);
        // uint32_t index = phase_accum << 24;
        bufferSection[i] = (uint16_t) data[index];

        phase_accum += phase_inc;
#ifdef TEST
        #include "stdio.h"
        printf("accum: %d\n", phase_accum);
#endif // TEST
    }
}

uint64_t map_12b_to_hz(uint16_t value) {
    uint16_t in_max = 0xfff;
    uint32_t min = 440;
    uint32_t max = 830; // NOTE:: off by 1
    uint32_t range = max - min;
    return min + (value * range) / in_max;
}

// double octave[21] = {440,
//                      466.16, 466.16, 493.88, 493.88,
//                      523.25, 523.25, 554.37, 554.37, 587.33,
//                      622.25, 622.25, 659.26, 659.26,
//                      698.46, 698.46, 739.99, 739.99, 783.99,
//                      830.61, 830.61};
uint32_t freqs[12]={440, 467, 494, 524,
                    555, 588, 623, 660,
                    699, 740, 784, 831};

const uint32_t osc_oct_incs[12]={0x28f5c28, 0x2b652fa, 0x2df9c9f, 0x30b5b6a,
                         0x339b57b, 0x36acd24, 0x39ed026, 0x3d5f013,
                         0x410530d, 0x44e2e74, 0x48fb7ac, 0x4d527e5};

// void osc_lut_inc_generator(){
//     for (uint16_t i=0; i<21; i++) {
//         incs[i] = (uint64_t)((octave[i] * (1ULL<<32))/44000);
//     }
// }

static uint64_t search(uint32_t item, const uint32_t *arr){
    int found = 0;
    uint64_t curr = ((uint64_t) (((uint64_t)item) << 32)) / 44000;
    for (int i=0; i<12; i++) {
        if (curr == arr[i]){
            found = i;
            break;
        }
    }
    return arr[found];
}

uint32_t alter_wave_frequency (uint32_t output_freq){
    uint32_t found = 0;
    for (int i=0; i<12; i++) {
        if (output_freq > freqs[i] && output_freq <= freqs[i+1]){
            found = i;
            break;
        } else {continue;}
    }
    // NOTE::: i may excede the cpu cycles on hand....
    //         i have 64 MHz / 44100 = ~1454 cycles ....
    //         i need a HACK
    // phase_pending_update_inc = // search(output_freq, incs);
    //         ((uint64_t) (((uint64_t) output_freq) << 32)) / 44000;

    // FIXME:: will not work on 32 bit interrupt systems
    //         i need os lock for this and i dont want to deal with RTOS...
    // atomic_uint_least64_t freq = (atomic_uint_least64_t) 0x001000000;
    // atomic_load(&freq);
    // if (output_freq <= 442) {
    //     phase_pending_update_inc =
    //         // ((uint64_t) (((uint64_t) output_freq) << 32)) / 44000;
    //         osc_oct_incs[0];
    //         // 0x28f5c28;
    // } else if (output_freq <= 635 && output_freq > 630) {
    //     phase_pending_update_inc =
    //         osc_oct_incs[5];
    // } else if (output_freq == 830) {
    //     phase_pending_update_inc =
    //         osc_oct_incs[11];
    // }
    // else {
    //     phase_pending_update_inc = incs[1];
    // }
    return osc_oct_incs[found];
}
