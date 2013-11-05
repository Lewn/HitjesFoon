#ifndef __DIAL_H__
#define __DIAL_H__

#include <avr/io.h>

#define MAX_CLOCK 1300
#define MIN_CLOCK 1000

// MUST BE INT1 FOR INTERRUPTS
#if defined(__AVR_ATtiny45__)

#define DIAL_PORT_LETTER B
#define DIAL_PIN_NR PB0

#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)

#define DIAL_PORT_LETTER D
#define DIAL_PIN_NR PD3

#endif



#define DIAL_PORT GETPORT(DIAL_PORT_LETTER)
#define DIAL_PIN GETPIN(DIAL_PORT_LETTER)
#define DIAL_PIN_MASK (1 << DIAL_PIN_NR)


#include <avr/interrupt.h>
#include <util/delay.h>

#include "display.h"
#include "usb.h"
#include "timer.h"

uint8_t tick;
#if defined(__AVR_ATtiny45__)
uint8_t overflowCount;
#endif

void checkDial(void);
uint8_t dialEnd(void);
void setupDial(void);

#endif
