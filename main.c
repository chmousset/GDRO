/*
		ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

		Licensed under the Apache License, Version 2.0 (the "License");
		you may not use this file except in compliance with the License.
		You may obtain a copy of the License at

				http://www.apache.org/licenses/LICENSE-2.0

		Unless required by applicable law or agreed to in writing, software
		distributed under the License is distributed on an "AS IS" BASIS,
		WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
		See the License for the specific language governing permissions and
		limitations under the License.
*/

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"

#include "usbcfg.h"

#include "gfx.h"

static GHandle ghSlider1;
static font_t font1;

#define TRUE 1
#define FALSE 0

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

	(void)arg;
	chRegSetThreadName("blinker1");
	while (true) {
		palClearPad(GPIOG, GPIOG_LED4_RED);
		chThdSleepMilliseconds(500);
		palSetPad(GPIOG, GPIOG_LED4_RED);
		chThdSleepMilliseconds(500);
	}
}

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

	(void)arg;
	chRegSetThreadName("blinker2");
	while (true) {
		palClearPad(GPIOG, GPIOG_LED3_GREEN);
		chThdSleepMilliseconds(250);
		palSetPad(GPIOG, GPIOG_LED3_GREEN);
		chThdSleepMilliseconds(250);
	}
}

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

static const ShellCommand commands[] = {
	{NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
	(BaseSequentialStream *)&SDU1,
	commands
};

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
int main(void) {

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	// halInit();
	// chSysInit();
		// Initialize ChibiOS/HAL and ChibiOS/RT. No need to do this as uGFX does that for you. See GFX_OS_NO_INIT setting.
	// halInit();
	// chSysInit();

	// Initialize uGFX (This initializes ChibiOS/HAL and ChibiOS/RT internally. See GFX_OS_NO_INIT setting.
	gfxInit();

	// Prepare some resources
	font1 = gdispOpenFont("DejaVuSans24_aa");
	// gdispImageOpenFile(&img1, "smiley.png");

	// Set the widget defaults
	gwinSetDefaultFont(font1);
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);

	/*
	 * Creating the blinker threads.
	 */
	chThdCreateStatic(waThread1, sizeof(waThread1),
										NORMALPRIO + 10, Thread1, NULL);
	chThdCreateStatic(waThread2, sizeof(waThread2),
										NORMALPRIO + 10, Thread2, NULL);


	// Draw some shapes using the GDISP module. API can be found here: http://api.ugfx.io/group___g_d_i_s_p.html
	gdispClear(White);
	gdispFillArea(10, 10, 50, 120, Blue);
	gdispDrawLine(25, 20, 150, 80, Red);
	gdispDrawCircle(180, 80, 65, Green);

	// Render some text (See https://wiki.ugfx.io/index.php/Font_rendering)
	gdispDrawString(20, 220, "Hello uGFX!", font1, Black);

	// Render an image (See https://wiki.ugfx.io/index.php/Images)
	// Note that we're using the ROMFS to load the image from the microcontrollers flash. (See https://wiki.ugfx.io/index.php/ROMFS)
	// gdispImageDraw(&img1, 50, 80, img1.width, img1.width, 0, 0);

	// Create a slider widget for demo purposes
	{
	   GWidgetInit wi;

	   gwinWidgetClearInit(&wi);

	   wi.g.x = 10;
	   wi.g.y = 260;
	   wi.g.width = 220;
	   wi.g.height = 50;
	   wi.g.show = TRUE;
	   wi.customDraw = 0;
	   wi.customParam = 0;
	   wi.customStyle = 0;
	   wi.text = "Slider";
	   ghSlider1 = gwinSliderCreate(0, &wi);
	   gwinShow(ghSlider1);
	}

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	/*
	 * Initializes a serial-over-USB CDC driver.
	 */
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 * Note, a delay is inserted in order to not have to disconnect the cable
	 * after a reset.
	 */
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);


	/*
	 * Normal main() thread activity, spawning shells.
	 */
	while (true) {
		if (SDU1.config->usbp->state == USB_ACTIVE) {
			thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE,
													"shell", NORMALPRIO + 1,
													shellThread, (void *)&shell_cfg1);
			chThdWait(shelltp);               /* Waiting termination.             */
		}
		// chThdSleepMilliseconds(1000);
		gfxSleepMilliseconds(1000);
	}
}
