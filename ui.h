// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales

#if !defined(UI_H)
#define UI_H

#include "gfx.h"

#define PADDING		5

extern void createKeyboard(void);
extern void uiCreateMain(void);
extern void ui_init(void);
extern void createKeyboard(void);

void uiSimpleCallbackInit(void);
void uiSimpleCallbackAdd(GHandle gh, void (*callback)(GHandle));

extern font_t font20;

struct EVENTWATCHER
{
	GHandle gh;
	void (*callback)(GEvent *);
#if !defined(UI_CALLBACK_STATIC_CNT)
	struct EVENTWATCHER *next;
#endif
};
typedef struct EVENTWATCHER eventwatcher_t;

#endif
