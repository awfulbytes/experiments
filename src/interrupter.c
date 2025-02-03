#include "interrupter.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_dma.h"
#include "sysclk.c"
#include <stdint.h>
const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn};
volatile uint16_t *wave_me_d = sine_wave;

void main() {
    struct timer *timers[2] = {&tim6_settings, &tim7_settings};
    struct button *wave_buttons[2] = {&wave_choise_dac1, &wave_choise_dac2};
    struct gpio *dacs[2] = {&dac1, &dac2};
    struct dac *dacs_settings[2] = {&dac_ch1_settings, &dac_ch2_settings};
    struct dma *dma_chans[2] = {&dac_1_dma, &dac_2_dma};
    sys_clock_config();
    gpio_init(wave_buttons, dacs, &pitch_0_cv);
#if defined(DEBUG) || defined(DEBUGDAC)
    debug_tim2_pin31();
#else
#endif
    for (int i=0; i < 2; i++) {
        dma_config(dma_chans[i]);
    }
    for (int i=0; i < 2; i++){
        tim_init(44100, timers[i]);
    }
    tim_init(44100, &tim2_settings);
    adc_init_settings(&pitch0cv_in);

    for (int i=0; i<2; i++){
        dac_config(dacs_settings[i]);
        dac_act(dacs_settings[i]);
    }
    // DEPRECATED:: see ./ui.c
    // WaitForUserButtonPress(&ubButtonPress);
    while (1) {
        int32_t diff = prev_value - *pitch0cv_in.data;
        if ((((diff < 0) ? -diff : diff) > 5)) {
            prev_value = map_12bit_osc_freq(*pitch0cv_in.data);
            LL_mDelay(3);
        }
        if (pitch0cv_in.roof == 0x69){
            alter_wave_frequency(prev_value, timers[0]);
        }
        if (wave_choise_dac1.flag == 0x69) {
            // HACK:: make this the delay not the shitty mDelay!!
            *wave_me_d = *waves_bank[wave_choise_dac1.state % WAVE_CTR];
            while (alter_wave_form(waves_bank[wave_choise_dac1.state % WAVE_CTR], dma_chans[0])
                   != SUCCESS){};
        }
        if (wave_choise_dac2.flag == 0x69) {
            while (alter_wave_form(waves_bank[wave_choise_dac2.state % WAVE_CTR], dma_chans[1])
                   != SUCCESS){};
        }
        wave_choise_dac1.flag = 'D';
        wave_choise_dac2.flag = 'D';
        pitch0cv_in.roof = 'D';
    }
}
