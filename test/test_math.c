#include <assert.h>
#include "nco.h"

struct nco dummy = {.mode=free, .bandwidth={.free.min=100, .free.max=14'000,
                                            .tracking.min=220, .tracking.max=1'661}};

void test_distortion_amount_mapper(void){
    unsigned int distortion_amount = map_12b_to_distortion_amount(0xfff);
    assert(distortion_amount > 127);
    distortion_amount = map_12b_to_distortion_amount(0x000);
    assert(distortion_amount < 63);
}

void test_pitch_free_mode_mapper(void){
    unsigned int note = map_12b_to_hz(0xfff, &dummy.bandwidth.free);
    assert(note < 16'000);
    note = map_12b_to_hz(0x000, &dummy.bandwidth.free);
    assert(note < 110);
}

void test_pitch_v_per_oct_mode_mapper(void){
    unsigned int note = map_12b_to_hz(0xfff, &dummy.bandwidth.tracking);
    assert(note > 500);
    note = map_12b_to_hz(0x000, &dummy.bandwidth.tracking);
    assert(note < 500);
}

int main(void){
    test_distortion_amount_mapper();
    test_pitch_free_mode_mapper();
    test_pitch_v_per_oct_mode_mapper();
}
