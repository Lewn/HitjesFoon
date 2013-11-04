#include "hidheader.h"

PROGMEM const char usbHidReportDescriptor[40] = {
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Telefoon)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x10,                    //   USAGE (Dial)
    0x15, 0x01,                    //   LOGICAL_MINIMUM (1)
    0x25, 0x0a,                    //   LOGICAL_MAXIMUM (10)
    0x75, 0x04,                    //   REPORT_SIZE (4)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x42,                    //   INPUT (Data,Var,Abs,Null)
    0x19, 0x20,                    //   USAGE_MINIMUM (Hoorn)
    0x29, 0x21,                    //   USAGE_MAXIMUM (Aard)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x02,                    //   REPORT_COUNT (2)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x75, 0x02,                    //   REPORT_SIZE (2)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0xc0                           // END_COLLECTION
};
