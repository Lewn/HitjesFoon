#ifndef USBCONNECTION_H
#define USBCONNECTION_H


#include "hidapi.h"
#include "../../telephoneProtocol.h"
#include <stdio.h>
#include <memory.h>

using namespace std;

class USBConnection {
public:
    USBConnection();
    virtual ~USBConnection();

    void reset();
    int read();
    int getDialNumber();
    int isHornDown();

    static void printUSB();
protected:
    hid_device* usbDevice;
    int dialNumber, dialTimes;
    int hornDown;
private:
};

#endif // USBCONNECTION_H
