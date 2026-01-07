#include "nco.h"
#include <stdint.h>
#pragma once

struct overworld {
    volatile uint16_t pitch_cv;
    volatile uint16_t current_value_cv_0_pitch;
    volatile uint16_t osc_0_cv_distortion_amount;
    volatile uint16_t osc_0_cv_2_distortion_amount;

    volatile uint16_t current_value_cv_1_pitch;
    volatile uint16_t osc_1_cv_distortion_ammount;

    const freq_modes_e  starting_synced_mode;
    const uint_fast32_t dac1_clock;
    const uint_fast32_t adc1_clock;
};

struct overseer {
    volatile struct nco       *oscillators[2];
    volatile struct nco       *selected;
    volatile struct overworld *universe_data;
    volatile bool              sync;
    volatile bool              phase_align;
};


static inline void stage_modulated_values(volatile struct nco osc[static 1],
                                          volatile struct   overworld *data);

void tune(struct overseer *overseer, uint8_t osc_idx);

void merge_signals_dual_dac_mode(volatile struct nco *o[2], uint32_t dual_buffer[128], uint8_t table_size);
void sync_fcw(volatile struct nco *o[2]);
