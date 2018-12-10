// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales
// Settings app. Allow to modify and save GDRO settings

#include "ui.h"
#include "gfx.h"
#include "scale.h"
#include "flash.h"

#define APP_DISP_LABEL_HEIGHT	45

#define SECT_7 0x080C0000
#define CONFIG_DATA (SECT_7 + sizeof(uint32_t))

// Settings widgets
static GHandle ghLabelSettingsAxis;
static GHandle ghLabelSettingsProtocol;
static GHandle ghLabelSettingsResolution;
static GHandle ghListProtocol;
static GHandle ghListAxis;
static GHandle ghListResolution;
static GHandle ghCheckboxFlipAxis;
static GHandle ghButtonSave;
static GHandle ghButtonLoad;
GHandle ghSettingsTab;

void cbSettingsLoad(GEventGWin *we)
{
	(void) we;
	uint32_t size_data = * ((uint32_t *) SECT_7);
	uint8_t *ptr = (uint8_t *) scales;
	unsigned int i;

	if(size_data != sizeof(scales))
		return;	// saved data is invalid

	for(i=0; i<sizeof(scales); i++)
		ptr[i] = ((uint8_t *) CONFIG_DATA)[i];

	cbSettingsAxis(NULL);	// refresh the menu
}

void cbSettingsSave(GEventGWin *we)
{
	(void) we;
	uint32_t size_data = sizeof(scales);
	uint8_t *ptr = (uint8_t *) &size_data;
	unsigned int i = 0;
	flash_unlock();

	flash_erase(7);			// erase sector 7 where we save the configuration
	flash_wait_done(50000);	// let's give the FLASH plenty of time to do so

	// the first word is used to check that the configuration is valid
	for(i=0; i<sizeof(uint32_t); i++)
		flash_prog_byte(SECT_7+i, ptr[i]);

	// Now we save the configuration itself
	ptr = (uint8_t *) scales;
	for(i=0; i<sizeof(scales); i++)
		flash_prog_byte(CONFIG_DATA+i, ptr[i]);
	flash_lock();
}

void cbSettingsAxis(GEventGWin *we)
{
	(void) we;
	SCALEDriver *s = &scales[gwinListGetSelected(ghListAxis)];
	switch(s->type) {
		default:
			s->type = SCALE_DISABLE;
			/* Falls through. */
		case SCALE_DISABLE:
			/* Falls through. */
		case SCALE_SLAVE_24B:
			/* Falls through. */
		case SCALE_MASTER_IGAGING:
			/* Falls through. */
		case SCALE_QEI:
			gwinListSetSelected(ghListProtocol, s->type, gTrue);
	};
	switch(s->res) {
		default:
			s->res = RES_256cpi;
			/* Falls through. */
		case RES_256cpi:
			/* Falls through. */
		case RES_2560cpi:
			gwinListSetSelected(ghListResolution, s->res, gTrue);
	};
	gwinCheckboxCheck(ghCheckboxFlipAxis, s->flip);
}

void cbSettingsApply(GEventGWin *we)
{
	(void) we;
	SCALEDriver *s = &scales[gwinListGetSelected(ghListAxis)];
	s->type = gwinListGetSelected(ghListProtocol);
	s->res = gwinListGetSelected(ghListResolution);
	s->flip = gwinCheckboxIsChecked(ghCheckboxFlipAxis);
}

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

	wi.g.height = gdispGetHeight() - 40;
	wi.g.x = 0;
	wi.g.y = 30;
	ghListAxis = gwinListCreate(0, &wi, gFalse);
	gwinListSetScroll(ghListAxis, scrollSmooth);
	gwinSetFont(ghListAxis, font20);

	char tmp[2] = {0, 0};
	for(i=0; i<N_AXIS; i++)
	{
		tmp[0] = axis_label[i];
		gwinListAddItem(ghListAxis, tmp, gTrue);
	}

	wi.g.width = gdispGetWidth()/4; wi.g.height = 20;
	wi.g.x = gdispGetWidth()/4;
	wi.g.y = 5;
	wi.text = "Protocol";
	ghLabelSettingsProtocol = gwinLabelCreate(0, &wi);
	gwinSetFont(ghLabelSettingsProtocol, font20);

	wi.g.height = gdispGetHeight() - 40;
	wi.g.x = gdispGetWidth()/4;
	wi.g.y = 30;
	ghListProtocol = gwinListCreate(0, &wi, gFalse);
	gwinListSetScroll(ghListProtocol, scrollSmooth);
	gwinSetFont(ghListProtocol, font20);

	gwinListAddItem(ghListProtocol, "-", gFalse);
	gwinListAddItem(ghListProtocol, "24bits", gFalse);
	gwinListAddItem(ghListProtocol, "iGaging", gFalse);
	gwinListAddItem(ghListProtocol, "QEI", gFalse);


	wi.g.height = 20;
	wi.g.x = gdispGetWidth()/2;
	wi.g.y = 5;
	wi.text = "Resolution";
	ghLabelSettingsResolution = gwinLabelCreate(0, &wi);
	gwinSetFont(ghLabelSettingsResolution, font20);

	wi.g.height = gdispGetHeight() - 40;
	wi.g.x = gdispGetWidth()/2;
	wi.g.y = 30;
	ghListResolution = gwinListCreate(0, &wi, gFalse);
	gwinListSetScroll(ghListResolution, scrollSmooth);
	gwinSetFont(ghListResolution, font20);

	gwinListAddItem(ghListResolution, "256cpi", gFalse);
	gwinListAddItem(ghListResolution, "2560cpi", gFalse);

	// Flip axis checkbox
	wi.g.width = gdispGetWidth()/4-PADDING;		// includes text
	wi.g.height = 40;
	wi.g.y += PADDING;
	wi.g.x = gdispGetWidth() - gdispGetWidth()/4 + PADDING;
	wi.text = "Flip";
	ghCheckboxFlipAxis = gwinCheckboxCreate(0, &wi);
	gwinSetFont(ghCheckboxFlipAxis, font20);

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


	uiSimpleCallbackAdd(ghListAxis, cbSettingsAxis);
	uiSimpleCallbackAdd(ghButtonSave, cbSettingsSave);
	uiSimpleCallbackAdd(ghButtonLoad, cbSettingsLoad);
	uiSimpleCallbackAdd(ghCheckboxFlipAxis, cbSettingsApply);
	uiSimpleCallbackAdd(ghListProtocol, cbSettingsApply);
	uiSimpleCallbackAdd(ghListResolution, cbSettingsApply);

	cbSettingsLoad(NULL);	// restore the defaults
}
