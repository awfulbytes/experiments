#pragma once
#include <stdint.h>

struct overworld {
    volatile uint16_t pitch0_cv;
    volatile uint16_t current_value_cv_0_pitch;
    volatile uint16_t distortion_amount_cv;
    volatile uint16_t current_value_cv_distortion_amount;

    volatile uint16_t pitch1_cv;
    volatile uint16_t current_value_cv_1_pitch;

    const uint_fast32_t dac1_clock;
    const uint_fast32_t adc1_clock;
};

struct overseer {
    struct nco       *oscillators[2];
    struct overworld *universe_data;
};
