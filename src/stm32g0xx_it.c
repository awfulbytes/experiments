#include "main.h"
/* #include "stm32g0xx_ll_dac.h" */
/* #include "stm32g0xx_ll_exti.h" */
/* #include "stm32g0xx_it.h" */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void) {
    /* while(1); */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void) {
  while (1) {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void) {
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_15_IRQn 0 */

  /* USER CODE END EXTI4_15_IRQn 0 */
  if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_13) != RESET)
  {
    LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_13);
    /* USER CODE BEGIN LL_EXTI_LINE_13_FALLING */
    /* Call interruption treatment function */
    UserButton_Callback();
    /* USER CODE END LL_EXTI_LINE_13_FALLING */
  }
  /* USER CODE BEGIN EXTI4_15_IRQn 1 */

  /* USER CODE END EXTI4_15_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  This function handles DAC1 interrupt.
  * @param  None
  * @retval None
  */
void TIM6_DAC_LPTIM1_IRQHandler(void)
{
  /* Check whether DAC channel1 underrun caused the DAC interruption */
  if(LL_DAC_IsActiveFlag_DMAUDR1(DAC1) != 0)
  {
    /* Clear flag DAC channel1 underrun */
    LL_DAC_ClearFlag_DMAUDR1(DAC1);

    /* Call interruption treatment function */
    DacUnderrunError_Callback();
  }
}


/* USER CODE END 1 */

/* void NMI_Handler(void){ while (1); } */
/* void HardFault_Handler(void){ while (1); } */
/* void SVC_Handler(void){ while (1); } */
/* void PendSV_Handler(void){ while (1); } */

/* void SysTick_Handler(void){ */
/*     HAL_IncTick(); */
/* } */
/* void DMA1_Channel2_3_IRQHandler(void){ */
/*     /\* HAL_DMA_IRQHandler(&dmac1); *\/ */
/* } */
