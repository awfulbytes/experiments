/*
 * * Interrupt routines
 */
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_dma.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_hal_tim.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(TIM_HandleTypeDef *timx);
