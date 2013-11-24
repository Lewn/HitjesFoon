# Command-line client
CMDLINE = usbtest.exe

# One-liner to compile the command-line client from usbtest.c
$(CMDLINE): usbtest.cpp
	g++ -I include -I hidapi/include -L hidapi/lib -O -Wall usbtest.cpp src/USBConnection.cpp -o usbtest.exe -lhidapi.dll
