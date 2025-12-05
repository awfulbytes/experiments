#include <stdint.h>
#pragma once
typedef enum freq_modes {high, low} freq_modes_e;
typedef enum cyrcles {entrance, first, second, third, fourth, fifth, sixth, seventh, eighth, ninth,
              // tenth, eleventh, twelve, thirteenth, fourteenth, fifteenth, seventeenthh, eighteenth, ninteenth,
              hell} cyrcles_e;

struct ping_pong_buff{
    // uint8_t size;
    uint16_t ping_buff[128];
};

struct limits {
    uint16_t min;
    uint16_t max;
    uint16_t cv_raw_max;
};

struct phase_distortion{
    volatile bool     on;
    volatile uint16_t amount;
    cyrcles_e         dante;
    cyrcles_e         past_dante;
    struct   limits   level_range;
    volatile uint32_t distortion_value;
};

struct bandwidth {
    struct limits high;
    struct limits low;
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
    freq_modes_e              mode;
    struct   ping_pong_buff   data_buff;
    struct   phase_distortion distortion;
    struct   bandwidth        bandwidth;
    sanity_t                  in_the_house;
};

#pragma GCC diagnostic ignored "-Wconversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t                sector_length,
                          struct   nco            nco[static 1]);

void update_data_buff (const uint16_t data[static 128],
                       uint16_t       buffer_sector[static 128],
                       uint16_t       sector_length);
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

/*
  uint16_t map_12b_to_distortion_amount(uint16_t       value,
                                        struct limits  level_range[static 1]);
 */
__attribute__((pure))
uint16_t map_uint(uint16_t      adc_value,
                  struct limits boundaries[static 1]);

bool stage_pending_inc(volatile uint16_t      note,
                       struct   nco           nco[static 1],
                       const    uint_fast32_t sample_rate);

__attribute__((pure, always_inline))
inline static uint64_t compute_nco_increment(uint16_t            note,
                                             const uint_fast32_t sample_rate){
    int32_t tmp = ((note * (1<<16))/sample_rate);
    return (tmp<<16);
}

__attribute__((pure, always_inline))
inline static uint32_t compute_lut_index(struct nco nco[static 1]){

    return (uint32_t) (((uint64_t) nco->phase.accum * (1<<7))>>32);
}

#ifdef TEST
#include <stdio.h>
inline static void dbg_info_nco(struct    nco *nco,
                                uint32_t       fract,
                                int16_t        diff) {
    printf("\n");
    printf("fract:\t%d\n", fract);
    printf("interp-factor:\t%d\n", (diff * fract) >> 16);
    printf("increment:\t0x%lx\n", nco->phase.inc);
    printf("accumulat:\t0x%x\n", nco->phase.accum);
    printf("\n");
}
#endif // TEST
