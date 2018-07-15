// Copyright 2018 Charles-Henri Mousset
// Driver for scales and calipers that generate their own clock

#include "ch.h"
#include "hal.h"
#include "scale_slave.h"

SCALESLAVEDriver *scales_slave[SCALE_MAX_INTERFACE];
int scale_slave_count;

EXTConfig extcfg = {
	{
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL},
		{EXT_CH_MODE_DISABLED, NULL}
	}
};

static void scale_slave_24b_cb(EXTDriver *extp, expchannel_t channel)
{
	volatile SCALESLAVEDriver *d;
	int i;

	(void)extp;

	for(i=0; i<scale_slave_count; i++)
	{
		d = scales_slave[i];
		if(d->pin_clk != channel)
			continue;

		if(chVTGetSystemTimeX() - d->privdata->time_last > MS2ST(100))
		{
			d->privdata->bits = 0;
			d->privdata->pos_temp = 0;
		}
		d->privdata->time_last = chVTGetSystemTimeX();

		if(d->privdata->bits++ <= 23)
			d->privdata->pos_temp = (d->privdata->pos_temp >> 1) |
				(palReadPad(d->port_data, d->pin_data) ? (1<<23) : 0);
		if(d->privdata->bits == 24)
		{
			if(d->privdata->pos_temp & 0x100000)
				d->pos_um = -10 * (d->privdata->pos_temp & 0xFFFFF);
			else
				d->pos_um = 10 * (d->privdata->pos_temp & 0xFFFFF);
			d->privdata->bits = 0;
			d->privdata->pos_temp = 0;
		}
		return;
	}
}

int scale_slave_io_init(SCALESLAVEDriver *drv)
{
	palSetPadMode(drv->port_clk, drv->pin_clk, PAL_MODE_INPUT);
	palSetPadMode(drv->port_data, drv->pin_data, PAL_MODE_INPUT);
	if(extcfg.channels[drv->pin_clk].mode != EXT_CH_MODE_DISABLED)
		return -1;
	
	if(drv->port_clk == GPIOA)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA;
#if defined(GPIOB)
	else if(drv->port_clk == GPIOB)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB;
#endif
#if defined(GPIOC)
	else if(drv->port_clk == GPIOC)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC;
#endif
	else
		return -2;
	return 0;
}

int scale_slave24b_init(SCALESLAVEDriver *drv)
{
	extcfg.channels[drv->pin_clk].cb = scale_slave_24b_cb;

	drv->pos_um = 0;
	drv->privdata->bits = 0;
	drv->privdata->time_last = chVTGetSystemTimeX();
	drv->state = SCALE_RUNNING;
	return 0;
}

int scale_slave_init(SCALEDriver *drvs)
{
	scale_slave_count = 0;
	int ret;
	while(drvs->type && (scale_slave_count<SCALE_MAX_INTERFACE))
	{
		switch(drvs->type)
		{
			case SCALE_SLAVE_24B:
				ret = scale_slave_io_init((SCALESLAVEDriver *) drvs);
				if(ret)
					return ret;
				ret = scale_slave24b_init((SCALESLAVEDriver *) drvs);
				if(ret)
					return ret;
				scales_slave[scale_slave_count++] = (SCALESLAVEDriver *) drvs;
				break;
			default:
				break;
		};
		drvs++;
	}
	extStart(&EXTD1, &extcfg);
	return 0;
}
