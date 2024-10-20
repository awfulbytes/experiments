#include "int_handlers.h"


void NMI_Handler(void){}
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);

void SysTick_Handler(void){
    HAL_IncTick();
}
void DMA1_Channel2_3_IRQHandler(DMA_HandleTypeDef *hdmax){
    HAL_DMA_IRQHandler(hdmax);
}
void EXTI4_15_IRQHandler(TIM_HandleTypeDef *htimx){
    HAL_TIM_IRQHandler(htimx);
}
