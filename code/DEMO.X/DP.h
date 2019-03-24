#ifndef __DP_H
#define __DP_H 
#include <plib.h>                    //调用通用头文件
#define AD7609_FILTER_NUM 15
extern long CaptureTime1,CaptureTime2;
extern unsigned int Frequency1;
extern unsigned char buffer1[16];
extern unsigned char buffer2[16];
extern unsigned char buffer3[16];
extern unsigned char buffer4[16];
extern unsigned char buffer5[18];
extern int BatVolt[32];
extern int SensorVolt[8];
extern int SensorAmp[8];
extern unsigned char AD7609_Read;;
extern unsigned char Frequency1_Read;
extern int Wfm_Data;
extern int Air_Flow_Data;
extern int Air_Psi_Data;
extern int Air_Motor;
extern int Water_Input_temp;
extern int Water_Output_temp;
void Data_Processing();
#endif