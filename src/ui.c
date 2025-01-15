#include "ui.h"

/**
 * @brief  Wait for User push-button press to start transfer.
 * @param  *button A button with a state and other information.
 * @retval None
 */
void WaitForUserButtonPress
(struct button *button) {
    while (button->state == 0) {
        LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        LL_mDelay(LED_BLINK_SLOW);
    }
}

void UserButton_Callback
(struct button *abut) {

    /* LL_mDelay(6); // HACK:: dont do this */
    switch (abut->state) {
        case 0x0:
            abut->state = 0x1;
            break;
        case 0x1:
            abut->state = 0x2;
            break;
        case 0x2:
            abut->state >>= abut->state;
            break;
    }
}
