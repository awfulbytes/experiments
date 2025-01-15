#include "stm32g0xx_it.h"
#include "errors.h"
#include "stm32g0xx_ll_dma.h"
#include <stdint.h>
/* extern volatile uint16_t adc_value; */
/* extern volatile uint16_t prev_value; */
extern struct button ubButtonPress;
extern struct adc adc;

void HardFault_Handler(void) {
    while (1) {
    }
}
void NMI_Handler(void) {/* while(1); */}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void DMA1_Channel1_IRQHandler(void) {
    if (LL_DMA_IsActiveFlag_TE1(DMA1) == SET){
        LL_DMA_ClearFlag_TE1(DMA1);
        while(1) {
            LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
            LL_mDelay(1200);
            /* break; */
        }
    }
}

void EXTI4_15_IRQHandler(void) {
    if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_13) == SET) {
        LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_13);
        UserButton_Callback(&ubButtonPress);
        ubButtonPress.flag = 'i';
    }
}
