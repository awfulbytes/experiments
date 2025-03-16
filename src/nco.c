#include "nco.h"
// #include "stm32g071xx.h"
#include <stdint.h>
// #define TEST
// DDS variables (phase accumulator technique)
volatile uint32_t phase_accum = 0;
volatile uint64_t phase_inc = 0x001000000;
volatile bool phase_pending_update = false;
volatile uint64_t phase_pending_update_inc = 0x001000000;

void update_ping_pong_buff
(volatile const uint16_t data[static 128], uint16_t bufferSection[static 128], uint16_t sectionLength) {
    for (uint16_t i = 0; i < sectionLength; i++) {
        uint32_t index = (uint32_t) (((uint64_t) phase_accum * (1 << 7)) >> 32) % 128;
        // uint32_t index = phase_accum << 24;
        bufferSection[i] = (uint16_t) data[index];

        phase_accum += phase_inc;
#ifdef TEST
        #include "stdio.h"
        printf("accum: %d\n", phase_accum);
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

// DEPRECATED:: left for education
// static uint64_t search(uint32_t item, const uint32_t *arr){
//     int found = 0;
//     uint64_t curr = ((uint64_t) (((uint64_t)item) << 32)) / 44000;
//     for (int i=0; i<12; i++) {
//         if (curr == arr[i]){
//             found = i;
//             break;
//         }
//     }
//     return arr[found];
// }

uint32_t alter_wave_frequency (uint32_t output_freq){
    uint32_t found = 0;
    for (int i=0; i<12; i++) {
        if (output_freq > freqs[i] && output_freq <= freqs[i+1]){
            found = i;
            break;
        } else {continue;}
    }
    return osc_oct_incs[found];
}
