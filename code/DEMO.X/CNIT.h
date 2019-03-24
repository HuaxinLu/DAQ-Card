#ifndef __CNIT_H
#define __CNIT_H 
#include <plib.h>                    //调用通用头文件
extern unsigned char Write_AD5684_A_Flag;
extern unsigned char Write_AD5684_B_Flag;
extern unsigned char Write_AD5684_C_Flag;
extern unsigned char Write_AD5684_D_Flag;
extern unsigned int AD5684_A_Data;
extern unsigned int AD5684_B_Data;
extern unsigned int AD5684_C_Data;
extern unsigned int AD5684_D_Data;
void CNIT_Init(void);
void Clock_Init();
#endif