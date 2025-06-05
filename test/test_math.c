#include <assert.h>
#include "nco.h"

void test_distortion_amount_mapper(void){
    atomic_ushort distortion_amount = map_12b_to_distortion_amount(0xfff);
    assert(distortion_amount == 127);
    distortion_amount = map_12b_to_distortion_amount(0x000);
    assert(distortion_amount == 63);
}

void test_pitch_free_mode_mapper(void){
    atomic_ushort note = map_12b_to_hz(0xfff, free);
    assert(note == 16'000);
    note = map_12b_to_hz(0x000, free);
    assert(note == 110);
}

void test_pitch_v_per_oct_mode_mapper(void){
    atomic_ushort note = map_12b_to_hz(0xfff, v_per_octave);
    assert(note == 1661);
    note = map_12b_to_hz(0x000, v_per_octave);
    assert(note == 220);
}

int main(void){
    test_distortion_amount_mapper();
    test_pitch_free_mode_mapper();
    test_pitch_v_per_oct_mode_mapper();
}
