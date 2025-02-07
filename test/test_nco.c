#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "../include/nco.h"
#include "../include/forms.h"
uint32_t phase_accum;
uint64_t phase_inc = 0x01000000;
bool phase_pending_update;
uint64_t phase_pending_update_value;
uint16_t some[256];
uint16_t some2[256];

void test_accumulator(){
    assert(phase_inc == 0x01000000);
    update_ping_pong_buff(sine_wave, some, 128);
    // assert(phase_pending_update_value == phase_inc);
}

int main(void){
    test_accumulator();
    alter_wave_frequency(4095/2);
    printf("mult_4_right:       %lx\n", phase_pending_update_value * (1UL << 4));
    printf("accumulator:         %x\n", phase_accum);
    printf("phase_4_right:      %lx\n", (phase_pending_update_value) >> 4);
    alter_wave_frequency(0);
    printf("phase_0:            %lx\n", phase_pending_update_value);
    alter_wave_frequency(0xfff);
    printf("phase_max_(no_op):  %lx\n", phase_pending_update_value);
    // char *some;
    // some = "Hey there tests";
}
