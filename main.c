// DRO display for iGaging scales
// Copyright (C) 2018 Charles-Henri Mousset - ch.mousset@gmail.com
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3 of the License
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "ch.h"
#include "hal.h"
#include "ch_test.h"
#include "chprintf.h"
#include "shell.h"

#include "usbcfg.h"

#include "gfx.h"
#include "ugfx/src/gwin/gwin_keyboard_layout.h"

#include "scale.h"
#include "scale_master.h"
#include "scale_slave.h"
#include "scale_qei.h"
#include "cancom.h"
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "motion.h"


#define TRUE 1
#define FALSE 0

// Shell setup
#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

void shl_can(BaseSequentialStream *chp, int argc, char *argv[])
{
	CANTxFrame tx;
	if(argc < 1)
	{
		chprintf(chp, "can <id> [data] [data] ...");
		return;
	}
	tx.SID = atoi(argv[0]);
	tx.IDE = CAN_IDE_STD;
	tx.RTR = CAN_RTR_DATA;
	tx.DLC = 0;
	int i;
	for(i=1; i< argc; i++)
	{
		tx.data8[i-1] = atoi(argv[i]);
		tx.DLC++;
	}
	canTransmit(&CAND1, CAN_ANY_MAILBOX, &tx,  MS2ST(100));
}

void shl_rotate(BaseSequentialStream *chp, int argc, char *argv[])
{
	switch(argc)
	{
		case 0:
			chprintf(chp, "Please specify orientation\r\n");
			break;
		case 1:
			switch(atoi(argv[0]))
			{
				default:
					chprintf(chp, "orientation invalid: %d", atoi(argv[0]));
					break;
				case 0:
					gdispSetOrientation(GDISP_ROTATE_0);
					break;
				case 90:
					gdispSetOrientation(GDISP_ROTATE_90);
					break;
				case 180:
					gdispSetOrientation(GDISP_ROTATE_180);
					break;
				case 270:
					gdispSetOrientation(GDISP_ROTATE_270);
					break;
			};
	};
}

static const ShellCommand commands[] = {
	{"can", shl_can},
	{"rotate", shl_rotate},
	{"pos", shlPos},
	{"en", shlEn},
	{NULL, NULL}
};

char histbuff[128] = "";

static const ShellConfig shell_cfg1 = {
	(BaseSequentialStream *)&SDU1,
	commands,
	histbuff,
	sizeof(histbuff),
	.sc_completion = NULL
};
static const ShellConfig shell_cfg_base = {
	(BaseSequentialStream *)&SD1,
	commands,
	histbuff,
	sizeof(histbuff),
	.sc_completion = NULL
};


int main(void)
{
	halInit();
	chSysInit();
	gfxInit();

	cancom_init();

	ui_init();
	motion_init();

	// Shell over USB setup
	shellInit();
	sduObjectInit(&SDU1);
	palSetLineMode(LINE_VCP_TX, PAL_MODE_ALTERNATE(7));
	palSetLineMode(LINE_VCP_RX, PAL_MODE_ALTERNATE(7));
	sdStart(&SD1, NULL);
	chprintf(&SD1, "hello world");
	chThdCreateFromHeap(NULL, SHELL_WA_SIZE, "base shell", NORMALPRIO, shellThread, (void *)&shell_cfg_base);
	// sduStart(&SDU1, &serusbcfg);

	// usbDisconnectBus(serusbcfg.usbp);
	// chThdSleepMilliseconds(1000);
	// usbStart(serusbcfg.usbp, &usbcfg);
	// usbConnectBus(serusbcfg.usbp);

	// Setup scales drivers
	int i;
	for(i=0; i<N_AXIS; i++)
		scales[i].state = SCALE_UNINIT;
	scale_slave_init(scales);
	scale_master_init(scales);
	scale_qei_init(scales);

	while (true) {
		// if (SDU1.config->usbp->state == USB_ACTIVE) {
		// 	thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE,
		// 											"shell", NORMALPRIO + 1,
		// 											shellThread, (void *)&shell_cfg1);
		// 	chThdWait(shelltp);               /* Waiting termination.             */
		// }
		chThdSleepMilliseconds(100);
	}
}
