// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales
// Threading APP: uses CAN bus to configure a Step10V module

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

