#include "timer.h"
#include "macros.h"

uint16_t readTimer(void) {
    return TCNT1;
}

void stopTimer(void) {
    //stop timer
#if defined(__AVR_ATtiny45__)
    TCCR1 = 0;
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
    TCCR1B = 0;
#endif
    // set it to 0
    TCNT1 = 0;
}

void restartTimer(void) {
    stopTimer();
#if defined(__AVR_ATtiny45__)
    SETBITS(TCCR1, BIT(CS12) | BIT(CS10)); // start timer at 12 MHZ / 1024
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
    SETBITS(TCCR1B, BIT(CS12) | BIT(CS10)); // start timer at 12 MHZ / 1024
#endif
}
