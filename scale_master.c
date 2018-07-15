// Copyright 2018 Charles-Henri Mousset
// Driver for scales and calipers that need an external clock

#include "ch.h"
#include "hal.h"
#include "scale_master.h"

SCALEMASTERDriver *scales_master[SCALE_MAX_INTERFACE];
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
				scales_master[i]->privdata->pos_temp +=
					(palReadPad(scales_master[i]->port_data, scales_master[i]->pin_data) ? 1<<cnt_bits : 0);
			}
			cnt_bits++;
		}
		else
		{
			for(i=0; i<scales_master_count; i++)
			{
				if(scales_master[i]->privdata->pos_temp & (1<<20))
					scales_master[i]->privdata->pos_temp =
						scales_master[i]->privdata->pos_temp | 0xFFF00000; 	// sign extension
				scales_master[i]->pos_um = scales_master[i]->privdata->pos_temp * 10;
				scales_master[i]->privdata->pos_temp = 0;
			}
			cnt_bits = 0;
			cnt_pauses = 40;
			half_bit = true;
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
				scales_master[scales_master_count++] = (SCALEMASTERDriver *) drvs;
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
