#include "dial.h"
#include "macros.h"

uint8_t tick = -1;

void checkDial(void)
{
    uint16_t timer = readTimer();
    if (!timer)
    {
        restartTimer();
        tick = 1;
    }
    if (timer >= MIN_CLOCK)
    {
        if (timer <= MAX_CLOCK)
        {
            // down within last down, read as tick
            tick++;
            restartTimer();
        }
    }
}

uint8_t dialEnd(void)
{
    // down outside last down, invalid tick, reset
    stopTimer();
    if (tick > 0 && tick <= 10)
    {
        return tick;
    }
    else
    {
        return 16;
    }
}

ISR(INT1_vect, ISR_NOBLOCK) {
	CLEARBIT(GICR, INT1);
	_delay_ms(10);
	if (DIAL_PIN & DIAL_PIN_MASK) {
		checkDial();
	}
	SETBIT(GICR, INT1);
}

ISR(TIMER1_COMPA_vect) {
	uint8_t number = dialEnd();
	if(number != -1) {
		dispNum(number);
		setDialData(number);
	}
}

void setupDial(void) {
    // enable port D input
		CLEARBITS(GETDDR(DIAL_PORT_LETTER), DIAL_PIN_MASK);
		// set pullup
		SETBITS(DIAL_PORT, DIAL_PIN_MASK);

    // force reset timer at start
    stopTimer();
		
		// Setup timer interupt
		SETBIT(TIMSK, OCIE1A);
		// Compare value
		OCR1A = MAX_CLOCK;
		
		// Setup pin interupt to INT1 rising edge
		SETBITS(MCUCR, BIT(ISC11) | BIT(ISC10));
		SETBIT(GICR, INT1);
}
