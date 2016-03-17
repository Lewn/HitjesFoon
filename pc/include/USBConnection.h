#ifndef USBCONNECTION_H
#define USBCONNECTION_H


#include "gui/GUI.h"
#include "hidapi/hidapi.h"
#include "../../telephoneProtocol.h"
#include <stdio.h>
#include <memory.h>
#include "Tools.h"
#include "InputProcessor.h"

using namespace std;

class USBConnection {
public:
    USBConnection(GUI &gui);
    virtual ~USBConnection();

    void reset();
    int read();

    int isHornDown();
    int isEarthDown();

    void printUSB();
protected:
    GUI &gui;
    hid_device* usbDevice;
    int dialNumber, dialTimes;
    int hornDown, earthDown;
private:
    USBConnection(const USBConnection &that) = delete;
};

#endif // USBCONNECTION_H
