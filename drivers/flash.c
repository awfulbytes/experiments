#include "flash.h"

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

void wait_flash_ops(struct flash_memory *fl){
    uint32_t error = 0;

    while((fl->flash->SR & fl->sr.busy1_flag) != 0x0U){
        __asm__ volatile ("nop");
    }

    error = (fl->flash->SR & fl->sr.errors);
    fl->flash->SR = fl->sr.clear;

    if(error != 0x0U){
        fl->flash_proc.ErrorCode = error;
        return;
    }

    while((fl->flash->SR & fl->sr.cfg_busy1_flag) != 0x0U){
        __asm__ volatile ("nop");
    }
}

static inline uint32_t read_flash_lock_state(struct flash_memory *fl){
    return ((fl->flash->CR) & (fl->cr.lock));
}

cr_error_e unlock_flash_cr_access(struct flash_memory *fl){
    if((read_flash_lock_state(fl)) != 0x0U){
        (fl->flash->KEYR) = (fl->k.key1); /* FLASH_KEY1 */
        (fl->flash->KEYR) = (fl->k.key2); /* FLASH_KEY2 */
        if(read_flash_lock_state(fl) != 0x0U){
            return unlock;
        }
    }
    return none;
}

cr_error_e lock_flash_cr_access(struct flash_memory *fl){
    wait_flash_ops(fl);
    (fl->flash->CR) |= (fl->cr.lock);
    if(read_flash_lock_state(fl) != 0x0U){
        return none;
    }
    return lock;
}

#pragma GCC diagnostic ignored "-Wimplicit-enum-enum-cast"
void mass_flash_erase(struct flash_memory *fl){
    fl->flash_proc.Lock = locked;
    fl->flash_proc.ErrorCode = 0x0u;

    wait_flash_ops(fl);

    fl->eraser.Banks = fl->bank;
    if(fl->eraser.TypeErase == FLASH_TYPEERASE_MASS){
        /* Proceed to Mass Erase */
        if(IS_FLASH_BANK(fl->eraser.Banks)){
            fl->flash->CR |= (fl->cr.start | fl->eraser.Banks);
        }
        wait_flash_ops(fl);
    }

    fl->flash_proc.Lock = removed;
}

void perform_erase(struct flash_memory *fl){
}
