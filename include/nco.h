#include <stdint.h>
#include <stdatomic.h>
#include <stdint.h>
enum freq_modes {free, v_per_octave};
enum cyrcles {entrance, first, second, third, fourth, fifth, sixth, seventh, eighth, ninth};

struct phase_distortion{
    volatile bool on;
    volatile uint8_t amount;
    enum cyrcles dante;
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
                            atomic_ushort bufferSection[static 128],
                            uint16_t sectionLength);
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
atomic_ushort map_12b_to_hz(uint16_t adc_value, enum freq_modes pitch_modes)__attribute__((pure));
bool stage_pending_inc(volatile uint16_t adc_raw_value, struct nco nco[static 1], const uint_fast32_t sample_rate);

__attribute__((pure, always_inline)) inline static uint64_t compute_nco_increment(atomic_ushort note, const uint_fast32_t sample_rate){
    atomic_uint_fast32_t tmp = ((note * (1<<16))/sample_rate);
    return (tmp<<16);
}


__attribute__((pure, always_inline)) inline static uint32_t compute_lut_index(struct nco nco[static 1]){
    // uint32_t index = (uint32_t) (((uint64_t) nco->phase_accum * (1 << 7)) >> 32) % 128;
    return (uint32_t) (((uint64_t) nco->phase_accum * (1<<7))>>32);
}

