#include "usb.h"

static uchar    idleRate;   /* repeat rate for keyboards, never used for mice */

USB_PUBLIC uchar usbFunctionSetup(uchar data[8]) {
    usbRequest_t *rq = (void *)data;

    // The following requests are never used. But since they are required by
    // the specification, we implement them in this example.
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
        if(rq->bRequest == USBRQ_HID_GET_REPORT) {
            // wValue: ReportType (highbyte), ReportID (lowbyte)
            usbMsgPtr = (void *)&reportBuffer; // we only have this one
            return sizeof(reportBuffer);
        } else if(rq->bRequest == USBRQ_HID_GET_IDLE) {
            usbMsgPtr = &idleRate;
            return 1;
        } else if(rq->bRequest == USBRQ_HID_SET_IDLE) {
            idleRate = rq->wValue.bytes[1];
        }
    }

    return 0; // by default don't return any data
}

void setupUSB(void) {
    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();

    usbDeviceDisconnect(); // enforce re-enumeration
    for(uchar i = 0; i < 250; i++) { // wait 500 ms
        wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
    }
    usbDeviceConnect();
}

void updateUSB(void) {
    wdt_reset();
    usbPoll();
    if (isDataChanged() && usbInterruptIsReady()) {
        reportBuffer.number = getData();
        usbSetInterrupt((void *)&reportBuffer, sizeof(reportBuffer));
    }
}
