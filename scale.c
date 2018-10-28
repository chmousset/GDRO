// Copyright 2018 Charles-Henri Mousset
// Generic absctract driver for digital scales and calipers

#include "ch.h"
#include "scale.h"

SCALEDriver scales[] =
{
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOA,
		.pin_clk = 0,
		.port_data = GPIOF,
		.pin_data = 10,
	},
	{
		.type = SCALE_SLAVE_24B,
		.port_clk = GPIOF,
		.pin_clk = 9,
		.port_data = GPIOF,
		.pin_data = 8,
	},
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOF,
		.pin_clk = 7,
		.port_data = GPIOF,
		.pin_data = 6,
	},
	{.type = 0}
};
