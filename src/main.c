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
  sys_clock_config();
  dma_config();
  tim_init();
  dac_config();
  dac_act();
  while (1) {
  }
}
