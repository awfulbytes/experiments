#include "stm32g0xx_ll_dac.h"
#include "stm32g0xx_ll_gpio.h"
#define DEBUGDAC
#include "gpio.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_dmamux.h"
#include "adc.h"
#include "forms.h"
#include "timx.h"
#include "dma.h"
#include "nco.h"
#include <stdint.h>

#include "src/ui.c"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

const uint_fast32_t dac1_clock = 198'000;
const uint_fast32_t adc1_clock = 44'000;

volatile uint16_t pitch0_cv = 0xff;
volatile uint16_t distortion_amount_cv = 0xff;
volatile uint16_t current_value_cv_0_pitch = 1;
volatile uint16_t current_value_cv_distortion_amount = 1;
volatile uint16_t pitch1_cv = 0xff;
volatile uint16_t current_value_cv_1_pitch = 1;

const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn, pulse};
volatile const uint16_t *wave_me_d = sine_wave;
volatile const uint16_t *wave_me_d2 = sine_wave;

struct nco l_osc = {.phase_accum = 0,
                    .phase_inc = 0x01'00'00'00,
                    .phase_pending_update_inc=0,
                    .phase_pending_update=false,
                    .phase_done_update=false,
                    .mode = free,
                    .bandwidth.free = {.min=100, .max=14'000,
                                       .cv_raw_max=0xfff},
                    .bandwidth.tracking = {.min=220, .max=1'661,
                                           .cv_raw_max=0xfff},
                    .distortion.amount=64,
                    .distortion.level_range={.min=23, .max=129,
                                             .cv_raw_max=0xfff},
                    .distortion.on=false,
                    .distortion.past_dante = entrance,
                    .distortion.dante=entrance};

struct nco r_osc = {.phase_accum = 0,
                    .phase_inc = 0x01'00'00'00,
                    .phase_pending_update_inc=0,
                    .phase_pending_update=false,
                    .phase_done_update=false,
                    .mode=v_per_octave,
                    .bandwidth.free = {.min=100, .max=14'000,
                                       .cv_raw_max=0xfff},
                    .bandwidth.tracking = {.min=220, .max=1'661,
                                           .cv_raw_max=0xfff},
                    .distortion.amount=64,
                    .distortion.level_range={.min=23, .max=129,
                                             .cv_raw_max=0xfff},
                    .distortion.on=false,
                    .distortion.past_dante = entrance,
                    .distortion.dante=entrance};

uint16_t dac_double_buff[256] = {0};
uint16_t dac_double_buff2[256] = {0};

struct timer tim6_settings = {.timx=TIM6, .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM6, .trigger_output=LL_TIM_TRGO_UPDATE};
struct timer tim7_settings = {.timx=TIM7, .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM7, .trigger_output=LL_TIM_TRGO_UPDATE};
struct timer tim2_settings = {.timx=TIM2, .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM2, .trigger_output=LL_TIM_TRGO_UPDATE};

struct dac dac_ch1_settings = {.dacx=DAC1, .channel=LL_DAC_CHANNEL_1,
                               .trg_src=LL_DAC_TRIG_EXT_TIM6_TRGO,
                               .bus_clk_abp=LL_APB1_GRP1_PERIPH_DAC1,
                               .timx_dac_irq=TIM6_DAC_LPTIM1_IRQn,
                               .dacx_settings={.OutputMode=LL_DAC_OUTPUT_MODE_NORMAL,
                                               .OutputBuffer=LL_DAC_OUTPUT_BUFFER_ENABLE,
                                               .OutputConnection=LL_DAC_OUTPUT_CONNECT_GPIO}};

struct dma dac_1_dma = {.dmax=DMA1, .channel=LL_DMA_CHANNEL_3, .data=(uint16_t *) dac_double_buff,
                        .chan=(DMA_Channel_TypeDef *)((uint32_t)DMA1 + (8 + 40)),
                        .dmax_settings={.PeriphRequest=LL_DMAMUX_REQ_DAC1_CH1,
                                        .Direction=LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
                                        .NbData=256,
                                        .Mode=LL_DMA_MODE_CIRCULAR,
                                        .Priority=0x00,
                                        .PeriphOrM2MSrcDataSize=LL_DMA_PDATAALIGN_HALFWORD,
                                        .MemoryOrM2MDstDataSize=LL_DMA_MDATAALIGN_HALFWORD,
                                        .MemoryOrM2MDstIncMode=LL_DMA_MEMORY_INCREMENT,
                                        .PeriphOrM2MSrcIncMode=LL_DMA_PERIPH_NOINCREMENT}};

struct dac dac_ch2_settings = {.dacx=DAC1, .channel=LL_DAC_CHANNEL_2,
                               .trg_src=LL_DAC_TRIG_EXT_TIM7_TRGO,
                               .bus_clk_abp=LL_APB1_GRP1_PERIPH_DAC1,
                               .timx_dac_irq=TIM7_LPTIM2_IRQn,
                               .dacx_settings={.OutputMode=LL_DAC_OUTPUT_MODE_NORMAL,
                                               .OutputBuffer=LL_DAC_OUTPUT_BUFFER_ENABLE,
                                               .OutputConnection=LL_DAC_OUTPUT_CONNECT_GPIO}};

struct dma dac_2_dma = {.dmax=DMA1, .channel=LL_DMA_CHANNEL_2, .data=(uint16_t *) dac_double_buff2,
                        .chan=(DMA_Channel_TypeDef *)((uint32_t)DMA1 + (8 + 20)),
                        .dmax_settings={.PeriphRequest=LL_DMAMUX_REQ_DAC1_CH2,
                                        .Direction=LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
                                        .NbData=256,
                                        .Mode=LL_DMA_MODE_CIRCULAR,
                                        .Priority=0x00,
                                        .PeriphOrM2MSrcDataSize=LL_DMA_PDATAALIGN_HALFWORD,
                                        .MemoryOrM2MDstDataSize=LL_DMA_MDATAALIGN_HALFWORD,
                                        .MemoryOrM2MDstIncMode=LL_DMA_MEMORY_INCREMENT,
                                        .PeriphOrM2MSrcIncMode=LL_DMA_PERIPH_NOINCREMENT}};

volatile uint16_t cv_raw_adc_inp[3] = {0};
struct adc adc_settings = {.adcx=ADC1,
                          .data = cv_raw_adc_inp,
                          .roof='D',
                          .settings={.Clock=LL_ADC_CLOCK_SYNC_PCLK_DIV1,
                                     .Resolution=LL_ADC_RESOLUTION_12B,
                                     .DataAlignment=LL_ADC_DATA_ALIGN_RIGHT,
                                     .LowPowerMode=LL_ADC_LP_MODE_NONE},

                          .reg_settings={.TriggerSource=LL_ADC_REG_TRIG_EXT_TIM2_TRGO,
                                         .ContinuousMode=LL_ADC_REG_CONV_SINGLE,
                                         .SequencerLength=LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS,
                                         .SequencerDiscont=LL_ADC_REG_SEQ_DISCONT_DISABLE,
                                         .DMATransfer=LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
                                         .Overrun=LL_ADC_REG_OVR_DATA_OVERWRITTEN},

                          .dma_channel=LL_DMA_CHANNEL_4,
                          .dmax=DMA1,
                          .dmax_settings={.PeriphRequest=LL_DMAMUX_REQ_ADC1,
                                          .Direction=LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
                                          .NbData=3,
                                          .Mode=LL_DMA_MODE_CIRCULAR,
                                          .PeriphOrM2MSrcDataSize=LL_DMA_PDATAALIGN_HALFWORD,
                                          .MemoryOrM2MDstDataSize=LL_DMA_MDATAALIGN_HALFWORD,
                                          .PeriphOrM2MSrcIncMode=LL_DMA_PERIPH_NOINCREMENT,
                                          .MemoryOrM2MDstIncMode=LL_DMA_MEMORY_INCREMENT}};

struct button wave_choise_dac1 = {.state=0, .flag='D',
                                 .exti={.exti_irqn=EXTI4_15_IRQn,
                                        .exti_line=LL_EXTI_LINE_10,
                                        .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                        .exti_line_conf=LL_EXTI_CONFIG_LINE10},
                                 .pin={.pin_id=LL_GPIO_PIN_10,
                                       .port_id=GPIOC,
                                       .mode=LL_GPIO_MODE_INPUT,
                                       .pull=LL_GPIO_PULL_UP}};
struct button wave_choise_dac2 = {.state=0, .flag='D',
                                 .exti={.exti_irqn=EXTI4_15_IRQn,
                                        .exti_line=LL_EXTI_LINE_11,
                                        .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                        .exti_line_conf=LL_EXTI_CONFIG_LINE11},
                                 .pin={.pin_id=LL_GPIO_PIN_11,
                                       .port_id=GPIOC,
                                       .mode=LL_GPIO_MODE_INPUT,
                                       .pull=LL_GPIO_PULL_UP}};
struct button distortion_choice = {.state=0, .flag='D',
                                   .exti={.exti_irqn=EXTI4_15_IRQn,
                                          .exti_line=LL_EXTI_LINE_12,
                                          .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                          .exti_line_conf=LL_EXTI_CONFIG_LINE12},
                                   .pin={.pin_id=LL_GPIO_PIN_12,
                                         .port_id=GPIOC,
                                         .mode=LL_GPIO_MODE_INPUT,
                                         .pull=LL_GPIO_PULL_DOWN}};

struct gpio dac1 = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_4, .mode=LL_GPIO_MODE_ANALOG};
struct gpio dac2 = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_5, .mode=LL_GPIO_MODE_ANALOG};
struct gpio pitch_0_cv = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_0, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};
struct gpio dist_amount_0_cv = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_1, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};

struct encoder osc_0_pd_enc = {.A = {.pin = {.port_id=GPIOC,
                                             .pin_id=LL_GPIO_PIN_4,
                                             .mode = LL_GPIO_MODE_INPUT,
                                             .pull = LL_GPIO_PULL_UP},
                                     .value=0, .flag='D',
                                     .it_settings = {.exti_irqn=EXTI4_15_IRQn,
                                                     .exti_line=LL_EXTI_LINE_4,
                                                     .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                                     .exti_line_conf=LL_EXTI_CONFIG_LINE4}},
                               .B.pin = {.port_id=GPIOC,
                                         .pin_id=LL_GPIO_PIN_5,
                                         .mode = LL_GPIO_MODE_INPUT,
                                         .pull = LL_GPIO_PULL_UP},
                               .B.value = 0,
                               .B.it_settings = { },
                               .B.flag = 'D',
                               .increment=0, .direction=false};

// nxt:: for some reason the PA_2 is busy!!
struct gpio pitch_1_cv = {.port_id=GPIOB, .pin_id=LL_GPIO_PIN_1,
                          .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};

#warning "need to implement second oscillator distortion CV input."
struct gpio dist_amount_1_cv = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_1,
                                .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};

#warning "need to get a second encoder for the second oscillator modulation."
struct encoder osc_1_pd_enc = {.A = {.pin = {.port_id=GPIOC,
                                             .pin_id=LL_GPIO_PIN_4,
                                             .mode = LL_GPIO_MODE_INPUT,
                                             .pull = LL_GPIO_PULL_UP},
                               .value=0, .flag='D',
                               .it_settings = {.exti_irqn=EXTI4_15_IRQn,
                                               .exti_line=LL_EXTI_LINE_4,
                                               .exti_port_conf=LL_EXTI_CONFIG_PORTC,
                                               .exti_line_conf=LL_EXTI_CONFIG_LINE4}},
                               .B.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_5,
                                         .mode = LL_GPIO_MODE_INPUT,
                                         .pull = LL_GPIO_PULL_UP},
                               .B.value = 0,
                               .B.it_settings = { },
                               .B.flag = 'D',
                               .increment=0, .direction=false};
