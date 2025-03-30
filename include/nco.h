#include <stdint.h>
struct nco {
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

