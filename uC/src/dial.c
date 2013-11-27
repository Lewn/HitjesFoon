#include "dial.h"

volatile uint8_t tick = -1;

void checkDial(void) {
#if defined(_TINY_)
    uint8_t timer = readTimer();
#elif defined(_MEGA_)
    uint16_t timer = readTimer();
#endif // defined
    if (!timer) {
        restartTimer();
        tick = 1;
    }
    if (timer >= MIN_CLOCK) {
        if (timer <= MAX_CLOCK) {
            // down within last down, read as tick
            tick++;
            restartTimer();
        }
    }
}

uint8_t dialEnd(void) {
    // down outside last down, invalid tick, reset
    stopTimer();
    if (tick > 0 && tick <= 10) {
        return tick;
    } else {
        return 16;
    }
}

#if defined(_TINY_)
ISR(PCINT0_vect, ISR_NOBLOCK) {
    CLEARBIT(PCMSK, PCINT0);
    if (!(DIAL_PIN & DIAL_PIN_MASK)) {     // only listen to rising_edge, check if value high
        SETBIT(PCMSK, PCINT0);
        return;
    }
#elif defined(_MEGA_)
ISR(INT1_vect, ISR_NOBLOCK) {
    CLEARBIT(GICR, INT1);
#endif
    _delay_ms(10);
    if (DIAL_PIN & DIAL_PIN_MASK) {
        checkDial();
    }
#if defined(_TINY_)
    SETBIT(PCMSK, PCINT0);
#elif defined(_MEGA_)
    SETBIT(GICR, INT1);
#endif
}

ISR(TIMER1_COMPA_vect, ISR_NOBLOCK) {
    uint8_t number = dialEnd();
    if(number != -1) {
#if defined(_MEGA_)
        dispNum(number);
#endif
        setDialData(number);
    }
}

void setupDial(void) {
    // enable port input
    CLEARBITS(GETDDR(DIAL_PORT_LETTER), DIAL_PIN_MASK);
    // set pullup
    SETBITS(DIAL_PORT, DIAL_PIN_MASK);

    // force reset timer at start
    stopTimer();

    // Setup timer compare interupt
    SETBIT(TIMSK, OCIE1A);
    // Compare value
    OCR1A = MAX_CLOCK;

#if defined(_TINY_)
    // Setup pin interrupt on PCINT0
    SETBIT(GIMSK, PCIE);
    SETBIT(PCMSK, PCINT0);
#elif defined(_MEGA_)
    // Setup pin interupt to INT1 rising edge
    SETBITS(MCUCR, BIT(ISC11) | BIT(ISC10));
    SETBIT(GICR, INT1);
#endif
}
