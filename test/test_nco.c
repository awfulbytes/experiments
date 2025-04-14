#include <assert.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include "nco.h"
#include "forms.h"

extern struct nco l_osc, r_osc;
struct nco l_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=false, .phase_pending_update_inc=0,
                    .mode=single_octave};
struct nco r_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=false, .phase_pending_update_inc=0,
                    .mode=single_octave};
extern bool phase_pending_update;
atomic_ushort some[256];
atomic_ushort some2[256];
atomic_ushort full[256];
atomic_ushort sine_upd[256];

uint16_t adc_data = 0x000;

constexpr uint_fast32_t master_clock = 198000;
uint32_t required_freq = 440;
uint16_t acc_bits = sizeof(l_osc.phase_accum) * 8;
void emulate_adc_timer2_interrupt(void){
    adc_data = 0xfff;
    l_osc.phase_pending_update = true;
}

void test_phase_increment(){
    emulate_adc_timer2_interrupt();
    if (l_osc.phase_pending_update){
        stage_pending_inc(adc_data, &l_osc, master_clock);
    }
    assert(l_osc.phase_pending_update == false);
    assert(adc_data == 0xfff);
    uint64_t new_req = ((l_osc.phase_pending_update_inc * master_clock) >> acc_bits);

    char *pretty_mode_printer = ((l_osc.mode == 0) ? "free": "single_octave");
    printf("requested:\t\t\t%u[Hz]\ngot-back:\t\t\t%lu[Hz]\noscillator-mode:\t%s",
           required_freq, new_req, pretty_mode_printer);
    uint64_t pending_freq = ((l_osc.phase_pending_update_inc * master_clock) >> acc_bits);
    printf("\n------- phase values -------\n");
    printf("pending_incr:\t%lx\n", l_osc.phase_pending_update_inc);
    printf("pen_max_freq:\t%lu\n", pending_freq);
    printf("o_g_val_incr:\t%lx\n", l_osc.phase_inc);
}

void test_ping_pong(){
    l_osc.phase_inc = l_osc.phase_pending_update_inc;
    generate_half_signal(sine_wave, 128, &l_osc);
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
            // assert(l_osc.data_buff.ping_buff[i] == 0);
            assert(some[i] == l_osc.data_buff.ping_buff[0]);
        }
        // printf("data-big-buff:\t%d\n", some[i]);
    }

    assert(some[10] != some[16]);
    assert(sizeof(some)/sizeof(some[0]) == 2 * (sizeof(sine_wave)/sizeof(sine_wave[0])));
}

int main(void){
    test_phase_increment();
    test_ping_pong();
}
