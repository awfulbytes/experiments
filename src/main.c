/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "gpio.h"
#include "dac.h"
#include "timx.h"
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>

__IO uint8_t ubButtonPress = 0;
void UserButton_Init();

void main() {
  struct timer tim6_settings;
  sys_clock_config();
  tim6_settings = *timx_set(&tim6_settings);
  tim_init(&tim6_settings, 250);
  /* UserButton_Init(); */

  dma_config();
  gpio_init();
  dac_config();
  dac_act();
  while (1) {
    WaitForUserButtonPress();
    if (ubButtonPress == 1) {
      LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
      LL_mDelay(LED_BLINK_SLOW);
    }
  }
}

/**
  * @brief  Configures User push-button in EXTI Line Mode.
  * @param  None
  * @retval None
  */
/* void UserButton_Init */
/* (void) { */
/*   /\* Enable the BUTTON Clock *\/ */
/*   USER_BUTTON_GPIO_CLK_ENABLE(); */
/*   /\* Configure GPIO for BUTTON *\/ */
/*   LL_GPIO_SetPinMode(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_MODE_INPUT); */
/*   LL_GPIO_SetPinPull(USER_BUTTON_GPIO_PORT, USER_BUTTON_PIN, LL_GPIO_PULL_NO); */
/*   /\* Connect External Line to the GPIO *\/ */
/*   USER_BUTTON_SYSCFG_SET_EXTI(); */
/*   /\* Enable a rising trigger EXTI line 13 Interrupt *\/ */
/*   USER_BUTTON_EXTI_LINE_ENABLE(); */
/*   USER_BUTTON_EXTI_FALLING_TRIG_ENABLE(); */
/*   /\* Configure NVIC for USER_BUTTON_EXTI_IRQn *\/ */
/* } */


/**
  * @brief  Wait for User push-button press to start transfer.
  * @param  None
  * @retval None
  */
void WaitForUserButtonPress(void)
{
  ubButtonPress = 0;
  while (ubButtonPress == 0)
  {
    LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
    LL_mDelay(LED_BLINK_SLOW);
  }
}


/**
  * @brief  Function to manage IRQ Handler
  * @param  None
  * @retval None
  */
void UserButton_Callback(void)
{
  /* On the first press on user button, update only user button variable      */
  /* to manage waiting function.                                              */
  if(ubButtonPress == 0)
  {
    /* Update User push-button variable : to be checked in waiting loop in main program */
    ubButtonPress = 1;
  } else
    ubButtonPress = 0;
}
