
#if !defined(MOTION_H)
#define MOTION_H

extern float pos_x;
extern int pos_x_steps;
extern float speed_x;
extern float setpoint_pos_x;
extern int setpoint_pos_x_steps;
extern float setpoint_speed_x;
extern bool motion_done;
extern int direction_x;
extern int step_pin_value_x;
extern int isr_step_period;
extern bool isr_step_period_changed;

void motion_init(void);
void isr_step(GPTDriver *drv);
void isr_motion(GPTDriver *drv);
void shlPos(BaseSequentialStream *chp, int argc, char **argv);
void shlEn(BaseSequentialStream *chp, int argc, char **argv);

#endif
