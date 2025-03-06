#include "interrupter.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "cmsis_gcc.h"
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_hal_cortex.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dma.h"
#include "sysclk.c"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
volatile const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn};
volatile const uint16_t *wave_me_d = sine_wave;
volatile const uint16_t *wave_me_d2 = sine_wave;

void main() {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    struct button *wave_buttons[2] = {&wave_choise_dac1, &wave_choise_dac2};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    struct dma *dma_chans[2] = {&dac_1_dma, &dac_2_dma};
    sys_clock_config();
    gpio_init(wave_buttons, dacs, &pitch_0_cv);
    // FIXME::::::::::::::::::::::::
    // alter_wave_frequency(4095);
    update_ping_pong_buff(wave_me_d, dac_double_buff, 128);
    update_ping_pong_buff(wave_me_d, dac_double_buff + 128, 128);
    update_ping_pong_buff(wave_me_d2, dac_double_buff2, 128);
    update_ping_pong_buff(wave_me_d2, &dac_double_buff2[128], 128);
#if defined(DEBUG) || defined(DEBUGDAC)
    debug_tim2_pin31();
#else
#endif
    // osc_lut_inc_generator();
    for (int i=0; i < 2; i++) {
        dma_config(dma_chans[i]);
    }
    for (int i=0; i < 2; i++){
        tim_init(44000, timers[i]);
    }
    tim_init(44000, &tim2_settings);
    adc_init_settings(&pitch0cv_in);

    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }
    while (1) {
        // if (pitch0cv_in.roof == 0x69){
        //     pitch0cv_in.roof = 'D';
        // }
        if (phase_pending_update) {
            uint32_t note_to_hz = map_12b_to_hz(prev_value);
            // __disable_irq();
            phase_pending_update_inc = alter_wave_frequency(note_to_hz);
            // __enable_irq();
            // phase_pending_update_inc = (( (((output_freq) * (1UL << 32)) )) / 44000);
            // if (phase_pending_update_inc < 0x01000000) {
            //     phase_pending_update_inc = 0x01000000;
            // } else if (phase_pending_update_inc > 0x1aff11a) {
            //     phase_pending_update_inc = 0x1aff11a;
            // } else if (phase_pending_update_inc < 0x00100000){
            //     GPIOB->ODR ^= (1<<3);
            // }
            // else {
            //     phase_inc = phase_pending_update_inc;
            // }
            // if (phase_inc == 0x01000000) {
            //     phase_inc = 0x1220000;
            // } else if (phase_inc == 0x1220000) {
            //     phase_inc = 0x01'00'00'00;}
            // __enable_irq();
            // if (phase_pending_update_inc > (0x01'00'00'00 << 6)) {
            //     // phase_inc = 0x01'00'00'00;

            // } else if ((phase_pending_update_inc < 0x01'00'00'00)) {
            // }
            // else {
            // phase_inc = phase_pending_update_inc;
            //     // NOTE:: this seems to imply that the reason of hardlocking
            //     //        is due to 64 bit calculation... i may need to do this
            //     //        on native word sized double buffer struct for the 2 parts
            //     //        this is HARD though
            // }

            phase_pending_update = false;
            phase_done_update = true;
            // __enable_irq();
        }
        if (wave_choise_dac1.flag == 0x69) {
            // alter_wave_frequency(prev_value);
            // HACK:: make this the delay not the shitty mDelay!!
            // phase_inc <<= wave_choise_dac1.state + 1;
            // if ((0x01000000 << 3) < phase_inc) {
            //     phase_inc = 0x01000000;
            // }
            wave_button_callback(&wave_choise_dac1);
            wave_me_d = waves_bank[wave_choise_dac1.state];
        }
        if (wave_choise_dac2.flag == 0x69) {
            wave_button_callback(&wave_choise_dac2);
            wave_me_d2 = waves_bank[wave_choise_dac2.state];
            __disable_irq();
            update_ping_pong_buff(wave_me_d2, dac_double_buff2, 128);
            update_ping_pong_buff(wave_me_d2, &dac_double_buff2[128], 128);
            __enable_irq();
        }
    }
}
