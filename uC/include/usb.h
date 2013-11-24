#ifndef __USB_H__
#define __USB_H__

#include "common.h"

#include <avr/wdt.h>
#include <util/delay.h>

#include "hidheader.h"
#include "telephoneData.h"
#include "usbdrv/usbdrv.h"

volatile static report_t reportBuffer;

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]);
void setupUSB(void);
void updateUSB(void);

#endif
