#include "display.h"
#if defined(_MEGA_)

void dispDot(uint8_t dot) {
    if (!dot) {
        SETBIT(DISPLAY_PORT, PB7);
    } else {
        CLEARBIT(DISPLAY_PORT, PB7);
    }
}

void dispNum(uint8_t num) {
    uint8_t seg = pgm_read_byte(sevensegdigits + 16);
    if (num >= 0 && num < 16) {
        seg = pgm_read_byte(sevensegdigits + num);
    }
    WRITEMASKEDBYTE(DISPLAY_PORT, seg, 0b01111111);
}

void setupDisplay(void) {
    // enable port output
    SETBITS(GETDDR(DISPLAY_PORT_LETTER), 0xFF);

    // show a 0.
    dispNum(0);
}
#endif
