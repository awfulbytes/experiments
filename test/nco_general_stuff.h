#include <assert.h>
#include <stdint.h>
#include "nco.h"
#include "forms.h"

#define array_length(x)                 (sizeof x / sizeof x[0])

struct nco l_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=false, .phase_pending_update_inc=0,
                    .mode=v_per_octave,
                    .distortion.amount=64,
                    .distortion.on=true,
                    .distortion.distortion_value=0,
                    .bandwidth={.free.min=100, .free.max=14'000, .free.cv_raw_max=0xfff,
                                .tracking.min=220, .tracking.max=1'661, .tracking.cv_raw_max=0xfff},
                    .distortion.dante=9,
                    .distortion.level_range={.min=23, .max=129, .cv_raw_max=0xfff},
};
struct nco r_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=false, .phase_pending_update_inc=0,
                    .mode=v_per_octave,
                    .distortion.amount=64,
                    .distortion.on=true,
                    .distortion.distortion_value=0,
                    .distortion.dante=9,
};

uint16_t some[256];
uint16_t some2[256];
uint16_t full[256];
uint16_t sine_upd[256];

extern bool phase_pending_update;

uint16_t adc_data = 0x000;
uint16_t osc_max_current_mode;

constexpr uint_fast32_t master_clock = 198000;
uint32_t required_freq = 440;
uint16_t acc_bits = (sizeof l_osc.phase_accum) * 8;
