// Copyright 2018 Charles-Henri Mousset
// Generic absctract driver for digital scales and calipers

#include "ch.h"
#include "scale.h"

char axis_label[] = "XYZABCD";

SCALEDriver scales[N_AXIS] =
{
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOA,
		.pin_clk = 0,
		.port_data = GPIOF,
		.pin_data = 10,
		.res = RES_256cpi,
		.flip = false
	},
	{
		.type = SCALE_SLAVE_24B,
		.port_clk = GPIOF,
		.pin_clk = 9,
		.port_data = GPIOF,
		.pin_data = 8,
		.res = RES_256cpi,
		.flip = false,
	},
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOF,
		.pin_clk = 7,
		.port_data = GPIOF,
		.pin_data = 6,
		.res = RES_256cpi,
		.flip = false,
	},
	{.type = 0}
};
