/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "interrupter.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "stm32g0xx.h"
#include "stm32g0xx_ll_tim.h"
#include "sysclk.c"
volatile uint16_t adc_value = 0xff;
volatile uint16_t prev_value = 1;

struct timer tim6_settings = {.timx=TIM6, .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM6, .trigger_output=LL_TIM_TRGO_UPDATE};
struct dac dac_ch1_settings = {0};
struct adc adcx = {.data = &adc_value, .channel=0, .roof='D', .settings={0}, .reg_settings={0}};
struct button wave_choise_but = {.state=0, .flag='D', .exti={0}, .pin={0}};
struct gpio led, adc_pin = {0};
const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn};


void main() {
    sys_clock_config();
    gpio_init(&wave_choise_but, &led, &adc_pin);
    dma_config();
    tim_init(250);
    adc_init_settings(&adcx);


    dac_ch1_init(&dac_ch1_settings);
    dac_config(&dac_ch1_settings);
    dac_act(&dac_ch1_settings);
    // DEPRECATED:: see ./ui.c
    // WaitForUserButtonPress(&ubButtonPress);
    while (1) {
        int32_t diff = prev_value - *adcx.data;
        if ((((diff < 0) ? -diff : diff) > 3)){
            prev_value = map_12bit_osc_freq(*adcx.data);
            start_adc_conversion();
        }
        if (adcx.roof == 0x69){
            alter_wave_frequency(prev_value, &tim6_settings);
        }
        if (wave_choise_but.flag == 0x69) {
            // HACK:: make this the delay not the shitty mDelay!!
            while (dma_change_wave(waves_bank[wave_choise_but.state % WAVE_CTR]) != SUCCESS){
            };
        }
        wave_choise_but.flag = 'D';
        adcx.roof = 'D';
    }
}
