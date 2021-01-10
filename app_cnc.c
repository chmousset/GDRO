// DRO display for iGaging scales
// CNC APP: Generate STEP/DIR motion
// Copyright (C) 2018-2020 Charles-Henri Mousset - ch.mousset@gmail.com
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
#include "app_cnc.h"
#include "motion.h"

#define APP_DISP_LABEL_HEIGHT	45
#define APP_DISP_MAX_AXIS ((GDISP_SCREEN_HEIGHT-20) / (APP_DISP_LABEL_HEIGHT+PADDING))
#define APP_DISP_AXIS (APP_DISP_MAX_AXIS > N_AXIS? N_AXIS : APP_DISP_MAX_AXIS)

char default_cnc[] = "X  -000.000";
GHandle ghCNCButton_m10[APP_DISP_AXIS];
GHandle ghCNCButton_p10[APP_DISP_AXIS];
GHandle ghCNCLabelPos[APP_DISP_AXIS];
GHandle ghCNCTab;

// Callback for all -10, +10 buttons
void cbButtonQuickMove(GEventGWin *we)
{
	int i;
	for(i=0; i<APP_DISP_AXIS; i++)
	{
		if (((GEventGWinButton*)we)->gwin == ghCNCButton_m10[i])
		{
			if(i==0) {
				setpoint_pos_x -= 0.01;
			}
			return;
		}
		if (((GEventGWinButton*)we)->gwin == ghCNCButton_p10[i])
		{
			if(i==0) {
				setpoint_pos_x += 0.01;
			}
			return;
		}
	}
}


// UI thread
static THD_WORKING_AREA(waCNCThread, 256);
gThreadreturn CNCThread(void *arg)
{
	(void)arg;
	char tmp[32];

	chRegSetThreadName("CNC");

	while (true)
	{
		chThdSleepMilliseconds(100);
		tmp[0] = 'X';
		tmp[1] = ' ';
		um2s(tmp+2, (int)(pos_x*1e6));
		gwinSetText(ghCNCLabelPos[0], tmp, TRUE);
	}
}


// Machine position Labels
void appCNCInit(GHandle parent, bool_t singleAppMode)
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
		ghCNCTab = gwinTabsetAddTab(parent, "CNC", FALSE);
		wi.g.parent = ghCNCTab;
	}

	// Apply the label parameters
	wi.g.y = PADDING;
	wi.g.x = PADDING;
	wi.g.width = 210;
	wi.g.height = APP_DISP_LABEL_HEIGHT;
	wi.text = "X -000.000";
	for(i=0; i<APP_DISP_MAX_AXIS; i++)
	{
		ghCNCLabelPos[i] = gwinLabelCreate(0, &wi);
		wi.g.y += wi.g.height + PADDING;
		
		default_cnc[0] = axis_label[i];
		gwinSetText(ghCNCLabelPos[i], default_cnc, TRUE);
	}

	// -10, +10, goto buttons
	wi.g.y = PADDING;
	int left = wi.g.x + wi.g.width + PADDING;
	wi.g.width = 80;
	for(i=0; i<APP_DISP_MAX_AXIS; i++)
	{
		// first the '-10' button on the left
		wi.text = "-10";
		wi.g.x = left;
		ghCNCButton_m10[i] = gwinButtonCreate(0, &wi);
		uiSimpleCallbackAdd(ghCNCButton_m10[i], cbButtonQuickMove);

		// Then the '+10' button
		wi.text = "+10";
		wi.g.x += wi.g.width + PADDING;
		ghCNCButton_p10[i] = gwinButtonCreate(0, &wi);
		uiSimpleCallbackAdd(ghCNCButton_p10[i], cbButtonQuickMove);

		// next row
		wi.g.y += wi.g.height + PADDING;
	}

	gfxThreadCreate((void*)waCNCThread, sizeof(waCNCThread), NORMALPRIO,
		CNCThread, NULL);
}
