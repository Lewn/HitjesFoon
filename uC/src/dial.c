#include "dial.h"

uint8_t tick = -1;

void checkDial(void) {
    uint16_t timer = readTimer();
    if (!timer) {
		restartTimer();
#if defined(_TINY_)
        // timer will be restarted, so also reset the overflow count
        overflowCount = 0;
#endif
        tick = 1;
    }
    if (timer >= MIN_CLOCK) {
        if (timer <= MAX_CLOCK) {
            // down within last down, read as tick
            tick++;
#if defined(_TINY_)
            // timer will be restarted, so also reset the overflow count
            overflowCount = 0;
#endif
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
    if (!(DIAL_PIN & DIAL_PIN_MASK)) {     // only listen to rising_edge, check if value high
        return;
    }
    CLEARBIT(PCMSK, PCINT0);
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


#if defined(_TINY_)
ISR(TIMER1_OVF_vect, ISR_NOBLOCK) {
    // increment the overflow count
    if (++overflowCount == 0) {
        // don't let the overflow count overflow itself
        overflowCount = -1;
    }
}
#endif

ISR(TIMER1_COMPA_vect, ISR_NOBLOCK) {
#if defined(_TINY_)
    if (overflowCount < ((MAX_CLOCK >> 8) & 0xff)) {
        // check if we overflowed enough times
        return;
    }
#endif
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

#if defined(_TINY_)
    // Setup timer overflow interrupt, for counting large values
    // And compare for exact values
    SETBITS(TIMSK, BIT(TOIE1) | BIT(OCIE1A));
    // use the rest of MAX_CLOCK made 8 bits smaller
    OCR1A = (MAX_CLOCK >> 8) & 0xff;
#elif defined(_MEGA_)
    // Setup timer compare interupt
    SETBIT(TIMSK, OCIE1A);
    // Compare value
    OCR1A = MAX_CLOCK;
#endif

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
