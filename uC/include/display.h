#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "common.h"

#if defined(_MEGA_)

#define DISPLAY_PORT_LETTER B

#define DISPLAY_PORT (GETPORT(DISPLAY_PORT_LETTER))

#include <avr/pgmspace.h>

#include "7segdecode.h"

void dispDot(uint8_t dot);
void dispNum(uint8_t num);
void setupDisplay(void);

#else

#define dispDot(x)
#define dispNum(x)
#define setupDisplay(x)

#endif

#endif
