#ifndef __HIDHEADER_H__
#define __HIDHEADER_H__

#include <avr/pgmspace.h>
#include "usbdrv/usbdrv.h"

extern PROGMEM const char usbHidReportDescriptor[];

/* A keypad :)
 *     . . . . X3 X2 X1 X0 .... 4 bit unsigned which key is pressed (10 = 0)
 */
typedef struct{
    char    number;
} report_t;

#endif
