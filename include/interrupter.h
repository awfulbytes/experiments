#include "gpio.h"
#include"flash.h"
#include "adc.h"
#include "forms.h"
#include "timx.h"
#include "dma.h"
#include "nco.h"
#include "ui.h"
#include "overseer.h"


#define cv_init       0xff
#define dac_clk       144000
#define adc_clk       48000
#define starting_mode free
#define max_cv        0x7fff

struct overworld world = {
    .pitch_cv                           = cv_init,
    ._cv_0_pitch                        = cv_init,
    .osc_0_cv_distortion_amount         = cv_init,
    .tunner_pitch_raw_d                 = cv_init,
    ._cv_1_pitch                        = cv_init,
    .osc_1_cv_distortion_ammount        = cv_init,
    .dac1_clock = dac_clk,
    .adc1_clock = adc_clk,
};
volatile uint8_t cnt_adc_cycles = 0;

volatile const uint16_t *waves_bank[WAVE_CTR] = {sine_wave, sawup, sawdn, pulse};

struct nco l_osc = {.phase = {.accum = 0,
                              .inc = 0x01000000,
                              .pending_update_inc=0,
                              .pending_update=false,
                              .done_update=false,},
                    .in_the_house.report = 0,
                    .tempered = { .first_fundamental    = 0x7fff,
                                  .last_fundamental     = 0x7fff,
                                  .quantized_et         = 0x7fff,
                                  ._semi_tones_in_range = 0,
                                  .semitone             = 0,
                                  .cv_semitones         = 0,
                                  .drift_error          = 0,
                                  .last_to_first_ratio  = 0,
                                  .oct = {.shift=false, .change=false, .span=1, .unit=12, .jump = 0},
                                  .flag = 0x0,
                                  .rec = false,
                                  .rec_history = false,
                                  .just_reced = false,
                                  .type = none,
                                  .type_change = false,
                                  .absolute = {.min = 85, .max = 12000},
                                  .tuner_bounds = {.min=85, .max=3000, .cv_raw_max = 0x7fff},
                                  .mutable_bounds = {.min=85, .max=12000, .cv_raw_max = 0x7fff},
                    },
                    .curr_wave_ptr = sine_wave,
                    .mode = starting_mode,
                    .bandwidth.free = {.min=4000, .max=12000,
                                       .cv_raw_max=0x7fff},
                    .bandwidth.tracking = {.min=20, .max=5764,
                                           .cv_raw_max=0x7fff},
                    .distortion.amount=64,
                    .distortion.level_range={.min=1, .max=127,
                                             .cv_raw_max=0x7fff},
                    .distortion.on=false,};

struct nco r_osc = {.phase = {.accum = 0,
                              .inc = 0x01000000,
                              .pending_update_inc=0,
                              .pending_update=false,
                              .done_update=false,},
                    .in_the_house.report = 1,
                    .tempered = { .first_fundamental = 0x7fff,
                                  .last_fundamental = 0x7fff,
                                  .oct.span = 1,
                                  .rec = false,
                                  .type = none,
                                  .absolute = {.min = 0, .max = 0},
                                  .tuner_bounds = {.min=80, .max=12000, .cv_raw_max=0x7fff},
                                  .mutable_bounds = {.min=80, .max=12000, .cv_raw_max = 0x7fff},
                    },
                    .curr_wave_ptr = sine_wave,
                    .mode=starting_mode,
                    .bandwidth.free = {.min=4000, .max=12000,
                                       .cv_raw_max=0x7fff},
                    .bandwidth.tracking = {.min=20, .max=5764,
                                           .cv_raw_max=0x7fff},
                    .distortion.amount=64,
                    .distortion.level_range={.min=1, .max=127,
                                             .cv_raw_max=0x7fff},
                    .distortion.on=false,};

volatile uint32_t dac_double_buff2[256] = {0};

struct timer tim6_settings = {.id=TIM6,
                              .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM6,
                              .trigger_output=LL_TIM_TRGO_UPDATE,
                              .irq_settings={.nvic_id=TIM6_DAC_LPTIM1_IRQn, .priority=0x00}};
struct timer tim7_settings = {.id=TIM7,
                              .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM7,
                              .trigger_output=LL_TIM_TRGO_UPDATE,
                              .irq_settings={.nvic_id=TIM7_LPTIM2_IRQn, .priority=0xC0}};
struct timer tim2_settings = {.id=TIM2,
                              .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM2,
                              .trigger_output=LL_TIM_TRGO_UPDATE,
                              .irq_settings={.nvic_id=TIM2_IRQn, .priority=0x00}};
struct timer tim3_settings = {.id=TIM3,
                              .apb_clock_reg=LL_APB1_GRP1_PERIPH_TIM3,
                              .trigger_output=LL_TIM_TRGO_UPDATE,
                              .irq_settings={.nvic_id=TIM3_IRQn, .priority=0x80}};

struct dac dac_ch1_settings = {.dacx=DAC1, .channel=LL_DAC_CHANNEL_1,
                               .trg_src=LL_DAC_TRIG_EXT_TIM6_TRGO,
                               .bus_clk_abp=LL_APB1_GRP1_PERIPH_DAC1,
                               .timx_dac_irq=TIM6_DAC_LPTIM1_IRQn,
                               .dacx_settings={.OutputMode=LL_DAC_OUTPUT_MODE_NORMAL,
                                               .OutputBuffer=LL_DAC_OUTPUT_BUFFER_ENABLE,
                                               .OutputConnection=LL_DAC_OUTPUT_CONNECT_GPIO}};

struct dac dac_ch2_settings = {.dacx=DAC1, .channel=LL_DAC_CHANNEL_2,
                               .trg_src=LL_DAC_TRIG_EXT_TIM6_TRGO,
                               .bus_clk_abp=LL_APB1_GRP1_PERIPH_DAC1,
                               .timx_dac_irq=TIM6_DAC_LPTIM1_IRQn,
                               .dacx_settings={.OutputMode=LL_DAC_OUTPUT_MODE_NORMAL,
                                               .OutputBuffer=LL_DAC_OUTPUT_BUFFER_ENABLE,
                                               .OutputConnection=LL_DAC_OUTPUT_CONNECT_GPIO}};

struct dma dac_dma = {.dmax=DMA1, .channel=LL_DMA_CHANNEL_2, .data=(uint32_t *) dac_double_buff2,
                        .chan=(DMA_Channel_TypeDef *)((uint32_t)DMA1 + (8 + 20)),
                        .dmax_settings={.PeriphRequest=LL_DMAMUX_REQ_DAC1_CH2,
                                        .Direction=LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
                                        .NbData=256,
                                        .Mode=LL_DMA_MODE_CIRCULAR,
                                        .Priority=0x00,
                                        .PeriphOrM2MSrcDataSize=LL_DMA_PDATAALIGN_WORD,
                                        .MemoryOrM2MDstDataSize=LL_DMA_MDATAALIGN_WORD,
                                        .MemoryOrM2MDstIncMode=LL_DMA_MEMORY_INCREMENT,
                                        .PeriphOrM2MSrcIncMode=LL_DMA_PERIPH_NOINCREMENT}};

#define sample_count  5 /* π(nxt) do not forget the ranks number */
volatile uint16_t cv_raw_adc_inp[sample_count] = {0};
struct adc adc_settings = {.adcx=ADC1,
                          .data = cv_raw_adc_inp,
                          .roof='D',
                          .settings={.Clock=LL_ADC_CLOCK_SYNC_PCLK_DIV1,
                                     .Resolution=LL_ADC_RESOLUTION_12B,
                                     .DataAlignment=LL_ADC_DATA_ALIGN_RIGHT,
                                     .LowPowerMode=LL_ADC_LP_MODE_NONE},

                          .reg_settings={.TriggerSource=LL_ADC_REG_TRIG_EXT_TIM2_TRGO,
                                         .ContinuousMode=LL_ADC_REG_CONV_SINGLE,
                                         .SequencerLength=LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS,
                                         .SequencerDiscont=LL_ADC_REG_SEQ_DISCONT_DISABLE,
                                         .DMATransfer=LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
                                         .Overrun=LL_ADC_REG_OVR_DATA_OVERWRITTEN},

                          .dma_channel=LL_DMA_CHANNEL_4,
                          .dmax=DMA1,
                          .dmax_settings={.PeriphRequest=LL_DMAMUX_REQ_ADC1,
                                          .Direction=LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
                                          .NbData=sample_count,
                                          .Mode=LL_DMA_MODE_CIRCULAR,
                                          .PeriphOrM2MSrcDataSize=LL_DMA_PDATAALIGN_HALFWORD,
                                          .MemoryOrM2MDstDataSize=LL_DMA_MDATAALIGN_HALFWORD,
                                          .PeriphOrM2MSrcIncMode=LL_DMA_PERIPH_NOINCREMENT,
                                          .MemoryOrM2MDstIncMode=LL_DMA_MEMORY_INCREMENT}};

volatile struct button freq_mode_but_dac1 ={.pin={.pin_id=LL_GPIO_PIN_10, .id=0xff, .port_id=GPIOC, .mode=LL_GPIO_MODE_INPUT, .pull=LL_GPIO_PULL_UP},
                                           .exti={.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_10, .exti_port_conf=LL_EXTI_CONFIG_PORTC, .exti_line_conf=LL_EXTI_CONFIG_LINE10},
                                           .state=0, .flag='D'};

volatile struct button osc_0_mode_choice = {.pin={.pin_id=LL_GPIO_PIN_11, .id=0xff, .port_id=GPIOC, .mode=LL_GPIO_MODE_INPUT, .pull=LL_GPIO_PULL_UP},
                                           .exti={.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_11, .exti_port_conf=LL_EXTI_CONFIG_PORTC, .exti_line_conf=LL_EXTI_CONFIG_LINE11},
                                           .state=0, .flag='D'};

struct button distortion_choice = {.pin={.pin_id=LL_GPIO_PIN_12, .id=0xff, .port_id=GPIOC, .mode=LL_GPIO_MODE_INPUT, .pull=LL_GPIO_PULL_UP},
                                   .exti={.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_12, .exti_port_conf=LL_EXTI_CONFIG_PORTC, .exti_line_conf=LL_EXTI_CONFIG_LINE12},
                                   .state=0, .flag='D'};

struct gpio dac1 = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_4, .mode=LL_GPIO_MODE_ANALOG};
struct gpio dac2 = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_5, .mode=LL_GPIO_MODE_ANALOG};
struct gpio pitch_0_cv = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_0, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};
struct gpio dist_amount_0_cv = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_1, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};
struct gpio tunner_adc_in = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_6, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};
struct gpio pitch_1_cv = {.port_id=GPIOB, .pin_id=LL_GPIO_PIN_0, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};
struct gpio dist_amount_1_cv = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_7, .mode=LL_GPIO_MODE_ANALOG, .pull=LL_GPIO_PULL_NO};

struct flip_switch octave_switch = {.pins[1] = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_7, .id = 7, .mode=LL_GPIO_MODE_INPUT, .pull= LL_GPIO_PULL_DOWN},
                                    .it[1]   = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_7, .exti_line_conf=LL_EXTI_CONFIG_LINE7, .exti_port_conf=LL_EXTI_CONFIG_PORTC},
                                    .priority[1] = 0x40,
                                    ._state[1] = true,

                                    .pins[0] = {.port_id=GPIOA, .pin_id=LL_GPIO_PIN_8, .id = 8, .mode=LL_GPIO_MODE_INPUT, .pull= LL_GPIO_PULL_DOWN},
                                    .it[0]   = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_8, .exti_line_conf=LL_EXTI_CONFIG_LINE8, .exti_port_conf=LL_EXTI_CONFIG_PORTA},
                                    .priority[0] = 0x40,
                                    ._state[0] = true,
};

/* note: these 2 flippers align the code with pcb. */
struct flip_switch switch2_dev_rev0 = {.pins[1] = {.port_id=GPIOB, .pin_id=LL_GPIO_PIN_15, .id = 15, .mode=LL_GPIO_MODE_INPUT, .pull= LL_GPIO_PULL_DOWN},
                                    .it[1]   = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_15, .exti_line_conf=LL_EXTI_CONFIG_LINE15, .exti_port_conf=LL_EXTI_CONFIG_PORTB},
                                    .priority[1] = 0x80,
                                    ._state[1] = true,

                                    .pins[0] = {.port_id=GPIOB, .pin_id=LL_GPIO_PIN_14, .id = 14, .mode=LL_GPIO_MODE_INPUT, .pull= LL_GPIO_PULL_DOWN},
                                    .it[0]   = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_14, .exti_line_conf=LL_EXTI_CONFIG_LINE14, .exti_port_conf=LL_EXTI_CONFIG_PORTB},
                                    .priority[0] = 0x80,
                                    ._state[0] = true,
};
/* todo how to use the 3rd flipper */
struct flip_switch octave_switch3_dev_rev0 = {.pins[1] = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_9, .id = 9, .mode=LL_GPIO_MODE_INPUT, .pull= LL_GPIO_PULL_DOWN},
                                    .it[1]   = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_9, .exti_line_conf=LL_EXTI_CONFIG_LINE9, .exti_port_conf=LL_EXTI_CONFIG_PORTD},
                                    .priority[1] = 0x40,
                                    ._state[1] = true,

                                    .pins[0] = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_8, .id = 8, .mode=LL_GPIO_MODE_INPUT, .pull= LL_GPIO_PULL_DOWN},
                                    .it[0]   = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_8, .exti_line_conf=LL_EXTI_CONFIG_LINE8, .exti_port_conf=LL_EXTI_CONFIG_PORTD},
                                    .priority[0] = 0x40,
                                    ._state[0] = true,
};
/* todo */

struct encoder osc_0_pd_enc = {.A = {.pin = {.port_id=GPIOC, .id=4, .pin_id=LL_GPIO_PIN_4, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                                     .it_settings = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_4, .exti_port_conf=LL_EXTI_CONFIG_PORTC, .exti_line_conf=LL_EXTI_CONFIG_LINE4},
                                     .value={0}, .flag='D'},

                               .B = {.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_5, .id=5, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                                     .value = {0}, .it_settings = {0}, .flag = 'D'},

                               .direction=cw,
                               .virtual_wave_button = {.flag = 'D', .state=SINE}};

struct encoder osc_1_pd_enc = {.A = {.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_6, .id=6, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                                     .it_settings = {.exti_irqn=EXTI4_15_IRQn, .exti_line=LL_EXTI_LINE_6, .exti_port_conf=LL_EXTI_CONFIG_PORTC, .exti_line_conf=LL_EXTI_CONFIG_LINE6},
                                     .value={0}, .flag='D'},

                               .B = {.pin = {.port_id=GPIOC, .pin_id=LL_GPIO_PIN_8, .id=8, .mode = LL_GPIO_MODE_INPUT, .pull = LL_GPIO_PULL_UP},
                                     .value = {0}, .it_settings = {0}, .flag = 'D'},

                               .direction=cw,
                               .virtual_wave_button = {.flag = 'D', .state=SINE}};

struct display display = {.shift_registers={0,0},
                          .octave_shifts={0,0},
                          .view={wave, wave},
                          .tuner_view={playing, playing},
                          .blinky = &tim7_settings,
                          .locks={unlocked_view, unlocked_view},
                          .leds[0] = {.pin = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_5, .id=0, .mode=LL_GPIO_MODE_OUTPUT, .pull=LL_GPIO_PULL_NO},
                                      .state=false},
                          .leds[1] = {.pin = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_4, .id=1, .mode=LL_GPIO_MODE_OUTPUT, .pull=LL_GPIO_PULL_NO},
                                      .state=false},
                          .leds[2] = {.pin = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_3, .id=2, .mode=LL_GPIO_MODE_OUTPUT, .pull=LL_GPIO_PULL_NO},
                                      .state=false},
                          .leds[3] = {.pin = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_2, .id=3, .mode=LL_GPIO_MODE_OUTPUT, .pull=LL_GPIO_PULL_NO},
                                      .state=false},
                          .leds[4] = {.pin = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_1, .id=4, .mode=LL_GPIO_MODE_OUTPUT, .pull=LL_GPIO_PULL_NO},
                                      .state=false},
                          .leds[5] = {.pin = {.port_id=GPIOD, .pin_id=LL_GPIO_PIN_0, .id=5, .mode=LL_GPIO_MODE_OUTPUT, .pull=LL_GPIO_PULL_NO},
                                      .state=false},
};


struct overseer cosmos = {
    .oscillators = {&l_osc, &r_osc},
    .selected = &l_osc,
    ._data = &world,
    .sync = false,
};

struct flash_memory flash_settings = {.flash = FLASH,
                                      .bank  = FLASH_BANK_1,
                                      .flash_proc = {.Lock             = HAL_UNLOCKED,
                                                     .ErrorCode        = HAL_FLASH_ERROR_NONE,
                                                     .ProcedureOnGoing = FLASH_TYPENONE,
                                                     .Address          = 0x0U,
                                                     .Banks            = 0x0U,
                                                     .Page             = 0x0U,
                                                     .NbPagesToErase   = 0x0U},
                                      .option_bytes = {0}, /* todo */
                                      .eraser = {.TypeErase = FLASH_TYPEERASE_MASS,
                                                 .Banks     = 0x0U,
                                                 .NbPages   = 0x0U,
                                                 .Page      = 0x0U},
                                      .k = {.key1 = FLASH_KEY1,
                                            .key2 = FLASH_KEY2},
                                      .sr = {.errors         = FLASH_SR_ERRORS,
                                             .cfg_busy1_flag = FLASH_SR_CFGBSY,
                                             .clear          = FLASH_SR_CLEAR,
                                             .busy1_flag     = FLASH_SR_BSY1},
                                      .cr = {.lock    = FLASH_CR_LOCK,
                                             .start   = FLASH_CR_STRT,
                                             .options = {.k = {.key1 = FLASH_OPTKEY1,
                                                               .key2 = FLASH_OPTKEY2},
                                                         .start = FLASH_CR_OPTSTRT,
                                                         .lock  = FLASH_CR_OPTLOCK}}};
