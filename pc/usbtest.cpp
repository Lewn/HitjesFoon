#include <iostream>
#include "USBConnection.h"

using namespace std;

int main() {
    try {
		cout << "Connecting to phone" << endl;
        USBConnection connection;

        while (true) {
            // read from usb
            int bla = connection.read();
            /*if ((bla = connection.read()) != INPUT_NONE) {
				cout << "Received package from phone: " << endl;
				cout << "\t number dialed: " << bla << endl;
				cout << "\t horn is:       " << connection.isHornDown() << endl;
				cout << "\t earth is:      " << connection.isEarthDown() << endl;
            }*/
        }
    } catch (const char* ex) {
        printf("\n\nERROR: %s\n", ex);
    }
}
