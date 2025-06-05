#include "ui.h"
#include "adc.h"
#include "dma.h"
#include "nco.h"

extern volatile uint16_t prev_value_cv_0_pitch;
extern volatile uint16_t prev_value_cv_distortion_amount;
extern volatile uint16_t pitch0_cv;
extern volatile uint16_t distortion_amount_cv;
extern struct nco l_osc, r_osc;
extern volatile const uint16_t *wave_me_d, *wave_me_d2;
extern atomic_ushort dac_double_buff[256], dac_double_buff2[256];
extern struct button wave_choise_dac1;
extern struct button wave_choise_dac2;
extern struct button distortion_choice;
extern struct encoder osc_0_pd_enc;

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void DMA1_Channel2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void);
