#include "stm32g071xx.h"
#include "stm32g0xx_hal_flash.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {unlock, lock, done}cr_error_e;
typedef enum {removed = 0x0U, locked = 0x1U}lock_e;

struct keys{
    const uint32_t key1;
    const uint32_t key2;
};

struct ob_info{
    uint32_t lock;
    uint32_t start;
    const struct keys k;
};

struct cr_info{
    uint32_t lock;
    uint32_t start;
    struct ob_info options;
};

struct sr_info{
    uint32_t errors;
    uint32_t clear;
    uint32_t busy1_flag;
    uint32_t cfg_busy1_flag;
};

struct flash_memory{
    FLASH_TypeDef *flash;
    FLASH_ProcessTypeDef flash_proc;
    FLASH_OBProgramInitTypeDef option_bytes;
    FLASH_EraseInitTypeDef eraser;
    struct sr_info sr;
    struct cr_info cr;
    const struct keys k;
    uint32_t cfg_busy;
    uint32_t bank;
};


void wait_flash_ops(struct flash_memory *fl);
