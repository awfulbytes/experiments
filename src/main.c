/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "stm32g0xx_ll_utils.h"
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>
volatile uint16_t adc_value = 0xff;
volatile uint16_t prev_value = 1;
struct adc adc = {.data = &adc_value, .roof='D'};
struct button ubButtonPress = {.state=0, .flag='D'};
const uint16_t *waves_bank[] = {sine_wave, sawup, sawdn};


void main() {
    struct timer tim6_settings = {0};
    struct dac dac_settings = {0};

    sys_clock_config();
    tim6_settings = *timx_set(&tim6_settings);
    tim_init(&tim6_settings, 250, sine_wave);


    dma_config();
    gpio_init();
    ADC_DMA_Config(&adc);
    dac_default_init(&dac_settings);
    dac_config(&dac_settings);
    dac_act(&dac_settings);
    WaitForUserButtonPress(&ubButtonPress);
    while (1) {
        int32_t diff = prev_value - *adc.data;
        if ((((diff < 0) ? -diff : diff) > 30) && (*adc.data >= 3)){
            prev_value = map_12bit_osc_freq(*adc.data);
            Start_ADC_Conversion();
        }
        if (adc.roof == 0x69){
            alter_wave_frequency(prev_value, &tim6_settings);
        } else {
            continue;
        }
        if (ubButtonPress.flag == 0x69) {
            LL_mDelay(2);
            const uint16_t *tmp = waves_bank[ubButtonPress.state];
            dma_change_wave(tmp);
        }
        ubButtonPress.flag = 'D';
        adc.roof = 'D';
    }
}
