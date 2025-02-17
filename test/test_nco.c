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

uint32_t master_clock = 44100;
uint32_t required_freq = 444;
uint16_t acc_bits = sizeof(phase_accum) * 8;

void test_phase_increment(){
    uint32_t old_freq = ((phase_inc * master_clock) >> 32) + 1;
    uint64_t new_incr = (required_freq * (1UL<<acc_bits)) / master_clock;
    uint64_t new_req = ((new_incr * master_clock) >> acc_bits) + 1;
    assert(required_freq == new_req);
    assert((old_freq < new_req) && (phase_inc < new_incr));
    // printf("requested: %u[Hz]\tgot: %lu[Hz]\n", required_freq, new_req);
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
    test_ping_pong();
    test_phase_increment();
}
