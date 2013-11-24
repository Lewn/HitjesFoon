#ifndef __COMMON_H__
#define __COMMON_H__

#include "macros.h"

#if defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
#define _TINY_
#elif defined(__AVR_ATmega16A__) || defined(__AVR_ATmega16__)
#define _MEGA_
#endif

#endif
