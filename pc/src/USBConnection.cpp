#include "USBConnection.h"

USBConnection::USBConnection() {
    printUSB();

    usbDevice = hid_open(0x16c0, 0x27d9, L"leon.loopik.nl:USBPhone");
    if (!usbDevice) {
        printlevel(LWARNING, "Unable to open device, usb input will be disabled\n");
        printlevel(LWARNING, "Press any key to continue\n");
        getchar();
    } else {
        hid_set_nonblocking(usbDevice, 1);

        /* Commented out because not needed and always gave an error
        unsigned char* buf = new unsigned char[2];
        // Request state (cmd 0x81). The first byte is the report number (0x1).
        buf[0] = 0x0;
        buf[1] = 0x81;
        int res = hid_write(usbDevice, buf, 1);
        if (res < 0) {
            printlevel(LERROR, "Unable to write() (2)\n");
            //usbDevice = NULL;
        }

        SAFE_DELETE_ARRAY(buf);
        */

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
    hornDown = 1;
    earthDown = 0;
}

int USBConnection::read() {
    if (usbDevice) {
        unsigned char buf[2];
        memset(buf, 0, sizeof(buf));
        int res = hid_read(usbDevice, buf, sizeof(buf));
        if (res < 0) {
            throw "Read usb exception";
        }
        if (res) {
            if (buf[0] & DIAL_DATA_MASK) {
                // sent a number
                return ((buf[0] >> DIAL_DATA_SHIFT) & DIAL_DATA_MASK) % 10;
            }
            if (buf[0] & HORN_DATA_MASK) {
                // horn picked up
                if (hornDown) {
                    hornDown = 0;
                    return INPUT_HORN_UP;
                }
            } else {
                // horn down
                if (!hornDown) {
                    hornDown = 1;
                    return INPUT_HORN_DOWN;
                }
            }
            if (buf[0] & EARTH_DATA_MASK) {
                // earth button down
                if (earthDown) {
                    earthDown = 0;
                    return INPUT_EARTH_UP;
                }
            } else {
                // earth button up
                if (!earthDown) {
                    earthDown = 1;
                    return INPUT_EARTH_DOWN;
                }
            }
        }
    }
    return INPUT_NONE;
}

int USBConnection::isHornDown() {
    return hornDown;
}

int USBConnection::isEarthDown() {
    return earthDown;
}


void USBConnection::printUSB() {
    struct hid_device_info *devs, *cur_dev;

    devs = hid_enumerate(0x0, 0x0);
    cur_dev = devs;
    while (cur_dev) {
        printlevel(LDEBUG, "Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printlevel(LDEBUG, "\n");
        printlevel(LDEBUG, "  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printlevel(LDEBUG, "  Product:      %ls\n", cur_dev->product_string);
        printlevel(LDEBUG, "  Release:      %hx\n", cur_dev->release_number);
        printlevel(LDEBUG, "  Interface:    %d\n",  cur_dev->interface_number);
        printlevel(LDEBUG, "\n");
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
}
