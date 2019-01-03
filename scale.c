// Generic absctract driver for digital scales and calipers
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
#include "scale.h"
#include <string.h>

char axis_label[] = "XYZABCD";


static SCALEDriver scales_default[N_AXIS] =
{
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOA,
		.pin_clk = 0,
		.port_data = GPIOF,
		.pin_data = 10,
		.res = RES_2560cpi,
		.flip = false,
		.pullup = false,
	},
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOF,
		.pin_clk = 9,
		.port_data = GPIOF,
		.pin_data = 8,
		.res = RES_2560cpi,
		.flip = false,
		.pullup = false,
	},
	{
		.type = SCALE_MASTER_IGAGING,
		.port_clk = GPIOF,
		.pin_clk = 7,
		.port_data = GPIOF,
		.pin_data = 6,
		.res = RES_2560cpi,
		.flip = false,
		.pullup = false,
	},
	{
		.type = SCALE_SLAVE_24B,
		.port_clk = GPIOA,
		.pin_clk = 15,
		.port_data = GPIOI,
		.pin_data = 2,
		.res = RES_25600cpi,
		.flip = false,
		.pullup = true,
	},
	{.type = 0}
};

SCALEDriver scales[N_AXIS];

void scale_default(void)
{
	memcpy((void*) scales, (void*) scales_default, sizeof(scales));
}
