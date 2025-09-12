#include <stdint.h>

#pragma once
typedef enum freq_modes {free, v_per_octave} freq_modes_e;
typedef enum cyrcles {entrance, first, second, third, fourth, fifth, sixth, seventh, eighth, ninth,
              // tenth, eleventh, twelve, thirteenth, fourteenth, fifteenth, seventeenthh, eighteenth, ninteenth,
              hell} cyrcles_e;

struct ping_pong_buff{
    // uint8_t size;
    uint16_t ping_buff[128];
};

struct limits {
    uint8_t  min;
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
    struct limits free;
    struct limits tracking;
};

struct nco {
    volatile uint64_t         phase_pending_update_inc;
    volatile uint64_t         phase_inc;
    volatile uint32_t         phase_accum;
    freq_modes_e              mode;
    struct   ping_pong_buff   data_buff;
    struct   phase_distortion distortion;
    struct   bandwidth        bandwidth;
    volatile bool             phase_pending_update;
    volatile bool             phase_done_update;
};

#pragma GCC diagnostic ignored "-Wconversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t                sector_length,
                          struct   nco            nco[static 1]);

void update_data_buff (const uint16_t data[static 128],
                       uint16_t       buffer_sector[static 128],
                       uint16_t       sector_length);
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

__attribute__((pure))
uint16_t map_12b_to_distortion_amount(uint16_t       value,
                                      struct limits  level_range[static 1]);
__attribute__((pure))
uint16_t map_12b_to_range(uint16_t      adc_value,
                          struct limits boundaries[static 1]);

bool stage_pending_inc(volatile uint16_t      adc_raw_value,
                       struct   nco           nco[static 1],
                       const    uint_fast32_t sample_rate);

void stage_modulated_signal_values(struct   nco      osc[static 1],
                                   uint16_t          distortion_cv,
                                   volatile uint16_t pitch_cv,
                                   uint32_t          master_clock);

__attribute__((pure, always_inline))
inline static uint64_t compute_nco_increment(uint16_t            note,
                                             const uint_fast32_t sample_rate){
    int32_t tmp = ((note * (1<<16))/sample_rate);
    return (tmp<<16);
}


__attribute__((pure, always_inline))
inline static uint32_t compute_lut_index(struct nco nco[static 1]){

    return (uint32_t) (((uint64_t) nco->phase_accum * (1<<7))>>32);
}

