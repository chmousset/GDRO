// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales

#if !defined(SCALE_SLAVE_H)
#define SCALE_SLAVE_H

#include "scale.h"

typedef struct {
	int32_t pos_temp;
	uint8_t bits;
	int time_last;
} SCALESLAVEPrivdata;

typedef struct {
	SCALE_DEFAULT_MEMBERS
	SCALESLAVEPrivdata *privdata;
} SCALESLAVEDriver;

extern int scale_slave_init(SCALEDriver *drvs);
extern int scale_slave24b_init(SCALESLAVEDriver *drv);

#endif
