#include "flash.h"

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
    wait_flash_ops(fl);
    if((read_flash_lock_state(fl)) != 0x0U){
        (fl->flash->KEYR) = (fl->k.key1); /* FLASH_KEY1 */
        (fl->flash->KEYR) = (fl->k.key2); /* FLASH_KEY2 */
        if(read_flash_lock_state(fl) != 0x0U){
            return unlock;
        }
    }
    return noerr;
}

cr_error_e lock_flash_cr_access(struct flash_memory *fl){
    wait_flash_ops(fl);
    (fl->flash->CR) |= (fl->cr.lock);
    if(read_flash_lock_state(fl) != 0x0U){
        return noerr;
    }
    return lock;
}

#pragma GCC diagnostic ignored "-Wenum-conversion"
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

void sw_system_reset(void){
    register uint32_t unlock_rq = (0x05faU << 16);
    register uint32_t enable_rq = (1U<<2);
    SCB->AIRCR = unlock_rq | enable_rq;
}

void set_empty_flag(struct flash_memory *fl){
    fl->flash->ACR |= (1<<16);
}

void perform_erase(struct flash_memory *fl){
    cr_error_e flash_locking_errors = noerr;

    flash_locking_errors = unlock_flash_cr_access(fl);
    if(flash_locking_errors == noerr){
        mass_flash_erase(fl);
    }

    flash_locking_errors = lock_flash_cr_access(fl);
    if(flash_locking_errors == noerr){
        set_empty_flag(fl); /* power reset after */
    }
}
