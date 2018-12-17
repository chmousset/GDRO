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


#include "ui.h"
#include "gfx.h"
#include "../ugfx/src/gwin/gwin_keyboard_layout.h"
#include "app_disp.h"
#include "app_settings.h"
#include "app_thread.h"
#include <stdlib.h>
#include <string.h>

// #define UI_CALLBACK_STATIC_CNT 100
#if defined(UI_CALLBACK_STATIC_CNT)
eventwatcher_t ui_callback_list[UI_CALLBACK_STATIC_CNT];
#else
eventwatcher_t ui_callback_list;
#endif


// Keyboard setup
static const GVSpecialKey kbKeys[] = {
	{ "\010", "\b", 0, 0 },							// \005 (5)	= Backspace
	{ "\015", "\r", 0, 0 },								// \006 (6)	= Enter 1
};
static const char *kbRows[] = {"789", "456", "123", "-0.", "X\001\002", 0 };
static const GVKeySet kbSet[] = { kbRows, 0 };
const GVKeyTable kbNumPad = { kbKeys, kbSet };


// Display widgets
font_t font32;
font_t font20;

static GHandle ghTabset;
static GHandle ghKeyboardFrame;
static GHandle ghConsole;
static GHandle ghKeyboard;
static GHandle ghButtonValueCancel;
static GHandle ghButtonValueOk;
float *kbValuePtr;
char kbStr[32] = "";
unsigned int kbStrIdx = 0;

static GListener gl;

const GWidgetStyle MyCustomStyle = {
	HTML2COLOR(0x000000),			// window background
	HTML2COLOR(0x2A8FCD),			// focused

	// enabled color set
	{
		HTML2COLOR(0xfffc42),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x505050),		// fill
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
		HTML2COLOR(0xfffc42),		// text
		HTML2COLOR(0x404040),		// edge
		HTML2COLOR(0x808080),		// fill
		HTML2COLOR(0x00E000)		// progress - active area
	}
};


// UI thread
static THD_WORKING_AREA(waUIThread, 512);
gThreadreturn UIThread(void *arg)
{
	(void) arg;
#if (GFX_USE_OS_CHIBIOS == GFXON)
	chRegSetThreadName("UI main");
#endif
	while(1)
	{
		gfxSleepMilliseconds(10);
		uiSimpleCallbackLoop();
	}
}

void cbButtonCancel(GEventGWin *we)
{
	(void) we;
	gwinHide(ghKeyboardFrame);
	gwinShow(ghTabset);
}

void cbButtonOk(GEventGWin *we)
{
	if(kbValuePtr)
		*kbValuePtr = (float)atof(kbStr);
	cbButtonCancel(we);
}

void cbKeyboard(GEventKeyboard *pk)
{
	int i;

	i = 0;
	while (pk->bytecount--)
	{
		if(pk->c[i] == 0x08)	// backspace
		{
			kbStr[kbStrIdx] = 0;
			if(kbStrIdx)
				kbStrIdx--;
		}
		else if(pk->c[i] == 2)	// ok key
			cbButtonOk(NULL);
		else if(pk->c[i] == 'X')
			cbButtonCancel(NULL);
		else
		{
			if(kbStrIdx < sizeof(kbStr)-1)
			{
				kbStr[kbStrIdx++] = pk->c[i];
				kbStr[kbStrIdx] = 0;
			}
		}
		gwinPrintf(ghConsole, "%02X\n", pk->c[i]);
		i++;
	}
}

static GHandle ghButtonValueOk;
void createKeyboard(void)
{
	GWidgetInit wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = FALSE;

	// Apply the frame parameters
	wi.g.width = GDISP_SCREEN_WIDTH;
	wi.g.height = GDISP_SCREEN_HEIGHT;
	wi.g.y = 0;
	wi.g.x = 0;
	wi.text = "Enter Number";
	ghKeyboardFrame = gwinFrameCreate(0, &wi, 0);
	wi.g.show = TRUE;
	wi.g.parent = ghKeyboardFrame;

	// Create the console - set colors before making it visible
	wi.g.x = 0; wi.g.y = 0;
	wi.g.width /= 2; wi.g.height -= 60;
	ghConsole = gwinConsoleCreate(0, &wi.g);
	gwinSetColor(ghConsole, GFX_BLACK);
	gwinSetBgColor(ghConsole, HTML2COLOR(0xF0F0F0));
	gwinClear(ghConsole);

	// Apply the button parameters
	wi.g.y += wi.g.height;
	wi.g.x = 0;
	wi.g.width = GDISP_SCREEN_WIDTH/4;
	wi.g.height = 40;
	wi.text = "Cancel";
	ghButtonValueCancel = gwinButtonCreate(0, &wi);

	wi.g.x += wi.g.width;
	wi.text = "OK";
	ghButtonValueOk = gwinButtonCreate(0, &wi);

	// Create the keyboard
	wi.g.x = GDISP_SCREEN_WIDTH/2; wi.g.y = 0;
	wi.g.width = wi.g.x;
	wi.g.height = GDISP_SCREEN_HEIGHT-20;
	ghKeyboard = gwinKeyboardCreate(0, &wi);

	kbValuePtr = NULL;

	uiSimpleCallbackAdd(ghButtonValueOk, cbButtonOk);
	uiSimpleCallbackAdd(ghButtonValueCancel, cbButtonCancel);
}

void keyboard_getfloat(float *value)
{
	kbValuePtr = value;
	kbStrIdx = 0;
	gwinHide(ghTabset);
	gwinShow(ghKeyboardFrame);
}


void uiCreateMain(void)
{
	GWidgetInit	wi;

	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	// Create the Tabset
	wi.g.width = gdispGetWidth(); wi.g.height = gdispGetHeight(); wi.g.x = 0, wi.g.y = 0;
	ghTabset = gwinTabsetCreate(0, &wi, GWIN_TABSET_BORDER);
	gwinSetFont(ghTabset, font20);

	appDispInit(ghTabset, FALSE);
	appThreadInit(ghTabset, FALSE);
	appSettingsInit(ghTabset, FALSE);
}

void ui_init(void)
{
	font32 = gdispOpenFont("DejaVuSans32_aa");
	font20 = gdispOpenFont("DejaVuSans20_aa");
	gwinSetDefaultFont(font32);
	gwinSetDefaultStyle(&MyCustomStyle, FALSE);

	gdispClear(Black);
	uiSimpleCallbackInit();
	uiCreateMain();

	// Create keyboard and attach the key press events
	createKeyboard();
	geventAttachSource(&gl, gwinKeyboardGetEventSource(ghKeyboard), GLISTEN_KEYTRANSITIONS|GLISTEN_KEYUP);

	gfxThreadCreate(waUIThread, sizeof(waUIThread), NORMALPRIO, UIThread, NULL);
}

int cb_cnt;

void uiSimpleCallbackInit(void)
{
#if defined(UI_CALLBACK_STATIC_CNT)
	ui_callback_list[0].callback = NULL;
#else
	ui_callback_list.next = NULL;
	ui_callback_list.callback = NULL;
#endif

	geventListenerInit(&gl);
	gwinAttachListener(&gl);
	cb_cnt = 0;
}

eventwatcher_t *uiSimpleCallbackGetNextEmpty(void)
{
	cb_cnt = 0;
#if defined(UI_CALLBACK_STATIC_CNT)
	int i;
	for(i=0; i<UI_CALLBACK_STATIC_CNT; i++)
	{
		if(ui_callback_list[i].callback == NULL)
			return &ui_callback_list[i];
		cb_cnt++;
	}
	return NULL;
#else
	eventwatcher_t *ptr = &ui_callback_list;
	while(ptr->next)
	{
		ptr = ptr->next;
		cb_cnt++;
	}
	if(ptr->callback)
	{
		ptr->next = gfxAlloc(sizeof(eventwatcher_t));
		if(ptr->next == NULL)
			return NULL;
		ptr = ptr->next;
		ptr->next = NULL;
	}
	return ptr;
#endif
}

void uiSimpleCallbackAdd(GHandle gh, void (*callback)(GEventGWin *))
{
	eventwatcher_t *ptr;
	ptr = uiSimpleCallbackGetNextEmpty();
	if(ptr == NULL || callback == NULL)
		return;

	ptr->callback = callback;
	ptr->gh = gh;
}

void uiSimpleCallbackLoop(void)
{
	GEvent *pe;
	GEventGWin *we;

	pe = geventEventWait(&gl, gDelayForever);
	if(pe)
	{
		if(pe->type == GEVENT_KEYBOARD)
		{
			cbKeyboard((GEventKeyboard *)pe);
			return;
		}
		if(pe->type > GEVENT_GWIN_FIRST && pe->type < GEVENT_GADC_FIRST)
		{
			we = (GEventGWin*)pe;
		}
		else
		{
			// geventEventComplete(&gl);
			return;
		}

#if defined(UI_CALLBACK_STATIC_CNT)
		int i;
		for(i=0; i<UI_CALLBACK_STATIC_CNT; i++)
		{
			if(ui_callback_list[i].callback == NULL)
				break;
			if(ui_callback_list[i].gh == we->gwin)
			{
				ui_callback_list[i].callback(we);
				break;
			}
		}
#else
		eventwatcher_t *ptr = &ui_callback_list;
		while(ptr)
		{
			if(ptr->gh == we->gwin)
			{
				ptr->callback(we);
				break;
			}
			ptr = ptr->next;
		}
#endif
		// geventEventComplete(&gl);
	}
}

// micrometer to string. Make sure your string is >= 8 chars
void um2s(char *ptr, int um)
{
	int j = 1000000;
	if(um < 0)
	{
		*ptr++ = '-';
		um = -1 * um;
	}
	else
		*ptr++ = ' ';

	while(j>=1)
	{
		*ptr++ = '0' + (um / j);
		um = um % j;
		if(j==1000)
			*ptr++ = '.';
		j = j/10;
	}
	*ptr++ = 0;
}
