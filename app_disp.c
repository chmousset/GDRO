// DRO display for iGaging scales
// Display APP: uses CAN bus to configure a Step10V module
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
#include "scale.h"
#include "app_disp.h"
#include "app_thread.h"
#include "app_settings.h"

#define APP_DISP_LABEL_HEIGHT	45
#define APP_DISP_MAX_AXIS ((GDISP_SCREEN_HEIGHT-20) / (APP_DISP_LABEL_HEIGHT+PADDING))
#define APP_DISP_AXIS (APP_DISP_MAX_AXIS > N_AXIS? N_AXIS : APP_DISP_MAX_AXIS)

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
static THD_WORKING_AREA(waDispThread, 256);
gThreadreturn DispThread(void *arg)
{
	(void)arg;
	int i;
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
				*ptr++ = axis_label[i];
				*ptr++ = ' ';

				um2s(ptr, disp[i]);
				gwinSetText(ghDispLabelPos[i], default_disp, TRUE);
				chThdSleepMilliseconds(1);
			}
		}
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
	wi.g.width = 210;
	wi.g.height = APP_DISP_LABEL_HEIGHT;
	wi.text = "X -000.000";
	for(i=0; i<APP_DISP_MAX_AXIS; i++)
	{
		ghDispLabelPos[i] = gwinLabelCreate(0, &wi);
		wi.g.y += wi.g.height + PADDING;
		
		default_disp[0] = axis_label[i];
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
