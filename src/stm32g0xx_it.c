/* #include "main.h" */
#include "gpio.h"
#include "stm32g0xx.h"
extern struct button ubButtonPress;
/* #include "stm32g0xx_ll_dac.h" */
/* #include "stm32g0xx_ll_exti.h" */
/* #include "stm32g0xx_it.h" */

void HardFault_Handler(void) {
    while (1) {
    }
}
void NMI_Handler(void) {/* while(1); */}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void EXTI4_15_IRQHandler(void) {
    if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_13) == SET) {
        LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_13);
        UserButton_Callback(&ubButtonPress);
        ubButtonPress.flag = 'i';
    }
}
