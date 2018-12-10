// DRO display for iGaging scales
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
