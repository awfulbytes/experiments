#include <assert.h>
#include <stdbool.h>
#include "nco.h"

struct nco dummy = {.mode=free,
                    .bandwidth={.free.min=100, .free.max=14000, .free.cv_raw_max=0xfff,
                                .tracking.min=220, .tracking.max=1661, .tracking.cv_raw_max=0xfff},
                    .distortion.level_range={.min=25, .max=129, .cv_raw_max=0xfff}};

void test_distortion_amount_mapper(void){
    uint16_t distortion_amount = map_uint(0xfff, &dummy.distortion.level_range);
    assert(distortion_amount == dummy.distortion.level_range.max);

    distortion_amount = map_uint(0x000, &dummy.distortion.level_range);
    assert(distortion_amount == dummy.distortion.level_range.min);
}

void test_pitch_free_mode_mapper(void){
    uint16_t note = map_uint(0xfff, &dummy.bandwidth.free);
    assert(note == dummy.bandwidth.free.max);

    note = map_uint(0x000, &dummy.bandwidth.free);
    assert(note == dummy.bandwidth.free.min);
}

void test_pitch_v_per_oct_mode_mapper(void){
    uint16_t note = map_uint(0xfff, &dummy.bandwidth.tracking);
    assert(note == dummy.bandwidth.tracking.max);

    note = map_uint(0x000, &dummy.bandwidth.tracking);
    assert(note == dummy.bandwidth.tracking.min);
}

int main(void){
    test_distortion_amount_mapper();
    test_pitch_free_mode_mapper();
    test_pitch_v_per_oct_mode_mapper();
}
