#include <util/delay.h>

#include "common.h"
#include "dial.h"
#include "buttons.h"
#include "usb.h"

#if defined(_TINY_)
#include "calibration.h"

// Called by V-USB after device reset
void hadUsbReset() {
	calibrate();
}
#elif defined(_MEGA_)
#include "display.h"
#endif

int main(void) {
    setupDial();
    setupButtons();
#if defined(_MEGA_)
	setupDisplay();
#endif
	setupUSB();
    sei();

    while(1) {
        updateButtons();
        updateUSB();
        _delay_ms(9);
    }
    return 0;
}

