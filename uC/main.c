#include <util/delay.h>

#include "macros.h"
#include "7segdecode.h"
#include "dial.h"
#include "buttons.h"
#include "usb.h"

int main(void)
{
	setupDial();
	setupButtons();
	setupDisplay();
	setupUSB();
	sei();
	
	while(1)
	{
		updateButtons();
		updateUSB();
		_delay_ms(10);
	}
	return 0;
}

