#include <stdlib.h>
#include <math.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "motion.h"

#undef LINE_ARD_D5
#undef LINE_ARD_D10
#define LINE_ARD_D5                 PAL_LINE(GPIOI, 0U)
#define LINE_ARD_D10                PAL_LINE(GPIOA, 8U)


// IO definition
#define LINE_STEP_X LINE_ARD_D2
#define LINE_STEP_Y LINE_ARD_D3
#define LINE_STEP_Z LINE_ARD_D4
#define LINE_DIR_X LINE_ARD_D5
#define LINE_DIR_Y LINE_ARD_D6
#define LINE_DIR_Z LINE_ARD_D7
#define LINE_EN_XYZ LINE_ARD_D8

// Motion controller parameters
#define STEP_FCY (10800*1000) // frequency of the step pulse generator counter
#define MOTION_LOOP_PERIOD_US 1000 // 1kHz motion loop
#define ISR_MOTION_PERIOD 1e-3
#define ISR_MOTION_PERIOD_INV (1/ISR_MOTION_PERIOD)
#define GPT_STEP GPTD4
#define GPT_MOTION GPTD3

// axis definition
#define MICROSTEPS 32 // units: none
#define STEPS_PER_REV 200 // units: none
#define BALLSCREW_PITCH (4e-3) // unit:m
#define STEPS_PER_M (MICROSTEPS*STEPS_PER_REV/BALLSCREW_PITCH) // units: 1/m
#define STEPS_PER_M_INV (1/STEPS_PER_M) // units: m

// motion limits
#define MAX_SPEED 0.03 // unit: m/s
#define MAX_ACCEL 0.1 // unit: m/s²
#define MAX_STEP_PERIOD_CNT 0xFFFF // assume 16-bits counters

// running variables
float pos_x = 0.0;
int pos_x_steps = 0;
float speed_x = 0.0;
float setpoint_pos_x = 0.0;
int setpoint_pos_x_steps = 0;
float setpoint_speed_x = 0.0;
bool motion_done = true;
int direction_x;
int step_pin_value_x = 0;
int isr_step_period = 0xFFFF;
bool isr_step_period_changed = false;

static const GPTConfig motion_loop_cfg = {
	.frequency =  1E6,
	.callback  =  isr_motion,
	.cr2       =  0U,
	.dier      =  0U
};
static const GPTConfig step_loop_cfg = {
	.frequency =  STEP_FCY,
	.callback  =  isr_step,
	.cr2       =  0U,
	.dier      =  0U
};

void motion_init(void)
{
	palSetLineMode(LINE_STEP_X, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_STEP_Y, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_STEP_Z, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_DIR_X, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_DIR_Y, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_DIR_Z, PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LINE_EN_XYZ, PAL_MODE_OUTPUT_PUSHPULL);

	palClearLine(LINE_STEP_X); // steps on rising edge
	palClearLine(LINE_STEP_Y); // steps on rising edge
	palClearLine(LINE_STEP_Z); // steps on rising edge
	palClearLine(LINE_EN_XYZ); // enable stepper drivers

	gptStart(&GPT_MOTION, &motion_loop_cfg);
	gptStartContinuous(&GPT_MOTION, MOTION_LOOP_PERIOD_US);
	gptStart(&GPT_STEP, &step_loop_cfg);
}

void shlPos(BaseSequentialStream *chp, int argc, char **argv)
{
	if(argc == 0)
	{
		chprintf(chp, "x=%f (%d) done=%d\r\n", pos_x, pos_x_steps, motion_done);
		chprintf(chp, "%f\r\n", setpoint_pos_x * STEPS_PER_M);
		chprintf(chp, "%f\r\n", pos_x * STEPS_PER_M_INV);
		return;
	}
	setpoint_pos_x = atof(argv[0]);
	chprintf(chp, "going to %f\r\n", setpoint_pos_x);
}

void shlEn(BaseSequentialStream *chp, int argc, char **argv)
{
	if(argc == 1) {
		palWriteLine(LINE_EN_XYZ, atoi(argv[0]) ? 0 : 1);
	}
	chprintf(chp, "Power %s\r\n", palReadLine(LINE_EN_XYZ) ? "Disabled" : "Enabled");
}

void isr_step(GPTDriver *drv)
{
	(void) drv;
	if(isr_step_period_changed) {
		gptChangeIntervalI(&GPT_STEP, isr_step_period);
		isr_step_period_changed = false;
	}

	if(setpoint_pos_x_steps == pos_x_steps) {
		return;
	}
	if(step_pin_value_x) {
		palClearLine(LINE_STEP_X);
		step_pin_value_x = 0;
		return;
	}
	palSetLine(LINE_STEP_X);
	step_pin_value_x = 1;
	if(direction_x) {
		pos_x_steps += 1;
	}
	else {
		pos_x_steps -= 1;
	}
}

void isr_motion(GPTDriver *drv)
{
	(void) drv;
	int step_period_cnt;

	palSetLine(LINE_STEP_Y);
	// return;

	// Check if we have to move
	pos_x = STEPS_PER_M_INV * pos_x_steps;
	setpoint_pos_x_steps = (int)(setpoint_pos_x * STEPS_PER_M);
	if(setpoint_pos_x_steps == pos_x_steps) {
		motion_done = true;
		speed_x = 0.0;
		if(GPT_STEP.state != GPT_STOP) {
			chSysLockFromISR();
			gptStopTimerI(&GPT_STEP);
			chSysUnlockFromISR();
		}
		palClearLine(LINE_STEP_Y);
		return; // nothing to do, already at target
	}
	motion_done = false;

	// what is the direction we should go to?
	float dir;
	if (setpoint_pos_x > pos_x) {
		dir = 1.0;
	}
	else {
		dir = -1.0;
	}

	// Calculate what speed the axis should be at
	float max_speed_to_target = sqrt(2.0*fabsf(pos_x - setpoint_pos_x)*MAX_ACCEL);
	if(dir * speed_x > max_speed_to_target) {
		// we are in the 3rd phase of movement (close to setpoint)
		speed_x = dir * max_speed_to_target;
	}
	else {
		// if we are in 1st or 2nd phase of movement
		speed_x += dir * MAX_ACCEL * ISR_MOTION_PERIOD;
	}
	if(dir * speed_x > MAX_SPEED) {
		// if we are in 2nd phase of movement
		speed_x = dir * MAX_SPEED;
	}

	// calculate the step period
	float step_period_f;
	if(speed_x == 0.0) {
		step_period_cnt = MAX_STEP_PERIOD_CNT;
	}
	else {
		step_period_f = STEPS_PER_M_INV * STEP_FCY / fabsf(speed_x);
		if(step_period_f > MAX_STEP_PERIOD_CNT) {
			step_period_cnt = MAX_STEP_PERIOD_CNT;
		}
		else {
			step_period_cnt = (int) step_period_f; 
		}
		if(step_period_cnt < 10) {
			step_period_cnt = 10;
		}
	}

	// setup direction pin
	direction_x = speed_x>0 ? 1 : 0;
	palWriteLine(LINE_DIR_X, direction_x);

	// start (or update) the step timer
	if(GPT_STEP.state == GPT_READY) {
		chSysLockFromISR();
		gptStartContinuousI(&GPT_STEP, step_period_cnt);
		chSysUnlockFromISR();
	}
	else {
		isr_step_period = step_period_cnt/2;
		isr_step_period_changed = true;
	}
	palClearLine(LINE_STEP_Y);
}
