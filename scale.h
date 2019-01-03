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

#if !defined(SCALES_H)
#define SCALES_H

#include "hal.h"

#define SCALE_MAX_INTERFACE	3
#define N_AXIS 6

extern char axis_label[];

typedef enum scale_resolution
{
	RES_256cpi,
	RES_2560cpi,
	RES_25600cpi
} scale_resolution;

typedef enum scale_type_e
{
	SCALE_DISABLE = 0,
	SCALE_SLAVE_24B=1,
	SCALE_MASTER_IGAGING,
	SCALE_QEI			// Quadrature encoder interface
} scale_type;

typedef enum scale_state_e
{
	SCALE_UNINIT = 0,
	SCALE_RUNNING
} scale_state;

typedef struct {
	int32_t pos_temp;
	uint8_t bits;
	int time_last;
} SCALESLAVEPrivdata;

typedef struct {
	int32_t pos_temp;
} SCALEMASTERPrivdata;

typedef struct
{
	scale_type type;		/* Type of scale, used to init the scales 		*/
	scale_state state;
	int32_t pos_um;			/* position in micrometer						*/
	ioportid_t port_clk;	/* port of clock (or chan A of QEI)             */
	unsigned int pin_clk;	/* pin of clock (or chan A of QEI) pin          */
	ioportid_t port_data;	/* port of data (or chan B of QEI) pin          */
	unsigned int pin_data;	/* pin of data (or chan B of QEI) pin           */
	scale_resolution res;	/* resolution of the scale                      */
	bool flip;				/* Axis is flipped */
	bool pullup;			/* Enable pullups on the inputs                 */
	union
	{
		SCALEMASTERPrivdata master;
		SCALESLAVEPrivdata slave;
	} priv;					/* privatedata, scale type dependant */

} SCALEDriver;

extern SCALEDriver scales[N_AXIS];
void scale_default(void);

#endif
