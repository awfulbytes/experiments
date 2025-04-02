#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "nco.h"
#include "forms.h"

extern struct nco l_osc, r_osc;
struct nco l_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00, .phase_pending_update=false, .phase_pending_update_inc=0};
struct nco r_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00, .phase_pending_update=false, .phase_pending_update_inc=0};
extern bool phase_pending_update;
uint16_t some[256];
uint16_t some2[256];
uint16_t full[256];
uint16_t sine_upd[256];

uint32_t master_clock = 44000;
uint32_t required_freq = 440;
uint16_t acc_bits = sizeof(l_osc.phase_accum) * 8;

void test_phase_increment(){
    uint32_t old_freq = ((l_osc.phase_inc * master_clock) >> acc_bits);
    uint32_t tmp = ((required_freq * (1<<16)) / master_clock) + 1;
    uint64_t new_incr = tmp << 16;
    uint64_t new_req = ((new_incr * master_clock) >> acc_bits);
    // assert(required_freq == new_req);
    assert((old_freq < new_req) && (l_osc.phase_inc < new_incr));
    printf("requested:\t%u[Hz]\n_old_shit:\t%u[Hz]\ngot-back:\t%lu[Hz]\n",
           required_freq, old_freq, new_req);
    // assert(phase_pending_update_inc == new_incr);
    l_osc.phase_pending_update_inc = osc_fine_incs[0xffe];
    uint64_t pending_freq = ((l_osc.phase_pending_update_inc * master_clock) >> acc_bits);
    assert(l_osc.phase_pending_update_inc > new_incr);
    printf("\n------- phase values -------\n");
    // printf("%lu\t%lu\n", (2000UL<<32)/44000, sizeof(phase_pending_update_inc) * 8);
    printf("pending_incr:\t%lx\n", l_osc.phase_pending_update_inc);
    printf("pen_max_freq:\t%lu\n", pending_freq);
    printf("o_g_val_incr:\t%lx\n", l_osc.phase_inc);
    printf("new_val_incr:\t%lx\n", new_incr);
}

void test_ping_pong(){
    uint32_t test_mapper = map_12b_to_hz(0x555);
    alter_wave_frequency(test_mapper);
    // assert(phase_inc == 0x01000000>>1);
    update_ping_pong_buff(sine_wave, some, 128, &l_osc);
    update_ping_pong_buff(sine_wave, some + 128, 128, &l_osc);
    update_ping_pong_buff(sine_wave, some2, 128, &l_osc);
    update_ping_pong_buff(sine_wave, some2 + 128, 128, &l_osc);
    update_ping_pong_buff(sine_wave, full, 256, &l_osc);
    for (int i=0; i<256; ++i) {
        // printf("%d\t\t %d   %d    %d\n", i, some[i], some2[i], full[i]);
        assert(some[i] == some2[i]);
        assert(full[i] == some2[i]);
        assert(some[i] != 0 && some[i] != 0xfff);
    }
    assert(some[10] != some2[16]);
    assert(sizeof(some)/sizeof(some[0]) == 2 * (sizeof(sine_wave)/sizeof(sine_wave[0])));
}

int main(void){
    test_phase_increment();
    test_ping_pong();
}
