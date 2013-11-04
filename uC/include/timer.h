#ifndef __TIMER_H__
#define __TIMER_H__

#include <avr/io.h>

uint16_t readTimer(void);
void stopTimer(void);
void restartTimer(void);

#endif
