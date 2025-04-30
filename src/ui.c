#include "ui.h"

void phase_dist_button_callback(struct button *abut){
    switch (abut->state) {
        case 8:
            abut->state >>= abut->state;
            break;
        default:
            ++abut->state;
            break;
    }
    // abut->flag = 'D';
}

inline void wave_button_callback
(struct button *abut) {
    // NOTE:: need some more work but elswhere..
    switch (abut->state) {
        case 2:
            abut->state >>= abut->state;
            break;
        default:
            ++abut->state;
            break;
    }
    abut->flag = 'D';
}
