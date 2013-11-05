#include "dial.h"
#include "macros.h"

uint8_t tick = -1;

void checkDial(void) {
    uint16_t timer = readTimer();
    if (!timer) {
#if defined(__AVR_ATtiny45__)
        // timer will be restarted, so also reset the overflow count
        overflowCount = 0;
#endif
        restartTimer();
        tick = 1;
    }
    if (timer >= MIN_CLOCK) {
        if (timer <= MAX_CLOCK) {
            // down within last down, read as tick
            tick++;
#if defined(__AVR_ATtiny45__)
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

#if defined(__AVR_ATtiny45__)
ISR(PCINT0_vect, ISR_NOBLOCK) {
    if (BITSET(DIAL_PIN, DIAL_PIN_NR)) {     // only listen to rising_edge, check if value high
        return;
    }
    CLEARBIT(PCMSK, PCINT0);
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
ISR(INT1_vect, ISR_NOBLOCK) {
    CLEARBIT(GICR, INT1);
#endif
    _delay_ms(10);
    if (DIAL_PIN & DIAL_PIN_MASK) {
        checkDial();
    }
#if defined(__AVR_ATtiny45__)
    SETBIT(PCMSK, PCINT0);
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
    SETBIT(GICR, INT1);
#endif
}


#if defined(__AVR_ATtiny45__)
ISR(TIMER1_OVF_vect) {
    // increment the overflow count
    if (++overflowCount == 0) {
        // don't let the overflow count overflow itself
        overflowCount = -1;
    }
}
#endif

ISR(TIMER1_COMPA_vect) {
#if defined(__AVR_ATtiny45__)
    if (overflowCount < (MAX_CLOCK << 8)) {
        // check if we overflowed enough times
        return;
    }
#endif
    uint8_t number = dialEnd();
    if(number != -1) {
        dispNum(number);
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

#if defined(__AVR_ATtiny45__)
    // Setup timer overflow interrupt, for counting large values
    // And compare for exact values
    SETBITS(TIMSK, BIT(TOIE1) | BIT(OCIE1A));
    // use the rest of MAX_CLOCK made 8 bits smaller
    OCR1A = MAX_CLOCK - (MAX_CLOCK << 8);
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
    // Setup timer compare interupt
    SETBIT(TIMSK, OCIE1A);
    // Compare value
    OCR1A = MAX_CLOCK;
#endif

#if defined(__AVR_ATtiny45__)
    // Setup pin interrupt on PCINT0
    SETBIT(GIMSK, PCIE);
    SETBIT(PCMSK, PCINT0);
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
    // Setup pin interupt to INT1 rising edge
    SETBITS(MCUCR, BIT(ISC11) | BIT(ISC10));
    SETBIT(GICR, INT1);
#endif
}
