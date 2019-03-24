#ifndef __PI_H__
#define __PI_H__

extern int PIFlag0;

//PID
struct PID
{
	const float K_p;
	const float K_i;
	const float K_d;
	float error[3];
	float setpoint;
	float Plusewidth_U;
	const int Plusewidth_Umax;
	float Ratio_U;
};

struct Limit
{
	const int max_PluseWidth;
	const int min_PluseWidth;
};

extern struct PID PID_Vo;
extern struct PID PID_Io;

static struct Limit Limit_Vo={1540,200};
//extern double PIDCalc(struct PID *, int);
extern void Vo_PID(double);
extern void Io_PID(double);

#endif
