// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales
// Display APP: uses CAN bus to configure a Step10V module

#include "ui.h"
#include "gfx.h"
#include "scale.h"
#include "app_disp.h"
#include "app_thread.h"
#include "app_settings.h"

#define APP_DISP_LABEL_HEIGHT	45
#define APP_DISP_MAX_AXIS ((GDISP_SCREEN_HEIGHT-20) / (APP_DISP_LABEL_HEIGHT+PADDING))
#define APP_DISP_AXIS (APP_DISP_MAX_AXIS > N_AXIS? N_AXIS : APP_DISP_MAX_AXIS)

char axis[] = "XYZABCD";
char default_disp[] = "X  -0000.000";
int old_pos[APP_DISP_AXIS];
int inc_pos[APP_DISP_AXIS];
int disp[APP_DISP_AXIS];
bool_t	inc_mode[APP_DISP_AXIS];
GHandle ghDispButtonAbsInc[APP_DISP_AXIS];
GHandle ghDispLabelPos[APP_DISP_AXIS];
GHandle ghDispTab;

// Callback for all ABS/INC buttons
void cbButtonAbsInc(GEventGWin *we)
{
	int i;
	for(i=0; i<APP_DISP_AXIS; i++)
	{
		if (((GEventGWinButton*)we)->gwin == ghDispButtonAbsInc[i])
		{
			inc_mode[i] = !inc_mode[i];
			old_pos[i] = 0;
			if(inc_mode[i])
				inc_pos[i] = scales[i].pos_um;
			gwinSetText(ghDispButtonAbsInc[i], inc_mode[i] ? "INC" : "ABS", TRUE);
			return;
		}
	}
}


// UI thread
static char waDispThread[4096];
gThreadreturn DispThread(void *arg)
{
	(void)arg;
	int i, j;
	char *ptr;

	chRegSetThreadName("Display");
	for(i=0; i<APP_DISP_AXIS; i++)
		old_pos[i] = 1E9;

	while (true)
	{
		chThdSleepMilliseconds(20);
		for(i=0; i<APP_DISP_AXIS; i++)
		{
			if(old_pos[i] != scales[i].pos_um)
			{
				old_pos[i] = scales[i].pos_um;
				disp[i] = inc_mode[i] ? scales[i].pos_um - inc_pos[i] : scales[i].pos_um;

				// This version does not use sprintf
				ptr = default_disp;
				*ptr++ = axis[i];
				*ptr++ = ' ';
				*ptr++ = ' ';
				if(disp[i] < 0)
				{
					*ptr++ = '-';
					disp[i] = -1 * disp[i];
				}
				else
					*ptr++ = ' ';

				j = 1000000;
				while(j>=10)
				{
					*ptr++ = '0' + (disp[i] / j);
					disp[i] = disp[i] % j;
					if(j==1000)
						*ptr++ = '.';
					j = j/10;
				}
				*ptr++ = 0;
				// sprintf(default_disp, "%c%d.%02d", disp[i] < 0 ? '-' : ' ', abs(disp[i]) / 1000,
				// 		(abs(disp[i]) % 1000) / 10);
				gwinSetText(ghDispLabelPos[i], default_disp, TRUE);
				chThdSleepMilliseconds(1);
			}
		}
		// palTogglePad(GPIOG, GPIOG_LED3_GREEN);
	}
}


// Machine position Labels
void appDispInit(GHandle parent, bool_t singleAppMode)
{
	int i;
	// create widgets
	GWidgetInit	wi;
	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.parent = parent;

	// in multi-app mode, create a new tab
	if(singleAppMode == FALSE)
	{
		ghDispTab = gwinTabsetAddTab(parent, "Display", FALSE);
		wi.g.parent = ghDispTab;
	}

	// Apply the label parameters
	wi.g.y = PADDING;
	wi.g.x = PADDING;
	wi.g.width = 200;
	wi.g.height = APP_DISP_LABEL_HEIGHT;
	wi.text = "X  -000.000";
	for(i=0; i<APP_DISP_MAX_AXIS; i++)
	{
		ghDispLabelPos[i] = gwinLabelCreate(0, &wi);
		wi.g.y += wi.g.height + PADDING;
		
		default_disp[0] = axis[i];
		gwinSetText(ghDispLabelPos[i], default_disp, TRUE);
	}

	wi.g.y = PADDING;
	wi.g.x += wi.g.width + PADDING;
	wi.g.width = 100;
	wi.text = "ABS";
	for(i=0; i<APP_DISP_MAX_AXIS; i++)
	{
		ghDispButtonAbsInc[i] = gwinButtonCreate(0, &wi);
		wi.g.y += wi.g.height + PADDING;
		inc_mode[i] = FALSE;
		uiSimpleCallbackAdd(ghDispButtonAbsInc[i], cbButtonAbsInc);
	}

	gfxThreadCreate((void*)waDispThread, sizeof(waDispThread), NORMALPRIO,
		DispThread, NULL);
}
