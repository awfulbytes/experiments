/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "gpio.h"
#include "dac.h"
#include "forms.h"
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_utils.h"
#include "timx.h"
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>

struct button ubButtonPress = {.state=0, .flag='D'};
void WaitForUserButtonPress(struct button *button);


void main() {
  struct timer tim6_settings = {0};
  sys_clock_config();
  tim6_settings = *timx_set(&tim6_settings);
  tim_init(&tim6_settings, 250, sawdn);

  struct dac dac_settings = {0};

  dma_config();
  gpio_init();
  dac_default_init(&dac_settings);
  dac_config(&dac_settings);
  dac_act(&dac_settings);
  WaitForUserButtonPress(&ubButtonPress);
  while (1) {
    if (ubButtonPress.flag == 'S') {
      switch (ubButtonPress.state) {
        case 0x0:
          dma_change_wave(sin, 250, &tim6_settings);
          LL_mDelay(400);
          LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
          break;
        case 0x1:
          if (dma_change_wave(sawdn, 200, &tim6_settings) == SUCCESS){
            LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
            LL_mDelay(200);
          }
          break;
        case 0x2:
          if(dma_change_wave(sawup, 100, &tim6_settings) == SUCCESS){
            LL_mDelay(500);
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

/**
  * @brief  Wait for User push-button press to start transfer.
  * @param  *button A button with a state and other information.
  * @retval None
  */
void WaitForUserButtonPress
(struct button *button) {
  while (button->state == 0) {
    LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
    LL_mDelay(LED_BLINK_SLOW);
  }
}

void UserButton_Callback
(struct button *button) {
  switch (button->state) {
    case 0x0:
      button->state = 0x1;
      break;
    case 0x1:
      button->state = 0x2;
      break;
    case 0x2:
      button->state = 0x0;
      break;
  }
}
