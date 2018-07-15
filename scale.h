// Copyright 2018 Charles-Henri Mousset
// Generic absctract driver for digital scales and calipers

#if !defined(SCALES_H)
#define SCALES_H

#define SCALE_MAX_INTERFACE	3

typedef enum scale_type_e
{
	SCALE_END = 0,
	SCALE_SLAVE_24B=1,
	SCALE_MASTER_IGAGING,
	SCALE_QEI			// Quadrature encoder interface
} scale_type;

typedef enum scale_state_e
{
	SCALE_UNINIT = 0,
	SCALE_RUNNING
} scale_state;

#define SCALE_DEFAULT_MEMBERS \
	scale_type type;		/* Type of scale, used to init the scales 		*/ \
	scale_state state;														   \
	int32_t pos_um;			/* position in micrometer						*/ \
	ioportid_t port_clk;	/* port of clock (or chan A of QEI)             */ \
	unsigned int pin_clk;	/* pin of clock (or chan A of QEI) pin          */ \
	ioportid_t port_data;	/* port of data (or chan B of QEI) pin          */ \
	unsigned int pin_data;	/* pin of data (or chan B of QEI) pin           */

typedef struct
{
	SCALE_DEFAULT_MEMBERS
	void *privdata;			/* drivers generally need extra data to operate */ \
} SCALEDriver;

#endif
