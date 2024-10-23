#include "int_handlers.h"
extern DMA_HandleTypeDef dmac1;


void NMI_Handler(void){}
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);

void SysTick_Handler(void){
    HAL_IncTick();
}
void DMA1_Channel2_3_IRQHandler(void){
    HAL_DMA_IRQHandler(&dmac1);
}
void EXTI4_15_IRQHandler(TIM_HandleTypeDef *htimx){
    HAL_TIM_IRQHandler(htimx);
}
