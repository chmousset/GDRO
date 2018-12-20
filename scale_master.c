// Driver for scales and calipers that need an external clock
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
#include "scale_master.h"

SCALEDriver *scales_master[SCALE_MAX_INTERFACE];
int scales_master_count;


static THD_WORKING_AREA(waThreadScaleMaster, 128);
static THD_FUNCTION(ThreadScaleMaster, arg) 
{
	static int cnt_bits = 0;
	static int cnt_pauses = 0;
	int i;
	static bool half_bit=true;
	(void) arg;

	(void)arg;
	chRegSetThreadName("scale master");
	while (true) {
		chThdSleep(1);

		if(cnt_pauses)
		{
			cnt_pauses--;
			continue;
		}

		if(cnt_bits < 21)
		{
			if(half_bit)
			{
				half_bit = false;
				for(i=0; i<scales_master_count; i++)
					palSetPad(scales_master[i]->port_clk, scales_master[i]->pin_clk);
				continue;
			}
			half_bit = true;
			
			for(i=0; i<scales_master_count; i++)
			{
				palClearPad(scales_master[i]->port_clk, scales_master[i]->pin_clk);
				scales_master[i]->priv.master.pos_temp +=
					(palReadPad(scales_master[i]->port_data, scales_master[i]->pin_data) ? 1<<cnt_bits : 0);
			}
			cnt_bits++;
		}
		else
		{
			for(i=0; i<scales_master_count; i++)
			{
				if(scales_master[i]->priv.master.pos_temp & (1<<20))
					scales_master[i]->priv.master.pos_temp =
						scales_master[i]->priv.master.pos_temp | 0xFFF00000; 	// sign extension
				if(scales_master[i]->flip)
					scales_master[i]->priv.master.pos_temp = -1 * scales_master[i]->priv.master.pos_temp;
				if(scales_master[i]->res == RES_2560cpi)
					scales_master[i]->priv.master.pos_temp = scales_master[i]->priv.master.pos_temp * 10;
				if(scales_master[i]->res == RES_256cpi)
					scales_master[i]->priv.master.pos_temp = scales_master[i]->priv.master.pos_temp * 100;
				scales_master[i]->priv.master.pos_temp *= 254;
				scales_master[i]->priv.master.pos_temp /= 256;
				scales_master[i]->pos_um = (volatile) scales_master[i]->priv.master.pos_temp;
				scales_master[i]->priv.master.pos_temp = 0;
			}
			cnt_bits = 0;
			cnt_pauses = 40;
			half_bit = true;
			chThdSleepMilliseconds(40);
		}
	}
}

int scale_master_init(SCALEDriver *drvs)
{
	scales_master_count = 0;
	while(drvs->type && (scales_master_count<SCALE_MAX_INTERFACE))
	{
		switch(drvs->type)
		{
			case SCALE_MASTER_IGAGING:
				palSetPadMode(drvs->port_clk, drvs->pin_clk, PAL_MODE_OUTPUT_PUSHPULL);
				palSetPadMode(drvs->port_data, drvs->pin_data, PAL_MODE_INPUT);
				scales_master[scales_master_count++] = (SCALEDriver *) drvs;
				drvs->state = SCALE_RUNNING;
				break;
			default:
				break;
		};
		drvs++;
	}

	chThdCreateStatic(waThreadScaleMaster, sizeof(waThreadScaleMaster),
						HIGHPRIO, ThreadScaleMaster, NULL);
	return 0;
}
