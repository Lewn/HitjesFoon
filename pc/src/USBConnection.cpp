#include "USBConnection.h"

USBConnection::USBConnection(GUI &gui) : gui(gui) {
    printUSB();

    usbDevice = hid_open(0x16c0, 0x27d9, L"leon.loopik.nl:USBPhone");
    if (!usbDevice) {
        gui.confirm(LWARNING, "Unable to open device, usb input will be disabled");
    } else {
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
        int res = hid_read_timeout(usbDevice, buf, sizeof(buf), 1000);
        if (res < 0) {
            gui.confirm(LERROR, "USB read exception '%s'\n", hid_error(usbDevice));
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
    // TODO memory leak here?
    if (gui.getMsglevel() >= LDEBUG) {
        // Only bother if we are actually printing
        struct hid_device_info *devs, *cur_dev;

        devs = hid_enumerate(0x0, 0x0);
        cur_dev = devs;
        while (cur_dev) {
            gui.printlevel(LDEBUG, "Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
            gui.printlevel(LDEBUG, "\n");
            gui.printlevel(LDEBUG, "  Manufacturer: %ls\n", cur_dev->manufacturer_string);
            gui.printlevel(LDEBUG, "  Product:      %ls\n", cur_dev->product_string);
            gui.printlevel(LDEBUG, "  Release:      %hx\n", cur_dev->release_number);
            gui.printlevel(LDEBUG, "  Interface:    %d\n",  cur_dev->interface_number);
            gui.printlevel(LDEBUG, "\n");
            cur_dev = cur_dev->next;
        }
        hid_free_enumeration(devs);
    }
}
