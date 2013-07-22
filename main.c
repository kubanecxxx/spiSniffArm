#include "ch.h"
#include "hal.h"
#include "usb_user.h"

int main(void)
{
	chSysInit();
	halInit();

	//sampler sampl;
	palSetGroupMode(GPIOD,0b1111,12,PAL_MODE_OUTPUT_PUSHPULL);

	chThdSetPriority(LOWPRIO);
	usb_user_thread(NULL);


	return 1;
}


