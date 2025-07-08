#include "../test/nco_general_stuff.h"

// #include "../test/test_nco.c"
void emulate_frequency_change(struct nco nco[static 1]){
    nco->phase_pending_update_inc = nco->phase_inc << 2;
    nco->phase_pending_update = false;
    nco->phase_done_update = true;
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
    emulate_frequency_change(&l_osc);
    test_signal_generation_and_dac_buffer();

    assert(l_osc.phase_inc == l_osc.phase_pending_update_inc);
    assert(some[10] != some[16]);
    assert(array_length(some) == 2 * array_length(sine_wave));
}
