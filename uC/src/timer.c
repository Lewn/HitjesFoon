#include "timer.h"
#include "macros.h"

uint16_t readTimer(void)
{
    return TCNT1;
}

void stopTimer(void)
{
		//stop timer
    TCCR1B = 0;
		// set it to 0
    TCNT1 = 0;
}

void restartTimer(void)
{
    stopTimer();
		SETBITS(TCCR1B, BIT(CS12) | BIT(CS10)); // start timer at 12 MHZ / 1024
}
