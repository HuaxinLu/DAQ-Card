#include "PWM.h"

void Timer3_Init(void)
{
	T3CON = 0X00;                    // Timer2 ��λ
	TMR3= 0x0000;  	              // Timer2 ������ֵ
	PR3 = 0xFFFF;                 // Timer2 ���ڼĴ���
	T3CONbits.TON = 1;            // Timer2 ��������	
}
void PWM_Init(void)
{
	OC1RS = 0xFCFF;               //PWM1
	OC1R  = 0x0000;
	OC1CON= 0x8006;
	OC2RS = 0x8FFF;               //PWM2
	OC2R  = 0x0000;
	OC2CON= 0x800E;
    OC3RS = 0x8FFF;               //PWM2
	OC3R  = 0x0000;
	OC3CON= 0x800E;
}