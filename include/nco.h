#include <stdatomic.h>
#include <string.h>
#include <stdint.h>

struct ping_pong_buff{
    uint8_t size;
    uint16_t ping_buff[128];
};

struct nco {
    // volatile struct ping_pong_buff dbuff;
    volatile bool phase_pending_update;
    struct ping_pong_buff data_buff;
    volatile uint32_t phase_accum;
    volatile uint64_t phase_inc;
    volatile uint64_t phase_pending_update_inc;
};

#pragma GCC diagnostic ignored "-Wconversion"
void generate_half_signal(volatile const uint16_t data[static 128],
                          uint16_t sectionLength, struct nco nco[static 1]);

void update_ping_pong_buff (const uint16_t data[static 128],
                            atomic_ushort bufferSection[static 128],
                            uint16_t sectionLength);
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
atomic_ushort map_12b_to_hz(uint16_t adc_value);

void compute_nco_increment(atomic_ushort note, struct nco *nco, const uint_fast16_t sample_rate);
void stage_pending_inc(volatile uint16_t adc_raw_value, struct nco *nco, const uint_fast16_t sample_rate);

inline static uint32_t compute_1cycle_lut_index(struct nco nco[static 1]){
    // uint32_t index = (uint32_t) (((uint64_t) nco->phase_accum * (1 << 7)) >> 32) % 128;
    return (uint32_t) (((uint64_t) nco->phase_accum * (1<<7))>>32);
}

