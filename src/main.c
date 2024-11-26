/* #include "include/stm32g0xx_hal_driver/Inc/stm32g0xx.h" */
/* #include <stdio.h> */
/* #include "system_stm32g0xx.c" */
#include "main.h"     // Do i need this to be here or i can put all of them in main.h.... mooooore files more kBits...
#include "dac.h"
#include "timx.h"
#include "sysclk.c"
#include <stddef.h>
#include <stdint.h>


void main() {
  timer_t tim6_settings;
  sys_clock_config();
  tim6_settings = timx_init(&tim6_settings);
  tim_init(&tim6_settings);
  dma_config();
  dac_config();
  dac_act();
  while (1) {
  }
}
