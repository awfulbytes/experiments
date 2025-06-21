#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "nco.h"
#include "forms.h"

extern struct nco l_osc, r_osc;
struct nco l_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=false, .phase_pending_update_inc=0,
                    .mode=v_per_octave,
                    .distortion.amount=64,
                    .distortion.on=true,
                    .distortion.distortion_value=0,
                    .distortion.dante=9,};
struct nco r_osc = {.phase_accum = 0, .phase_inc = 0x01'00'00'00,
                    .phase_pending_update=false, .phase_pending_update_inc=0,
                    .mode=v_per_octave,
                    .distortion.amount=64,
                    .distortion.on=true,
                    .distortion.distortion_value=0,
                    .distortion.dante=9,};
extern bool phase_pending_update;
uint16_t some[256];
uint16_t some2[256];
uint16_t full[256];
uint16_t sine_upd[256];

uint16_t adc_data = 0x000;

constexpr uint_fast32_t master_clock = 198000;
uint32_t required_freq = 440;
uint16_t acc_bits = (sizeof l_osc.phase_accum) * 8;
void emulate_adc_timer2_interrupt(void){
    adc_data = 0xfff;
    l_osc.phase_pending_update = true;
}

void test_phase_increment_pending_request(){
    emulate_adc_timer2_interrupt();
    if (l_osc.phase_pending_update){
        l_osc.phase_done_update = stage_pending_inc(adc_data, &l_osc, master_clock);
        l_osc.phase_pending_update = !l_osc.phase_done_update;
    }
    assert(l_osc.phase_pending_update == false);
    uint64_t new_req = ((l_osc.phase_pending_update_inc * master_clock) >> acc_bits) + 1;
    if (l_osc.mode == v_per_octave && adc_data == 0xfff){
        uint16_t osc_max_current_mode = map_12b_to_hz(0xfff, l_osc.mode);
        assert(new_req + 2 == osc_max_current_mode);
    }
    assert(adc_data == 0xfff);

    char *pretty_mode_printer = ((l_osc.mode == 0) ? "free": "v_per_octave");
    printf("pend-val:\t\t\t%lu[Hz]\noscillator-mode:\t%s",
           new_req, pretty_mode_printer);
    uint64_t pending_freq = ((l_osc.phase_pending_update_inc * master_clock) >> acc_bits);
    printf("\n------- phase values -------\n");
    printf("pending_incr:\t%lx\n", l_osc.phase_pending_update_inc);
    printf("pen_max_freq:\t%lu\n", pending_freq);
    printf("o_g_val_incr:\t%lx\n", l_osc.phase_inc);
}

void test_signal_generation_and_dac_buffer(){
    assert(l_osc.phase_done_update);
    l_osc.phase_inc = l_osc.phase_pending_update_inc;
    generate_half_signal(sine_wave, 128, &l_osc);
    update_data_buff(l_osc.data_buff.ping_buff, some, 128);
    update_data_buff(l_osc.data_buff.ping_buff, some + 128, 128);

    for (int i=0; i<256; ++i) {
        if (i >= 128){
            assert(some[i] == l_osc.data_buff.ping_buff[i-128]);
        }
        else if (i < 128){
            assert(some[i] == l_osc.data_buff.ping_buff[i]);
        }
        else {
            assert(some[i] == l_osc.data_buff.ping_buff[0]);
        }
    }
}

int main(void){
    test_phase_increment_pending_request();
    test_signal_generation_and_dac_buffer();
    assert(l_osc.phase_inc > l_osc.phase_pending_update_inc);
    assert(l_osc.distortion.distortion_value < l_osc.phase_inc);
    assert(some[10] != some[16]);
    assert(sizeof(some)/sizeof(some[0]) == 2 * (sizeof(sine_wave)/sizeof(sine_wave[0])));
}
