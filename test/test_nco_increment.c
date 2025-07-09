#include "../test/nco_general_stuff.h"


void emulate_adc_timer2_interrupt(void){
    adc_data = 0xfff;
    l_osc.phase_pending_update = true;
}

void test_phase_increment_pending_request(){
    emulate_adc_timer2_interrupt();
    if (l_osc.phase_pending_update){
        l_osc.phase_done_update = stage_pending_inc(adc_data,
                                                    &l_osc,
                                                    master_clock);
        l_osc.phase_pending_update = !l_osc.phase_done_update;
    } else
        assert(0);
}


int main(void){
    test_phase_increment_pending_request();
    assert(l_osc.phase_inc < l_osc.phase_pending_update_inc);
    assert(l_osc.phase_pending_update == false);
}
