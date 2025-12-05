#include <assert.h>
#include <stdbool.h>
#include "nco.h"

struct nco dummy = {.mode=high,
                    .bandwidth={.high.min=100, .high.max=14000, .high.cv_raw_max=0xfff,
                                .low.min=220, .low.max=1661, .low.cv_raw_max=0xfff},
                    .distortion.level_range={.min=25, .max=129, .cv_raw_max=0xfff}};

void test_distortion_amount_mapper(void){
    uint16_t distortion_amount = map_uint(0xfff, &dummy.distortion.level_range);
    assert(distortion_amount == dummy.distortion.level_range.max);

    distortion_amount = map_uint(0x000, &dummy.distortion.level_range);
    assert(distortion_amount == dummy.distortion.level_range.min);
}

void test_pitch_high_mode_mapper(void){
    uint16_t note = map_uint(0xfff, &dummy.bandwidth.high);
    assert(note == dummy.bandwidth.high.max);

    note = map_uint(0x000, &dummy.bandwidth.high);
    assert(note == dummy.bandwidth.high.min);
}

void test_pitch_v_per_oct_mode_mapper(void){
    uint16_t note = map_uint(0xfff, &dummy.bandwidth.low);
    assert(note == dummy.bandwidth.low.max);

    note = map_uint(0x000, &dummy.bandwidth.low);
    assert(note == dummy.bandwidth.low.min);
}

int main(void){
    test_distortion_amount_mapper();
    test_pitch_high_mode_mapper();
    test_pitch_v_per_oct_mode_mapper();
}
