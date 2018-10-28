// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales


#include "ui.h"
#include "gfx.h"
#include "../ugfx/src/gwin/gwin_keyboard_layout.h"
#include "app_disp.h"
#include "app_settings.h"
#include "app_thread.h"

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

static GHandle ghConsole;
static GHandle ghKeyboard;

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

void ui_init(void)
{
	font32 = gdispOpenFont("DejaVuSans32_aa");
	font20 = gdispOpenFont("DejaVuSans20_aa");
	gwinSetDefaultFont(font32);
	gwinSetDefaultStyle(&MyCustomStyle, FALSE);

	gdispClear(Black);
	uiSimpleCallbackInit();
	uiCreateMain();
}

void createKeyboard(void) {
	GWidgetInit		wi;

	gwinWidgetClearInit(&wi);

	// Create the console - set colors before making it visible
	wi.g.show = gFalse;
	wi.g.x = 0; wi.g.y = 0;
	wi.g.width = gdispGetWidth()/2; wi.g.height = gdispGetHeight();
	ghConsole = gwinConsoleCreate(0, &wi.g);
	gwinSetColor(ghConsole, GFX_BLACK);
	gwinSetBgColor(ghConsole, HTML2COLOR(0xF0F0F0));
	gwinShow(ghConsole);
	gwinClear(ghConsole);

	// Create the keyboard
	wi.g.show = gTrue;
	wi.g.x = gdispGetWidth()/2; wi.g.y = 0;
	wi.g.width = gdispGetWidth()/2; wi.g.height = gdispGetHeight();
	ghKeyboard = gwinKeyboardCreate(0, &wi);
}

GHandle		ghTabset;
GHandle		ghPageDisplay, ghPage2, ghPageSettings;
GHandle		ghLabel_1, ghLabel_2, ghLabel_3;

void uiCreateMain(void)
{
	GWidgetInit	wi;

	int v_pitch = 60;


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

void uiSimpleCallbackAdd(GHandle gh, void (*callback)(GHandle))
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

	pe = geventEventWait(&gl, 10);
	if(pe)
	{

		if(pe->type > GEVENT_GWIN_FIRST && pe->type < GEVENT_GADC_FIRST)
		{
			we = (GEventGWin*)pe;
		}
		else
		{
			geventEventComplete(pe);
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
		geventEventComplete(we);
	}
}
