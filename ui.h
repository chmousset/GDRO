// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales

#if !defined(UI_H)
#define UI_H

#include "gfx.h"

#define PADDING		5

struct EVENTWATCHER
{
	GHandle gh;
	void (*callback)(GEventGWin *);
#if !defined(UI_CALLBACK_STATIC_CNT)
	struct EVENTWATCHER *next;
#endif
};
typedef struct EVENTWATCHER eventwatcher_t;

extern void createKeyboard(void);
extern void uiCreateMain(void);
extern void ui_init(void);

eventwatcher_t *uiSimpleCallbackGetNextEmpty(void);
extern void uiSimpleCallbackInit(void);
extern void uiSimpleCallbackAdd(GHandle gh, void (*callback)(GEventGWin *));
void uiSimpleCallbackLoop(void);

extern font_t font20;

#endif
