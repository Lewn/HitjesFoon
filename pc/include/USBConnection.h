#ifndef USBCONNECTION_H
#define USBCONNECTION_H


#include "hidapi/hidapi.h"
#include "../../telephoneProtocol.h"
#include <stdio.h>
#include <memory.h>
#include "Tools.h"
#include "InputProcessor.h"

using namespace std;

class USBConnection {
public:
    USBConnection();
    virtual ~USBConnection();

    void reset();
    int read();

    int isHornDown();
    int isEarthDown();

    static void printUSB();
protected:
    hid_device* usbDevice;
    int dialNumber, dialTimes;
    int hornDown, earthDown;
private:
    USBConnection(const USBConnection &that) = delete;
};

#endif // USBCONNECTION_H
