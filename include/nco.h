#include <stdint.h>

struct ping_pong_buff{
    uint8_t size;
    uint16_t ping_buff[128];
    uint16_t pong_buff[128];
};

struct nco {
    volatile struct ping_pong_buff dbuff;
    volatile uint32_t phase_accum;
    volatile uint64_t phase_inc;
    volatile bool phase_pending_update;
    volatile uint64_t phase_pending_update_inc;
};

void update_ping_pong_buff
(const volatile uint16_t data[static 128], uint16_t bufferSection[static 128], uint16_t sectionLength, struct nco *nco);
uint32_t map_12b_to_hz(uint16_t adc_value);
uint32_t alter_wave_frequency (uint32_t output_freq);
void osc_lut_inc_generator();


inline static uint32_t compute_1cycle_lut_index(struct nco nco[static 1]){
    // uint32_t index = (uint32_t) (((uint64_t) nco->phase_accum * (1 << 7)) >> 32) % 128;
    return (uint32_t) (((uint64_t) nco->phase_accum * (1<<7))>>32);
}
