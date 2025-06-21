#include <stdint.h>

#pragma once
enum freq_modes {free, v_per_octave};
enum cyrcles {entrance, first, second, third, fourth, fifth, sixth, seventh,
              eighth, ninth, tenth, eleventh, twelve, thirteenth, fourteenth,
              fifteenth, seventeenthh, eighteenth, ninteenth, hell};

struct phase_distortion{
    volatile bool on;
    volatile unsigned int amount;
    enum cyrcles dante;
    enum cyrcles past_dante;
    volatile uint32_t distortion_value;
};

struct ping_pong_buff{
    // uint8_t size;
    uint16_t ping_buff[128];
};

struct nco {
    volatile uint64_t phase_pending_update_inc;
    volatile uint64_t phase_inc;
    volatile uint32_t phase_accum;
    enum freq_modes mode;
    struct ping_pong_buff data_buff;
    struct phase_distortion distortion;
    volatile bool phase_pending_update;
    volatile bool phase_done_update;
};

#pragma GCC diagnostic ignored "-Wconversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t sectionLength, struct nco nco[static 1]);

void update_data_buff (const uint16_t data[static 128],
                            unsigned int bufferSection[static 128],
                            uint16_t sectionLength);
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

__attribute__((pure)) unsigned int map_12b_to_distortion_amount(uint16_t value);
__attribute__((pure))unsigned int map_12b_to_hz(uint16_t adc_value, enum freq_modes pitch_modes);
bool stage_pending_inc(volatile uint16_t adc_raw_value, struct nco nco[static 1], const uint_fast32_t sample_rate);
void stage_modulated_signal_values(struct nco osc[static 1], unsigned int distortion_cv, volatile uint16_t pitch_cv, uint32_t master_clock);

__attribute__((pure, always_inline)) inline static uint64_t compute_nco_increment(unsigned int note, const uint_fast32_t sample_rate){
    int32_t tmp = ((note * (1<<16))/sample_rate);
    return (tmp<<16);
}


__attribute__((pure, always_inline)) inline static uint32_t compute_lut_index(struct nco nco[static 1]){
    // uint32_t index = (uint32_t) (((uint64_t) nco->phase_accum * (1 << 7)) >> 32) % 128;
    return (uint32_t) (((uint64_t) nco->phase_accum * (1<<7))>>32);
}

