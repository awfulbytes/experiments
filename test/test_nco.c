#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include "nco.h"
#include "forms.h"
uint32_t phase_accum;
uint64_t phase_inc = 0x01000000;
bool phase_pending_update;
uint64_t phase_pending_update_value;
uint16_t some[256];
uint16_t some2[256];
uint16_t full[256];
uint16_t sine_upd[256];

uint32_t master_clock = 441000;
uint32_t required_freq = 440;
uint16_t acc_bits = sizeof(uint32_t) * 8;

void test_phase_increment(){
    // alter_wave_frequency(0xfff);
    // assert(phase_pending_update_value == ((phase_inc<<24) + 4095));
}

void test_ping_pong(){
    // assert(phase_inc == 0x01000000>>1);
    update_ping_pong_buff(sine_wave, some, 128);
    update_ping_pong_buff(sine_wave, &some[128], 128);
    update_ping_pong_buff(sine_wave, some2, 128);
    update_ping_pong_buff(sine_wave, &some2[128], 128);
    update_ping_pong_buff(sine_wave, full, 256);
    for (int i=0; i<256; i++) {
        assert(some[i] == some2[i]);
        assert(full[i] == some2[i]);
    }
    assert(sizeof(some) == 2 * sizeof(sine_wave));
}

int main(void){
    uint64_t req = (phase_inc * master_clock) / (1<<acc_bits);
    printf("%lx\n", req);
    printf("%lu\n", sizeof(req)*8);
    test_ping_pong();
    // test_phase_increment();
    alter_wave_frequency(0xfff);
    printf("max_pending_inc: \t%lx\n", phase_pending_update_value);   // * (1UL << 4));
    printf("phase inc to pending phase ratio:    %lf\n", (float)phase_inc/(float)phase_pending_update_value);
    // assert(phase_pending_update_value == phase_inc + 0xfff);
    // printf("phase_4_right:      %lx\n", (phase_pending_update_value) >> 4);
    alter_wave_frequency(0);
    printf("pending_phase_0: \t%lx\n", phase_pending_update_value);
    // alter_wave_frequency(0xfff);
    printf("phase inc to pending phase ratio:    %lf\n", (float)phase_inc/(float)phase_pending_update_value);
    // char *some;
    // some = "Hey there tests";
}
