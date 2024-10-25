#include "stm32g0xx_it.h"
#include "stm32g0xx_hal_dma.h"
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
void EXTI4_15_IRQHandler(void){
}
