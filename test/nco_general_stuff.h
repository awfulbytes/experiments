#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "nco.h"
#include "forms.h"

#define array_length(x)                 (sizeof x / sizeof x[0])

struct nco l_osc = {.phase = {.accum = 0, .inc = 0x01000000, .pending_update_inc=0, .pending_update=false,},
                    .mode=low,
                    .distortion.amount=64,
                    .distortion.on=true,
                    .distortion.distortion_value=0,
                    .bandwidth={.high.min=100, .high.max=14000, .high.cv_raw_max=0xfff,
                                .low.min=220, .low.max=1661, .low.cv_raw_max=0xfff},
                    .distortion.dante=9,
                    .distortion.level_range={.min=23, .max=129, .cv_raw_max=0xfff},
};
struct nco r_osc = {.phase = {.accum = 0, .inc = 0x01000000, .pending_update_inc=0, .pending_update=false,},
                    .mode=low,
                    .distortion.amount=64,
                    .distortion.on=true,
                    .distortion.distortion_value=0,
                    .distortion.dante=9,
};

uint32_t some[256];
uint32_t some2[256];
uint32_t full[256];
uint32_t sine_upd[256];

extern bool phase_pending_update;

uint16_t adc_data = 0x000;
uint16_t osc_max_current_mode;

uint_fast32_t master_clock = 198000;
uint32_t required_freq = 440;
uint16_t acc_bits = (sizeof l_osc.phase.accum) * 8;
