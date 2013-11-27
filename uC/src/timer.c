#include "timer.h"

#if defined(_TINY_)
uint8_t readTimer(void) {
#elif defined(_MEGA_)
uint16_t readTimer(void) {
#endif
    return TCNT1;
}

void stopTimer(void) {
    //stop timer
#if defined(_TINY_)
    TCCR1 = 0;
#elif defined(_MEGA_)
    TCCR1B = 0;
#endif
    // set it to 0
    TCNT1 = 0;
}

void restartTimer(void) {
    stopTimer();
    /**
     * @todo prescaler definieren
     */
#if defined(_TINY_)
    SETBITS(TCCR1, BIT(CS13) | BIT(CS12) | BIT(CS11)); // start timer at 16.5 MHZ / 8192
#elif defined(_MEGA_)
    SETBITS(TCCR1B, BIT(CS12) | BIT(CS10)); // start timer at 12 MHZ / 1024
#endif
}
