// Driver for scales and calipers that generate their own clock
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

#include "ch.h"
#include "hal.h"
#include "scale_slave.h"

SCALEDriver *scales_slave[SCALE_MAX_INTERFACE];
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
	volatile SCALEDriver *d;
	int i;

	(void)extp;

	for(i=0; i<scale_slave_count; i++)
	{
		d = scales_slave[i];
		if(d->pin_clk != channel)
			continue;

		if(chVTGetSystemTimeX() - d->priv.slave.time_last > MS2ST(100))
		{
			d->priv.slave.bits = 0;
			d->priv.slave.pos_temp = 0;
		}
		d->priv.slave.time_last = chVTGetSystemTimeX();

		if(d->priv.slave.bits++ <= 23)
			d->priv.slave.pos_temp = (d->priv.slave.pos_temp >> 1) |
				(palReadPad(d->port_data, d->pin_data) ? (1<<23) : 0);
		if(d->priv.slave.bits == 24)
		{
			d->priv.slave.pos_temp &= 0xFFFFF;
			if(d->priv.slave.pos_temp & 0x100000)
				d->priv.slave.pos_temp *= -1;

			if(d->res == RES_2560cpi)
				d->priv.slave.pos_temp *= 10;
			if(d->res == RES_256cpi)
				d->priv.slave.pos_temp *= 100;

			d->pos_um = (volatile) d->priv.slave.pos_temp;
			d->priv.slave.bits = 0;
			d->priv.slave.pos_temp = 0;
		}
		return;
	}
}

int scale_slave_io_init(SCALEDriver *drv)
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
#if defined(GPIOD)
	else if(drv->port_clk == GPIOD)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOD;
#endif
#if defined(GPIOE)
	else if(drv->port_clk == GPIOE)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
#endif
#if defined(GPIOF)
	else if(drv->port_clk == GPIOF)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
#endif
#if defined(GPIOG)
	else if(drv->port_clk == GPIOG)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOG;
#endif
#if defined(GPIOH)
	else if(drv->port_clk == GPIOH)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOH;
#endif
#if defined(GPIOI)
	else if(drv->port_clk == GPIOI)
		extcfg.channels[drv->pin_clk].mode = 
			EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOI;
#endif
	else
		return -2;
	drv->state = SCALE_RUNNING;
	return 0;
}

int scale_slave24b_init(SCALEDriver *drv)
{
	extcfg.channels[drv->pin_clk].cb = scale_slave_24b_cb;

	drv->pos_um = 0;
	drv->priv.slave.bits = 0;
	drv->priv.slave.time_last = chVTGetSystemTimeX();
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
				ret = scale_slave_io_init((SCALEDriver *) drvs);
				if(ret)
					return ret;
				ret = scale_slave24b_init((SCALEDriver *) drvs);
				if(ret)
					return ret;
				scales_slave[scale_slave_count++] = (SCALEDriver *) drvs;
				break;
			default:
				break;
		};
		drvs++;
	}
	extStart(&EXTD1, &extcfg);
	return 0;
}
