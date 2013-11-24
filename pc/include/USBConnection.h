#ifndef USBCONNECTION_H
#define USBCONNECTION_H


#include "hidapi.h"
#include "../../telephoneProtocol.h"
#include <stdio.h>
#include <memory.h>
#include "InputProcessor.h"
#include "Tools.h"

using namespace std;

class USBConnection {
public:
    USBConnection();
    virtual ~USBConnection();

    void reset();
    int read();
    int dialNumberReady();
    int getDialNumber();

    int isHornDown();
    int isEarthDown();

    static void printUSB();
protected:
    hid_device* usbDevice;
    int dialNumber, dialTimes;
    int hornDown, earthDown;
private:
};

#endif // USBCONNECTION_H
