#ifndef __TIMER_H__
#define __TIMER_H__

#include "common.h"

#include <avr/io.h>

#if defined(_TINY_)
uint8_t readTimer(void);
#elif defined(_MEGA_)
uint16_t readTimer(void);
#endif

void stopTimer(void);
void restartTimer(void);

#endif
