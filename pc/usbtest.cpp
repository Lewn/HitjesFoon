#include <iostream>
#include "USBConnection.h"

using namespace std;

int main() {
    try {	
		cout << "Connecting to phone" << endl;
        USBConnection connection;
		
        while (true) {
            // read from usb
            if (connection.read()) {
				cout << "Received package from phone: " << endl;
				cout << "\t number dialed: " << connection.getDialNumber() << endl;
				cout << "\t horn is:       " << connection.isHornDown() << endl;
            }
        }
    } catch (const char* ex) {
        printf("\n\nERROR: %s\n", ex);
    }
}
