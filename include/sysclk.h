#include "errors.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include <stdint.h>

typedef struct sysclk {
    uint32_t clk_source;
    uint32_t pllm;
    uint32_t plln;
    uint32_t pllr;
}sysclk_t;

void clk_config_settings(sysclk_t *clk);
void sys_clock_config(void);
