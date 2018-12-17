// DRO display for iGaging scales
// Threading APP: uses CAN bus to configure a Step10V module
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
#include "chprintf.h"

float pitch, home, end, oldpitch, oldhome, oldend;
int oldx, oldy, olda;
uint32_t oldqei;

// Machine position Labels
GHandle ghThreadLabelPosX;
GHandle ghThreadLabelPosY;
GHandle ghThreadLabelPosA;
GHandle ghThreadTab;
// Pitch display and selection
GHandle ghThreadButtonPitch;
GHandle ghThreadLabelPitch;

void cbButtonPitch(GEventGWin *we)
{
	(void) we;

	keyboard_getfloat(&pitch);

}

GHandle ghThreadButtonSetHome;
void cbButtonSetHome(GEventGWin *we)
{
	(void) we;
	keyboard_getfloat(&home);

}

GHandle ghThreadButtonSetEnd;
void cbButtonEnd(GEventGWin *we)
{
	(void) we;
	keyboard_getfloat(&end);
}

GHandle ghThreadButtonSetHome;
void cbButtonGotoHome(GEventGWin *we)
{
	(void) we;

}

// UI thread
static THD_WORKING_AREA(waThreadThread, 128+256);
gThreadreturn ThreadThread(void *arg)
{
	(void)arg;
	char tmp[30];

	chRegSetThreadName("Thread");
	pitch = 0.0;
	end = 0.0;
	home = 0.0;
	oldpitch = 0.0;
	oldend = 0.0;
	oldhome = 0.0;

	while(true)
	{
		chThdSleepMilliseconds(20);
		if(pitch != oldpitch)
		{
			chsnprintf(tmp, 30, "P  %.3f", pitch);
			gwinSetText(ghThreadLabelPitch, tmp, TRUE);
			oldpitch = pitch;
		}
		if(oldx != scales[2].pos_um)
		{
			tmp[0] = 'X'; tmp[1] = ' ';
			um2s(&tmp[2], scales[2].pos_um);
			gwinSetText(ghThreadLabelPosX, tmp, TRUE);
			oldx = scales[2].pos_um;
		}
		if(oldy != scales[0].pos_um)
		{
			tmp[0] = 'Y'; tmp[1] = ' ';
			um2s(&tmp[2], scales[0].pos_um);
			gwinSetText(ghThreadLabelPosY, tmp, TRUE);
			oldy = scales[0].pos_um;
		}
		if(oldqei != GPTD8.tim->CNT)
		{
			oldqei = GPTD8.tim->CNT;
			tmp[0] = 'A'; tmp[1] = ' ';
			um2s(&tmp[2], (oldqei * 254)/6);
			gwinSetText(ghThreadLabelPosA, tmp, TRUE);
		}
	}
}

void appThreadInit(GHandle parent, bool_t singleAppMode)
{
	// create widgets
	GWidgetInit	wi;
	// Apply some default values for GWIN
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.parent = parent;

	// in multi-app mode, create a new tab
	if(singleAppMode == FALSE)
	{
		ghThreadTab = gwinTabsetAddTab(parent, "Thread", FALSE);
		wi.g.parent = ghThreadTab;
	}

	// Apply the label parameters
	wi.g.y = PADDING;
	wi.g.x = PADDING;
	wi.g.width = gdispGetWidth()/2-PADDING*2;
	wi.g.height = 40;
	wi.text = "X  -000.000";
	ghThreadLabelPosX = gwinLabelCreate(0, &wi);

	wi.g.y += wi.g.height + PADDING;
	wi.text = "Y  -000.000";
	ghThreadLabelPosY = gwinLabelCreate(0, &wi);

	wi.g.y += wi.g.height + PADDING;
	wi.text = "A  -000.000";
	ghThreadLabelPosA = gwinLabelCreate(0, &wi);

	wi.g.y += wi.g.height + PADDING;
	wi.text = "P  -000.000";
	ghThreadLabelPitch = gwinLabelCreate(0, &wi);

	wi.g.y = PADDING;
	wi.g.x += wi.g.width;
	wi.text = "Set Home";
	ghThreadButtonSetHome = gwinButtonCreate(0, &wi);

	wi.g.y += wi.g.height + PADDING;
	wi.text = "Set End";
	ghThreadButtonSetEnd = gwinButtonCreate(0, &wi);

	wi.g.y += wi.g.height + PADDING;
	wi.text = "Set Pitch";
	ghThreadButtonPitch = gwinButtonCreate(0, &wi);

	// associate callbacks and widgets
	uiSimpleCallbackAdd(ghThreadButtonPitch, cbButtonPitch);
	uiSimpleCallbackAdd(ghThreadButtonSetHome, cbButtonSetHome);

	// start thread
	gfxThreadCreate((void*)waThreadThread, sizeof(waThreadThread), NORMALPRIO,
		ThreadThread, NULL);

}
