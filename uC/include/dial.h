#ifndef __DIAL_H__
#define __DIAL_H__

#include <avr/io.h>

#define MAX_CLOCK 1300
#define MIN_CLOCK 1000

// MUST BE INT1
#define DIAL_PORT_LETTER D
#define DIAL_PIN_NR PD3

#define DIAL_PORT GETPORT(DIAL_PORT_LETTER)
#define DIAL_PIN GETPIN(DIAL_PORT_LETTER)
#define DIAL_PIN_MASK (1 << DIAL_PIN_NR)

#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "usb.h"
#include "timer.h"

uint8_t tick;

void checkDial(void);
uint8_t dialEnd(void);
void setupDial(void);

#endif
