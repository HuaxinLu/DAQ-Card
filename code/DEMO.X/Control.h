#ifndef __CONTROL_H
#define __CONTROL_H 
#include <plib.h>                    //����ͨ��ͷ�ļ�
extern unsigned char Temp_Start;
extern int Pump_Vol;
void Clear_Output();
void Water_Control();
typedef struct
{
    float Kp;
    float Ki;
    float Kd;
    float error[3];
    float Expect;
    float Output;
    float OutputMax;
    float OutputMin;
}PID_DATA;
PID_DATA Temperature;
#endif