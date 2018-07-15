// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales

#if !defined(SCALE_MASTER_H)
#define SCALE_MASTER_H

#include "scale.h"

typedef struct {
	int32_t pos_temp;
} SCALEMASTERPrivdata;

typedef struct {
	SCALE_DEFAULT_MEMBERS
	SCALEMASTERPrivdata *privdata;
} SCALEMASTERDriver;

extern int scale_master_init(SCALEDriver *drvs);
extern int scale_master_igaging_init(SCALEMASTERDriver *drv);

#endif
