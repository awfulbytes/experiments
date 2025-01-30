/* #if !defined(STM32G071xx) || !defined(USE_HAL_DRIVER) */
/* #define  STM32G071xx */
/* #define  USE_HAL_DRIVER */
/* #endif */
#ifndef __MAIN_H
#define __MAIN_H
#include "gpio.h"
#include "ui.h"
#include "dac.h"
#include "adc.h"
#include "forms.h"
#include "timx.h"


#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* void WaitForUserButtonPress(struct button *button); */
void DacUnderrunError_Callback(void);

#endif
