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

// Pitch display and selection
GHandle ghThreadButtonPitch;
GHandle ghThreadLabelPitch;
void cbButtonPitch(GEventGWin *we)
{

}

GHandle ghThreadButtonSetHome;
void cbButtonSetHome(GEventGWin *we)
{

}

GHandle ghThreadButtonSetEnd;
void cbButtonEnd(GEventGWin *we)
{

}

GHandle ghThreadButtonSetHome;
void cbButtonGotoHome(GEventGWin *we)
{

}

// Machine position Labels
GHandle ghThreadLabelPosX;
GHandle ghThreadLabelPosY;
GHandle ghThreadLabelPosA;
GHandle ghThreadTab;
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


}

void appThreadStart()
{
	// Periodic task to send/receive CAN frames and display position
}

