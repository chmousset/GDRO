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

#include "scale_master.h"
#include "scale_slave.h"
#include <stdlib.h>
#include <string.h>


#define TRUE 1
#define FALSE 0

#define N_AXIS		3

SCALEMASTERPrivdata scale_x_privdata;
SCALESLAVEPrivdata scale_y_privdata;
SCALEMASTERPrivdata scale_z_privdata;
SCALEDriver scales[] =
{
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOB,
		.pin_clk = 4,
		.port_data = GPIOB,
		.pin_data = 7,
		.privdata = &scale_x_privdata
	},
	{
		.type = SCALE_SLAVE_24B,
		.port_clk = GPIOA,
		.pin_clk = 7,
		.port_data = GPIOA,
		.pin_data = 5,
		.privdata = &scale_y_privdata
	},
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOC,
		.pin_clk = 3,
		.port_data = GPIOC,
		.pin_data = 8,
		.privdata = &scale_z_privdata
	},
	{.type = 0}
};


// Display widgets
static font_t font1;
static GHandle ghLabel[N_AXIS*2];
static GHandle ghButtonInc[N_AXIS];

static GHandle ghConsole;
static GHandle ghKeyboard;

static GListener gl;

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

// UI thread
static THD_WORKING_AREA(waThreadUI, 4096);
static THD_FUNCTION(ThreadUI, arg) {

	int old_pos[N_AXIS];
	int inc_pos[N_AXIS];
	int disp[N_AXIS];
	int i, j;

	char buf[64], *ptr;
	GEvent* pe;

	bool_t inc_mode[N_AXIS];

	(void)arg;
	chRegSetThreadName("display");
	for(i=0; i<N_AXIS; i++)
		old_pos[i] = 1E9;

	while (true) {

		chThdSleepMilliseconds(100);

		pe = geventEventWait(&gl, 10);
 
		switch(pe->type) {
			case GEVENT_GWIN_BUTTON:
				for(i=0; i<N_AXIS; i++)
				{
					if (((GEventGWinButton*)pe)->gwin == ghButtonInc[i]) {
						inc_mode[i] = !inc_mode[i];
						if(inc_mode[i])
							inc_pos[i] = scales[i].pos_um;
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
			if(old_pos[i] != scales[i].pos_um)
			{
				old_pos[i] = scales[i].pos_um;
				disp[i] = inc_mode[i] ? scales[i].pos_um - inc_pos[i] : scales[i].pos_um;

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
				while(j>=10)
				{
					*ptr++ = '0' + (disp[i] / j);
					disp[i] = disp[i] % j;
					if(j==1000)
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


// Shell setup
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

// Keyboard setup
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

void uiCreateMain(void)
{
	GWidgetInit		wi;
	int i;
 
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

	wi.g.x = 5;
	wi.g.y = 20;
	wi.text = "X";
	ghLabel[i++] = gwinLabelCreate(NULL, &wi);
	wi.g.y += 70;
	wi.text = "Y";
	ghLabel[i++] = gwinLabelCreate(NULL, &wi);
	wi.g.y += 70;
	wi.text = "Z";
	ghLabel[i] = gwinLabelCreate(NULL, &wi);

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

int main(void)
{
	halInit();
	chSysInit();
	gfxInit();

	// Setup scales drivers
	scale_slave_init(scales);
	scale_master_init(scales);

	// Set the widget defaults
	font1 = gdispOpenFont("DejaVuSans32_aa");
	gwinSetDefaultFont(font1);
	gwinSetDefaultStyle(&MyCustomStyle, FALSE);

	gdispClear(Black);
	uiCreateMain();

	// We want to listen for widget events
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	chThdCreateStatic(waThreadUI, sizeof(waThreadUI),
						NORMALPRIO, ThreadUI, NULL);

	// Shell over USB setup
	shellInit();
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

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

bool_t LoadMouseCalibration(unsigned instance, void* data, size_t sz)
{
	(void) instance;
	unsigned int *ptr = (unsigned int*) BKPSRAM_BASE;
	if(*ptr == 0xdeadbeef)
	{
		ptr++;
		memcpy(data, (void*) ptr, sz);
		return true;
	}
	return false;
}

bool_t SaveMouseCalibration (unsigned instance, const void* data, size_t sz)
{
	(void) instance;
	int *ptr = (int*) BKPSRAM_BASE;
	int *bkp = (int*)(BKPSRAM_BASE + sizeof(int));
	memcpy((void*) bkp, data, sz);
	*ptr = 0xdeadbeef;
	return true;
}
