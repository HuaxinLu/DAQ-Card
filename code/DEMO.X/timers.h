#ifndef __TIMERS_H
#define __TIMERS_H 
#include <plib.h>                    //调用通用头文件


// Let compile time pre-processor calculate the PR1 (period)
#define SYS_FREQ 			(64000000L)
#define PB_DIV         		2
#define PRESCALE       		256
#define TOGGLES_PER_SEC		100
#define T1_TICK       		(SYS_FREQ/PB_DIV/PRESCALE/TOGGLES_PER_SEC)

extern unsigned short SysTick;       //计数
extern unsigned char time_flag1;
extern unsigned char cap1_overflow;
extern unsigned char cap1_error;
extern unsigned char send_can_control;
extern unsigned char send_can_bat;
void Timer1_init();
#endif