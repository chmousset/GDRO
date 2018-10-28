// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales
// Settings app. Allow to modify and save GDRO settings

#include "ui.h"
#include "gfx.h"

#define APP_DISP_LABEL_HEIGHT	45

// Callback for axis select list
GHandle ghSettingsListAxis;
void cbListAxis(GEventGWin *we)
{

}

// Machine position Labels
static GHandle ghLabelSettingsAxis;
static GHandle ghLabelSettingsProtocol;
static GHandle ghLabelSettingsResolution;
static GHandle ghListProtocol;
static GHandle ghListAxis;
static GHandle ghListResolution;
static GHandle ghCheckboxFlipAxis;
static GHandle ghCheckboxEnableAxis;
static GHandle ghButtonApply;
static GHandle ghButtonSave;
static GHandle ghButtonLoad;
GHandle ghSettingsTab;
void appSettingsInit(GHandle parent, bool_t singleAppMode)
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
		ghSettingsTab = gwinTabsetAddTab(parent, "Settings", FALSE);
		wi.g.parent = ghSettingsTab;
	}

	wi.g.width = gdispGetWidth()/4; wi.g.height = 20; wi.g.x = 0, wi.g.y = 5;
	wi.text = "Axis";
	ghLabelSettingsAxis = gwinLabelCreate(0, &wi);
	gwinSetFont(ghLabelSettingsAxis, font20);

	wi.g.height = gdispGetHeight() / 2;
	wi.g.x = 0;
	wi.g.y = 30;
	wi.text = "Axis";
	ghListAxis = gwinListCreate(0, &wi, gFalse);
	gwinListSetScroll(ghListAxis, scrollSmooth);
	gwinSetFont(ghListAxis, font20);

	gwinListAddItem(ghListAxis, "X", gFalse);
	gwinListAddItem(ghListAxis, "Y", gFalse);
	gwinListAddItem(ghListAxis, "Z", gFalse);
	gwinListAddItem(ghListAxis, "G", gFalse);
	gwinListAddItem(ghListAxis, "A", gFalse);

	wi.g.width = gdispGetWidth()/4; wi.g.height = 20;
	wi.g.x = gdispGetWidth()/4;
	wi.g.y = 5;
	wi.text = "Protocol";
	ghLabelSettingsProtocol = gwinLabelCreate(0, &wi);
	gwinSetFont(ghLabelSettingsProtocol, font20);

	wi.g.height = gdispGetHeight() / 2;
	wi.g.x = gdispGetWidth()/4;
	wi.g.y = 30;
	wi.text = "Protocol";
	ghListProtocol = gwinListCreate(0, &wi, gFalse);
	gwinListSetScroll(ghListProtocol, scrollSmooth);
	gwinSetFont(ghListProtocol, font20);

	gwinListAddItem(ghListProtocol, "iGaging", gFalse);
	gwinListAddItem(ghListProtocol, "24bits", gFalse);
	gwinListAddItem(ghListProtocol, "24x2bits", gFalse);


	wi.g.height = 20;
	wi.g.x = gdispGetWidth()/2;
	wi.g.y = 5;
	wi.text = "Resolution";
	ghLabelSettingsResolution = gwinLabelCreate(0, &wi);
	gwinSetFont(ghLabelSettingsResolution, font20);

	wi.g.height = gdispGetHeight() / 2;
	wi.g.x = gdispGetWidth()/2;
	wi.g.y = 30;
	wi.text = "Resolution";
	ghListResolution = gwinListCreate(0, &wi, gFalse);
	gwinListSetScroll(ghListResolution, scrollSmooth);
	gwinSetFont(ghListResolution, font20);

	gwinListAddItem(ghListResolution, "10um", gFalse);
	gwinListAddItem(ghListResolution, "1um", gFalse);
	gwinListAddItem(ghListResolution, "0.0005\"", gFalse);

	// Enable axis checkbox
	wi.g.width = gdispGetWidth()/4-PADDING;		// includes text
	wi.g.height = 20;
	wi.g.y = PADDING;
	wi.g.x = gdispGetWidth() - gdispGetWidth()/4 + PADDING;
	wi.text = "Enable";
	ghCheckboxEnableAxis = gwinCheckboxCreate(0, &wi);
	gwinSetFont(ghCheckboxEnableAxis, font20);

	// Flip axis checkbox
	wi.g.width = gdispGetWidth()/4-PADDING;		// includes text
	wi.g.height = 20;
	wi.g.y += PADDING + wi.g.height;
	wi.g.x = gdispGetWidth() - gdispGetWidth()/4 + PADDING;
	wi.text = "Flip";
	ghCheckboxFlipAxis = gwinCheckboxCreate(0, &wi);
	gwinSetFont(ghCheckboxFlipAxis, font20);

	// Apply button
	wi.g.y += PADDING + wi.g.height;
	wi.g.height = 30;
	wi.text = "Apply";	
	ghButtonApply = gwinButtonCreate(0, &wi);
	gwinSetFont(ghButtonApply, font20);

	// Save button
	wi.g.y += PADDING + wi.g.height;
	wi.text = "Save";
	ghButtonSave = gwinButtonCreate(0, &wi);
	gwinSetFont(ghButtonSave, font20);

	// Load button
	wi.g.y += PADDING + wi.g.height;
	wi.text = "Load";
	ghButtonLoad = gwinButtonCreate(0, &wi);
	gwinSetFont(ghButtonLoad, font20);
}
