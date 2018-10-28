// Copyright 2018 Charles-Henri Mousset
// DRO display for iGaging scales

#if !defined(SCALE_SLAVE_H)
#define SCALE_SLAVE_H

#include "scale.h"

extern int scale_slave_init(SCALEDriver *drvs);
extern int scale_slave24b_init(SCALEDriver *drv);

#endif
