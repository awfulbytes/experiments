#include "ui.h"


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
