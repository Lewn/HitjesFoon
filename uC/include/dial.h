#ifndef __DIAL_H__
#define __DIAL_H__

#include "common.h"

#include <avr/io.h>

#if defined(_TINY_)
#define MAX_CLOCK (F_CPU / 8  / 8192)    //Hz
#define MIN_CLOCK (F_CPU / 12 / 8192)    //Hz

#if MAX_CLOCK > 0xFF
    #error "MAX_CLOCK IS DEFINED TOO HIGH"
#endif

#elif defined(_MEGA_)
#define MAX_CLOCK (F_CPU / 8  / 1024)    //Hz
#define MIN_CLOCK (F_CPU / 12 / 1024)    //Hz
#endif


#if MAX_CLOCK < MIN_CLOCK
    #error "MAX_CLOCK should be higher than MIN_CLOCK"
#endif

// MUST BE INT1 FOR INTERRUPTS
#if defined(_TINY_)

#define DIAL_PORT_LETTER B
#define DIAL_PIN_NR PB0

#elif defined(_MEGA_)

#define DIAL_PORT_LETTER D
#define DIAL_PIN_NR PD3

#endif

#define DIAL_PORT GETPORT(DIAL_PORT_LETTER)
#define DIAL_PIN GETPIN(DIAL_PORT_LETTER)
#define DIAL_PIN_MASK (1 << DIAL_PIN_NR)


#include <avr/interrupt.h>
#include <util/delay.h>

#if defined(_MEGA_)
#include "display.h"
#endif

#include "usb.h"
#include "timer.h"

volatile uint8_t tick;

void checkDial(void);
uint8_t dialEnd(void);
void setupDial(void);

#endif
