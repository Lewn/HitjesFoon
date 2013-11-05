#include "USBConnection.h"

USBConnection::USBConnection() {
    printUSB();

    usbDevice = hid_open(0x16c0, 0x27d9, L"leon.loopik.nl:USBPhone");
    if (!usbDevice) {
        printf("unable to open device, usb input will be disabled\n");
        char* buf = new char[1024];
        getchar();
        delete[] buf;
    } else {
        hid_set_nonblocking(usbDevice, 1);

        unsigned char* buf = new unsigned char[2];
        // Request state (cmd 0x81). The first byte is the report number (0x1).
        buf[0] = 0x0;
        buf[1] = 0x81;
        int res = hid_write(usbDevice, buf, 17);
        if (res < 0) {
            printf("Unable to write() (2)\n");
        }

        delete[] buf;

        reset();
    }
}

USBConnection::~USBConnection() {
    if (usbDevice) {
        hid_close(usbDevice);
    }
    hid_exit();
}

void USBConnection::reset() {
    dialNumber = 0;
    dialTimes = 0;
}

int USBConnection::read() {
    int returnVal = 0;
    if (usbDevice) {
        unsigned char* buf = new unsigned char[2];
        memset(buf, 0, sizeof(buf));
        int res = hid_read(usbDevice, buf, sizeof(buf));   // timeout of 1 second, should be about the max dial time
        if (res < 0) {
            delete[] buf;
            throw "Read usb exception";
        }

        if (buf[0] & DIAL_DATA_MASK) {
            // sent a number
            int num = (((buf[0] >> DIAL_DATA_SHIFT) & DIAL_DATA_MASK) % 10);
            printf("\nReceived %d\n", num);
            dialNumber = dialNumber * 10 + num;
            if (++dialTimes == 3) {
                returnVal = 1;
            }
        }
        if (buf[0] & HORN_DATA_MASK) {
            // horn picked up
            if (hornDown) {
                printf("Horn picked up\n");
                hornDown = 0;
                returnVal = 1;
            }
        } else {
            // horn down
            if (!hornDown) {
                printf("Horn is down\n");
                hornDown = 1;
                returnVal = 1;
            }
        }
        if (buf[0] & EARTH_DATA_MASK) {
            // earth button pressed, reset
            reset();
        }
        delete[] buf;
    }
    return returnVal;
}

int USBConnection::getDialNumber() {
    int tmp = dialNumber;
    reset();
    return tmp;
}

int USBConnection::isHornDown() {
    return hornDown;
}



void USBConnection::printUSB() {
    struct hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Release:      %hx\n", cur_dev->release_number);
        printf("  Interface:    %d\n",  cur_dev->interface_number);
        printf("\n");
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}
