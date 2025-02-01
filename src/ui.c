#include "ui.h"

/**
 * @brief  Wait for User push-button press to start transfer.
 * @param  *button A button with a state and other information.
 * @retval None
 */
// DEPRECATED:: This should be in debug flag
void WaitForUserButtonPress
(struct button *button) {
    while (button->state == 0) {
        LL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        LL_mDelay(LED_BLINK_SLOW);
    }
}

inline void wave_button_callback
(struct button *abut) {
    // NOTE:: need some more work but elswhere..
    switch (abut->state) {
        case 2:
            abut->state >>= abut->state;
            break;
        default:
            abut->state++;
            break;
    }
}
