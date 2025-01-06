#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_utils.h"

#define LED4_PIN                           LL_GPIO_PIN_5
#define LED4_GPIO_PORT                     GPIOA
/* #define LED4_GPIO_CLK_ENABLE()             LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA) */

/**
  * @brief Toggle periods for various blinking modes
  */
#define LED_BLINK_FAST  200
#define LED_BLINK_SLOW  500
#define LED_BLINK_ERROR 1000

/**
  * @brief Key push-button
  */
#define USER_BUTTON_PIN                         LL_GPIO_PIN_13
#define USER_BUTTON_GPIO_PORT                   GPIOC
/* #define USER_BUTTON_GPIO_CLK_ENABLE()           LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC) */

#define USER_BUTTON_EXTI_LINE                   LL_EXTI_LINE_13
#define USER_BUTTON_EXTI_IRQn                   EXTI4_15_IRQn
#define USER_BUTTON_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(USER_BUTTON_EXTI_LINE)
#define USER_BUTTON_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTC, LL_EXTI_CONFIG_LINE13);  \
                                                } while(0)
#define USER_BUTTON_IRQHANDLER                  EXTI4_15_IRQHandler



#define USER_BUTTON_EXTI_IRQn EXTI4_15_IRQn

struct button {
  volatile uint8_t state;
  volatile uint8_t flag;
};

void gpio_init(void);

void UserButton_Callback(struct button *abut);
