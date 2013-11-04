
#include "buttons.h"
#include "macros.h"

#include "display.h"

void setupButtons(void) {
  // enable port input
	CLEARBITS((GETDDR(BUTTONS_PORT_LETTER)), (HORN_PIN_MASK | EARTH_PIN_MASK));
	// set pullup
	SETBITS(BUTTONS_PORT, (HORN_PIN_MASK | EARTH_PIN_MASK));
}

void updateButtons(void) {
	setHornData(BITSCLEAR(BUTTONS_PIN, HORN_PIN_MASK));
	setEarthData(BITSCLEAR(BUTTONS_PIN, EARTH_PIN_MASK));
}