#include "ui.h"
#include "adc.h"
#include "dma.h"
#include "nco.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void);
