#include <stdint.h>
#include <stdbool.h>
#include "reciprocal_division.h"
#pragma once

typedef enum freq_modes {free, tracking} freq_modes_e;
typedef volatile enum cyrcles {entrance, first, second, third, fourth, fifth, sixth, seventh, eighth, ninth,
                               // tenth, eleventh, twelve, thirteenth, fourteenth, fifteenth, seventeenthh, eighteenth, ninteenth,
                               hell} cyrcles_e;

struct limits {
    volatile uint16_t min;
    volatile uint16_t max;
    volatile uint16_t cv_raw_max;
};

struct phase_distortion{
    volatile bool     on;
    volatile uint16_t amount;
    volatile uint16_t amount_2;
    cyrcles_e         dante;
    cyrcles_e         past_dante;
    struct   limits   level_range;
    volatile uint32_t distortion_value;
};

struct bandwidth {
    struct limits free;
    struct limits tracking;
};

struct phasor {
    volatile uint64_t         pending_update_inc;
    volatile uint64_t         inc;
    volatile uint32_t         accum;
    volatile bool             pending_update;
    volatile bool             done_update;
};

typedef struct sanity {
    uint8_t report;
} sanity_t;
struct nco {
    struct   phasor           phase;
    volatile freq_modes_e     mode;
    volatile uint16_t         data_buff[128];
    struct   phase_distortion distortion;
    struct   bandwidth        bandwidth;
    sanity_t                  in_the_house;
    volatile const uint16_t  *curr_wave_ptr;
    volatile bool             on_scale;
};

#pragma GCC diagnostic ignored "-Wconversion"
void generate_half_signal(uint16_t                sector_length,
                          volatile struct nco     nco[static 1]);

void update_data_buff (const volatile uint32_t data[static 128],
                       uint32_t       buffer_sector[static 128],
                       uint16_t       sector_length);

uint16_t map_uint(uint16_t      adc_value,
                  volatile struct limits boundaries[static 1]);

bool stage_pending_inc(volatile uint16_t      note,
                       volatile struct   nco  nco[static 1],
                       const    uint32_t      sample_rate);

inline static uint64_t compute_nco_increment(uint16_t            note,
                                             const uint32_t sample_rate){
    int32_t tmp = ((note * (1<<16))/sample_rate);
    // tmp = U32DIVBY192000(tmp);
    return (tmp << 16);
}

inline static uint32_t compute_lut_index(uint32_t accum){

    return (uint32_t) (((uint64_t) accum * (1<<7))>>32);
}

#ifdef TEST
#include <stdio.h>
inline static void dbg_info_nco(volatile struct nco *nco,
                                uint32_t             fract,
                                int16_t              diff) {
    printf("\n");
    printf("fract:\t%d\n", fract);
    printf("interp-factor:\t%d\n", (diff * fract) >> 16);
    printf("increment:\t0x%lx\n", nco->phase.inc);
    printf("accumulat:\t0x%x\n", nco->phase.accum);
    printf("\n");
}
#endif // TEST
