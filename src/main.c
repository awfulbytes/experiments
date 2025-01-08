/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>

struct button ubButtonPress = {.state=0, .flag='D'};


void main() {
  struct timer tim6_settings = {0};
  sys_clock_config();
  tim6_settings = *timx_set(&tim6_settings);
  tim_init(&tim6_settings, 250, sine_wave);

  struct dac dac_settings = {0};

  dma_config();
  gpio_init();
  dac_default_init(&dac_settings);
  dac_config(&dac_settings);
  dac_act(&dac_settings);
  WaitForUserButtonPress(&ubButtonPress);
  while (1) {
    if (ubButtonPress.flag == 0x69) {
      switch (ubButtonPress.state) {
        case 0x0:
          dma_change_wave(sine_wave, 250, &tim6_settings);
          /* LL_mDelay(400); */
          LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
          break;
        case 0x1:
          if (dma_change_wave(sawdn, 200, &tim6_settings) == SUCCESS){
            LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
            /* LL_mDelay(200); */
          }
          break;
        case 0x2:
          if(dma_change_wave(sawup, 100, &tim6_settings) == SUCCESS){
            /* LL_mDelay(500); */
            LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
          }
          break;
      }
      ubButtonPress.flag = 'D';
    } else {
      continue;
    }
  }
}
