#include "dac.h"
#include "main.h"

void dac_init_analog
(DAC_HandleTypeDef dac1, DMA_HandleTypeDef dma_dac1){
    /* gpio_dac1_c1 = {0}; */
  GPIO_InitTypeDef gpio_dac1_c1 = {0};
    if (dac1.Instance == DAC1) {
      __HAL_RCC_DAC1_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_SLEEP_ENABLE();

      gpio_dac1_c1.Pin = GPIO_PIN_4;
      gpio_dac1_c1.Mode = GPIO_MODE_ANALOG;
      gpio_dac1_c1.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(GPIOA, &gpio_dac1_c1);

      dma_dac1.Instance = DMA1_Channel3;
      dma_dac1.Init.Request = DMA_REQUEST_DAC1_CH1;
      dma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;

      dma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
      dma_dac1.Init.MemInc = DMA_PINC_ENABLE;

      dma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      dma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;

      dma_dac1.Init.Mode = DMA_CIRCULAR;
      dma_dac1.Init.Priority = DMA_PRIORITY_LOW;

      if(HAL_DMA_Init(&dma_dac1) != HAL_OK)
          handle_it();

      __HAL_LINKDMA(&dac1, DMA_Handle1, dma_dac1);
    }
}

// TODO:: learn not to freeze the periherals
void dac_deinit_analog(DAC_HandleTypeDef dac1){
    if(dac1.Instance == DAC1){
        __HAL_RCC_DAC1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);
        HAL_DMA_DeInit(dac1.DMA_Handle1);
    }

}
