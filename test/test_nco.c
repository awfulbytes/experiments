#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include "nco.h"
#include "forms.h"

extern struct nco l_osc, r_osc;
struct nco l_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00, .phase_pending_update=false, .phase_pending_update_inc=0};
struct nco r_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00, .phase_pending_update=false, .phase_pending_update_inc=0};
extern bool phase_pending_update;
atomic_ushort some[256];
atomic_ushort some2[256];
atomic_ushort full[256];
atomic_ushort sine_upd[256];

uint32_t master_clock = 48000;
uint32_t required_freq = 440;
uint16_t acc_bits = sizeof(l_osc.phase_accum) * 8;

void test_phase_increment(){
    uint32_t old_freq = ((l_osc.phase_inc * master_clock) >> acc_bits);
    uint32_t tmp = ((required_freq * (1<<16)) / master_clock) + 1;
    uint64_t new_incr = tmp << 16;
    uint64_t new_req = ((new_incr * master_clock) >> acc_bits);
    // assert(required_freq == new_req);
    // assert((old_freq < new_req) && (l_osc.phase_inc < new_incr));
    printf("requested:\t%u[Hz]\n_old_shit:\t%u[Hz]\ngot-back:\t%lu[Hz]\n",
           required_freq, old_freq, new_req);
    // assert(phase_pending_update_inc == new_incr);
    l_osc.phase_pending_update_inc = new_incr;
    uint64_t pending_freq = ((l_osc.phase_pending_update_inc * master_clock) >> acc_bits);
    assert(l_osc.phase_pending_update_inc == new_incr);
    printf("\n------- phase values -------\n");
    // printf("%lu\t%lu\n", (2000UL<<32)/44000, sizeof(phase_pending_update_inc) * 8);
    printf("pending_incr:\t%lx\n", l_osc.phase_pending_update_inc);
    printf("pen_max_freq:\t%lu\n", pending_freq);
    printf("o_g_val_incr:\t%lx\n", l_osc.phase_inc);
    printf("new_val_incr:\t%lx\n", new_incr);
}

void test_ping_pong(){
    // assert(phase_inc == 0x01000000>>1);
    l_osc.phase_inc = l_osc.phase_pending_update_inc;
    generate_half_signal(sine_wave, 128, &l_osc);
    // update_ping_pong_buff(l_osc.data_buff.pong_buff, (some + 128), 128);
    // update_ping_pong_buff(l_osc.data_buff.ping_buff, some, 128);
    // memcpy(some, l_osc.data_buff.ping_buff, 128 * sizeof(uint16_t));
    update_ping_pong_buff(l_osc.data_buff.ping_buff, some, 128);
    update_ping_pong_buff(l_osc.data_buff.ping_buff, some + 128, 128);

    for (int i=0; i<256; ++i) {
        if (i > 128)
            assert(some[i] == l_osc.data_buff.ping_buff[i-128]);
        else if (i < 128){
            assert(some[i] == l_osc.data_buff.ping_buff[i]);
            // printf("b:\t %u\n", l_osc.data_buff.ping_buff[i]);
        }
        else {
            assert(l_osc.data_buff.ping_buff[i] == 0);
            assert(some[i] == l_osc.data_buff.ping_buff[0]);
        }
        // printf("data-big-buff:\t%d\n", some[i]);
    }

    // assert(some[10] != some2[16]);
    assert(sizeof(some)/sizeof(some[0]) == 2 * (sizeof(sine_wave)/sizeof(sine_wave[0])));
}

int main(void){
    test_phase_increment();
    test_ping_pong();
}
