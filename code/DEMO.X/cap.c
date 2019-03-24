#include "cap.h"
#include "uart_dma.h"
#include "timers.h"
#include "DP.h"
unsigned char cap1_overflow;
unsigned char cap1_error;

void Timer2_Init(void)
{
	T2CON = 0X20;                    // Timer2 ��λ
	IFS0bits.T2IF = 0;      // Timer2 �����־λ
	IPC2bits.T2IP = 2;      // Timer2 �ж����ȼ�
    IPC2bits.T2IS = 1;      // Timer2 �ж����ȼ�
	IEC0bits.T2IE = 1;      // Timer2 �ж�����
	TMR2= 0x0000;  	              // Timer2 ������ֵ
	PR2 = 0xFFFF;                 // Timer2 ���ڼĴ���
	T2CONbits.TON = 1;            // Timer2 ��������	
}
void Cap_Init()
{
    Timer2_Init();
    ConfigIntCapture1(IC_INT_ON | IC_INT_PRIOR_2 | IC_INT_SUB_PRIOR_3);
    OpenCapture1( IC_EVERY_FALL_EDGE | IC_INT_1CAPTURE | IC_TIMER2_SRC | IC_FEDGE_FALL | IC_ON );
}
void __attribute__((vector(5)))__attribute__((interrupt(IPL4)))_ICInterrupt(void)
{
    long temp;
    long a;
    long b;
    temp=mIC1ReadCapture();
    b = TMR2;
    if(b<temp)
        cap1_overflow--;
    if(CaptureTime1!=0)
    {
        CaptureTime2=temp+cap1_overflow*0XFFFF-CaptureTime1;
        Frequency1_Read=1;
    }

        
    CaptureTime1=temp;
    cap1_overflow=0;
    mIC1ClearIntFlag();
}
//---------------------------------------------------------------------------------
//	Timer2 �жϺ���
//---------------------------------------------------------------------------------
unsigned int Count = 0;      //����
void __attribute__ ((vector(8)))__attribute__((interrupt(IPL4)))_T2Interrupt(void)
{	
	mT2ClearIntFlag();
    cap1_overflow++;
    if(cap1_overflow>=6)
    {
        CaptureTime2=0;
        CaptureTime1=0;
        cap1_overflow=6;
    }
    
}