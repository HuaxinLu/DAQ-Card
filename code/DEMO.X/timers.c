#include "timers.h"
#include "uart_dma.h"
#include "Control.h"
#include "DP.h"
unsigned short SysTick = 0;       //计数
unsigned char time_flag1;
unsigned char time_flag2;
unsigned char send_screen;
unsigned char send_can_control;
unsigned char send_can_bat;
void Timer1_init()
{
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, T1_TICK);
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_2);
}
void __ISR(_TIMER_1_VECTOR, ipl2) Timer1Handler(void)//任务调度：10ms
{
    mT1ClearIntFlag();
    Data_Processing();
    time_flag1++;
    time_flag2++;
    if(time_flag1==20)//200ms 
    {
        time_flag1=0;
        send_screen=1;//写屏幕
    }
    if(time_flag2==100)//1s
    {
        if(Temp_Start==0xff)
            Water_Control();
        time_flag2=0;
    }
    
}
