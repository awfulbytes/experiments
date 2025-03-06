#include <assert.h>
#include <stdint.h>
// #include <assert.h>
#include <stdio.h>
#include "nco.h"
#include "forms.h"
uint32_t phase_accum;
// uint16_t phase_inc = 0x7ff;
// uint64_t phase_inc = 0x001301b0;
uint64_t phase_inc = 0x01000000;
// uint64_t phase_inc = 0x01011761;

bool phase_pending_update;
uint64_t phase_pending_update_inc = 0;
uint16_t some[256];
uint16_t some2[256];
uint16_t full[256];
uint16_t sine_upd[256];

uint32_t master_clock = 44000;
uint32_t required_freq = 440;
uint16_t acc_bits = sizeof(phase_accum) * 8;

void test_phase_increment(){
    uint32_t old_freq = ((phase_inc * master_clock) >> acc_bits);
    uint64_t new_incr = ((required_freq * (1UL<<acc_bits)) / master_clock);
    uint64_t new_req = ((new_incr * master_clock) >> acc_bits);
    // assert(required_freq == new_req);
    // assert((old_freq < new_req) && (phase_inc < new_incr));
    printf("requested:\t%u[Hz]\n_old_shit:\t%u[Hz]\ngot-back:\t%lu[Hz]\n",
           required_freq, old_freq, new_req);
    uint32_t test_mapper = alter_wave_frequency(830);
    // assert(phase_pending_update_inc == new_incr);
    // uint64_t test_mapper = map_12b_to_hz(0x7ff);
    phase_pending_update_inc = test_mapper;
    // alter_wave_frequency(test_mapper);
    printf("this is the mapper: %x", test_mapper);
    uint64_t pending_freq = ((phase_pending_update_inc * master_clock) >> acc_bits);
    // assert(phase_pending_update_inc > new_incr);
    printf("\n------- phase values -------\n");
    // printf("%lu\t%lu\n", (2000UL<<32)/44000, sizeof(phase_pending_update_inc) * 8);
    printf("pending_incr:\t%lx\n", phase_pending_update_inc);
    printf("pen_max_freq:\t%lu\n", pending_freq);
    printf("o_g_val_incr:\t%lx\n", phase_inc);
    printf("new_val_incr:\t%lx\n", new_incr);
    uint32_t differ = phase_pending_update_inc - phase_inc;
    printf("diff-PI:\t\t%d\n", differ);
}

void test_ping_pong(){
    uint32_t test_mapper = map_12b_to_hz(4095);
    alter_wave_frequency(test_mapper);
    // assert(phase_inc == 0x01000000>>1);
    update_ping_pong_buff(sine_wave, some, 128);
    update_ping_pong_buff(sine_wave, &some[128], 128);
    update_ping_pong_buff(sine_wave, some2, 128);
    update_ping_pong_buff(sine_wave, &some2[128], 128);
    update_ping_pong_buff(sine_wave, full, 256);
    for (int i=0; i<256; i++) {
        // printf("%d\t\t %d   %d    %d\n", i, some[i], some2[i], full[i]);
        assert(some[i] == some2[i]);
        assert(full[i] == some2[i]);
    }
    assert(sizeof(some)/sizeof(some[0]) == 2 * (sizeof(sine_wave)/sizeof(sine_wave[0])));
}

int main(void){
    test_phase_increment();
    test_ping_pong();
}
