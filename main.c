// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales

#include "ch.h"
#include "hal.h"
#include "ch_test.h"
#include "chprintf.h"
#include "shell.h"

#include "usbcfg.h"

#include "gfx.h"
#include "ugfx/src/gwin/gwin_keyboard_layout.h"

static GHandle ghSlider1;
static font_t font1;

#define TRUE 1
#define FALSE 0

#define N_AXIS		3

// Display widgets
static GHandle ghLabel[N_AXIS];
static GHandle   ghButtonInc[N_AXIS];

static GHandle   ghCheckbox1;

static GHandle		ghConsole;
static GHandle		ghKeyboard;

static GListener gl;

int pos_um[N_AXIS];

const GWidgetStyle MyCustomStyle = {
	HTML2COLOR(0x000000),			// window background
	HTML2COLOR(0x2A8FCD),			// focused
 
	// enabled color set
	{
		HTML2COLOR(0xfffc42),		// text
		HTML2COLOR(0x000000),		// edge
		HTML2COLOR(0x101010),		// fill
		HTML2COLOR(0x00E000)		// progress - active area
	},
 
	// disabled color set
	{
		HTML2COLOR(0xC0C0C0),		// text
		HTML2COLOR(0x808080),		// edge
		HTML2COLOR(0xE0E0E0),		// fill
		HTML2COLOR(0xC0E0C0)		// progress - active area
	},
 
	// pressed color set
	{
		HTML2COLOR(0x404040),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x808080),		// fill
		HTML2COLOR(0x00E000)		// progress - active area
	}
};

void input_scale_loop()
{
	static int cnt_bits = 0;
	static int cnt_pauses = 0;
	static int pos[N_AXIS];
	int i;
	static bool_t half_bit=true;

	if(cnt_pauses)
	{
		cnt_pauses--;
		return;
	}

	if(cnt_bits < 21)
	{
		if(half_bit)
		{
			half_bit = false;
			palSetPad(GPIOB, 4);
			return;
		}
		half_bit = true;
		palClearPad(GPIOB, 4);
		cnt_bits++;
		
		pos[0] = pos[0] + (palReadPad(GPIOB, 7) ? 1<<cnt_bits : 0);
		// pos[1] = pos[1] + (palReadPad(GPIOB, 7) ? 1<<cnt_bits : 0);
		// pos[2] = pos[2] + (palReadPad(GPIOB, 7) ? 1<<cnt_bits : 0);
		pos[1] = pos[2] = pos[0];
	}
	else if(cnt_bits++ == 21)
	{
		for(i=0; i<N_AXIS; i++)
		{
			if(pos[i] & (1<<20))
				pos[i] = pos[i] | 0xFFF00000; 	// sign extension
			pos_um[i] = pos[i] * 10;
			pos[i] = 0;
		}
		cnt_bits = 0;
		cnt_pauses = 40;
		half_bit = true;
	}
}

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


static THD_WORKING_AREA(waThreadScale, 128);
static THD_FUNCTION(ThreadScale, arg) {
	int cnt_bits = 0;
	int pos[N_AXIS];
	int i;
	
	(void)arg;
	chRegSetThreadName("scale");
	palSetPadMode(GPIOB, 4, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPadMode(GPIOB, 7, PAL_MODE_INPUT);
	while (true) {
		// if(cnt_bits++ < 21)
		// {
		// 	palSetPad(GPIOB, 4);
		// 	chThdSleep(1);
		// 	palClearPad(GPIOB, 4);
		// 	pos[0] = pos[0] + (palReadPad(GPIOB, 7) ? 1<<cnt_bits : 0);
		// 	pos[1] = pos[1] + (palReadPad(GPIOB, 7) ? 1<<cnt_bits : 0);
		// 	pos[2] = pos[2] + (palReadPad(GPIOB, 7) ? 1<<cnt_bits : 0);
		// 	chThdSleep(1);
		// }
		// else
		// {
		// 	for(i=0; i<N_AXIS; i++)
		// 	{
		// 		if(pos[i] & (1<<20))
		// 			pos[i] = pos[i] | 0xFFF00000; 	// sign extension
		// 		pos_um[i] = pos[i] * 10;
		// 		pos[i] = 0;
		// 	}
		// 	cnt_bits = 0;
		// 	chThdSleep(40*2);
		// }
		input_scale_loop();
		chThdSleep(1);
	}
}

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread2, 4096);
static THD_FUNCTION(Thread2, arg) {

	int old_pos[N_AXIS];
	int inc_pos[N_AXIS];
	int disp[N_AXIS];
	int i, j;

	char buf[64], *ptr;
	GEvent* pe;

	bool_t inc_mode[N_AXIS];
	bool_t zero_mode = false;

	(void)arg;
	chRegSetThreadName("display");
	for(i=0; i<N_AXIS; i++)
		old_pos[i] = 1E9;

	while (true) {

		chThdSleepMilliseconds(100);

		if(!zero_mode)

		pe = geventEventWait(&gl, 10);
 
		switch(pe->type) {
			case GEVENT_GWIN_BUTTON:
				for(i=0; i<N_AXIS; i++)
				{
					if (((GEventGWinButton*)pe)->gwin == ghButtonInc[i]) {
						inc_mode[i] = !inc_mode[i];
						if(inc_mode[i])
							inc_pos[i] = pos_um[i];
						gwinSetText(ghButtonInc[i], inc_mode[i] ? "INC" : "ABS", TRUE);
						old_pos[i] = 1E9;	// forces refresh
					}
				}
				break;
			default:
				break;
		};

		for(i=0; i<N_AXIS; i++)
		{
			if(old_pos[i] != pos_um[i])
			{
				old_pos[i] = pos_um[i];
				disp[i] = inc_mode[i] ? pos_um[i] - inc_pos[i] : pos_um[i];

				// This version does not use sprintf
				ptr = buf;
				if(disp[i] < 0)
				{
					*ptr++ = '-';
					disp[i] = -1 * disp[i];
				}
				else
					*ptr++ = ' ';

				j = 100000;
				while(j)
				{
					*ptr++ = '0' + (disp[i] / j);
					disp[i] = disp[i] % j;
					if(j==100)
						*ptr++ = '.';
					j = j/10;
				}
				*ptr++ = 0;
				// sprintf(buf, "%c%d.%02d", disp[i] < 0 ? '-' : ' ', abs(disp[i]) / 1000,
				// 		(abs(disp[i]) % 1000) / 10);
				gwinSetText(ghLabel[i], buf, TRUE);
				chThdSleepMilliseconds(1);
			}
		}
		palTogglePad(GPIOG, GPIOG_LED3_GREEN);
	}
}

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

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
	{"rotate", shl_rotate},
	{NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
	(BaseSequentialStream *)&SDU1,
	commands
};


static const GVSpecialKey kbKeys[] = {
	{ "\010", "\b", 0, 0 },							// \005 (5)	= Backspace
	{ "\015", "\r", 0, 0 },								// \006 (6)	= Enter 1
};
static const char *kbRows[] = {"789", "456", "123", "-0.", "X\001\002", 0 };
static const GVKeySet kbSet[] = { kbRows, 0 };
const GVKeyTable kbNumPad = { kbKeys, kbSet };


static void createKeyboard(void) {
	GWidgetInit		wi;

	gwinWidgetClearInit(&wi);

	// Create the console - set colors before making it visible
	wi.g.show = FALSE;
	wi.g.x = 0; wi.g.y = 0;
	wi.g.width = gdispGetWidth(); wi.g.height = gdispGetHeight()/4;
	ghConsole = gwinConsoleCreate(0, &wi.g);
	gwinSetColor(ghConsole, GFX_BLACK);
	gwinSetBgColor(ghConsole, HTML2COLOR(0xF0F0F0));
	gwinShow(ghConsole);
	gwinClear(ghConsole);

	// Create the keyboard
	wi.g.show = TRUE;
	wi.g.x = 0; wi.g.y = gdispGetHeight()/4;
	wi.g.width = gdispGetWidth(); wi.g.height = gdispGetHeight()*3/4;
	ghKeyboard = gwinKeyboardCreate(0, &wi);
}



/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
int main(void)
{
	int i;

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
		// Initialize ChibiOS/HAL and ChibiOS/RT. No need to do this as uGFX does that for you. See GFX_OS_NO_INIT setting.
	halInit();
	chSysInit();


	// Initialize uGFX (This initializes ChibiOS/HAL and ChibiOS/RT internally. See GFX_OS_NO_INIT setting.
	gfxInit();

	/*
	 * Creating the blinker threads.
	 */
	// chThdCreateStatic(waThread1, sizeof(waThread1),
	// 									NORMALPRIO + 10, Thread1, NULL);

	// Prepare some resources
	font1 = gdispOpenFont("DejaVuSans32_aa");
	// gdispImageOpenFile(&img1, "smiley.png");

	// Set the widget defaults
	gwinSetDefaultFont(font1);
	gwinSetDefaultStyle(&MyCustomStyle, FALSE);



	// Draw some shapes using the GDISP module. API can be found here: http://api.ugfx.io/group___g_d_i_s_p.html
	gdispClear(Black);
	// gdispFillArea(10, 10, 50, 120, Blue);
	// gdispDrawLine(25, 20, 150, 80, Red);
	// gdispDrawCircle(180, 80, 65, Green);

	// Render some text (See https://wiki.ugfx.io/index.php/Font_rendering)
	gdispDrawString(5, 8+10+5, "X", font1, Green);
	gdispDrawString(5, 8+10+75, "Y", font1, Green);
	gdispDrawString(5, 8+10+145, "Z", font1, Green);

	// Render an image (See https://wiki.ugfx.io/index.php/Images)
	// Note that we're using the ROMFS to load the image from the microcontrollers flash. (See https://wiki.ugfx.io/index.php/ROMFS)
	// gdispImageDraw(&img1, 50, 80, img1.width, img1.width, 0, 0);

	// Create a slider widget for demo purposes
	// {
	// 	 GWidgetInit wi;

	// 	 gwinWidgetClearInit(&wi);

	// 	 wi.g.x = 0;
	// 	 wi.g.y = 200;
	// 	 wi.g.width = 280;
	// 	 wi.g.height = 40;
	// 	 wi.g.show = TRUE;
	// 	 wi.customDraw = 0;
	// 	 wi.customParam = 0;
	// 	 wi.customStyle = 0;
	// 	 wi.text = "Slider";
	// 	 ghSlider1 = gwinSliderCreate(0, &wi);
	// 	 gwinShow(ghSlider1);
	// }
	{
		GWidgetInit		wi;
	 
		// Apply some default values for GWIN
		wi.customDraw = 0;
		wi.customParam = 0;
		wi.customStyle = 0;
		wi.g.show = TRUE;
	 
		// Apply the label parameters	
		wi.g.y = 20;
		wi.g.x = 40;
		wi.g.width = 155;
		wi.g.height = 40;
		wi.text = "Label 1";
	 
		// Create the actual label
		for(i=0; i<N_AXIS; i++)
		{
			ghLabel[i] = gwinLabelCreate(NULL, &wi);
			wi.g.y += 70;
		}
	}

	// {
	// 	GWidgetInit	wi;
	 
	// 	// Apply some default values for GWIN
	// 	wi.customDraw = 0;
	// 	wi.customParam = 0;
	// 	wi.customStyle = 0;
	// 	wi.g.show = TRUE;
	 
	// 	// Apply the checkbox parameters	
	// 	wi.g.width = 100;		// includes text
	// 	wi.g.height = 40;
	// 	wi.g.y = 10;
	// 	wi.g.x = 200;
	// 	wi.text = "INC";
	 
	// 	// Create the actual checkbox 
	// 	ghCheckbox1 = gwinCheckboxCreate(NULL, &wi);
	// }
	{
		GWidgetInit	wi;
	 
		// Apply some default values for GWIN
		gwinWidgetClearInit(&wi);
		wi.g.show = TRUE;
	 
		// Apply the button parameters	
		wi.g.width = 100;
		wi.g.height = 60;
		wi.g.y = 10;
		wi.g.x = 220;
		wi.text = "ABS";
	 
		// Create the actual button
		for(i=0; i<N_AXIS; i++)
		{
			ghButtonInc[i] = gwinButtonCreate(NULL, &wi);
			wi.g.y += 70;
		}
	}
	// We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	chThdCreateStatic(waThreadScale, sizeof(waThreadScale),
						HIGHPRIO, ThreadScale, NULL);
	chThdCreateStatic(waThread2, sizeof(waThread2),
						NORMALPRIO, Thread2, NULL);

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
		chThdSleepMilliseconds(100);
	}
}
