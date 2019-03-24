#include <plib.h>                    //调用通用头文件
#include "uart_dma.h"
#include "config.h"
#include "timers.h"
#include "can.h"
#include "SPI.h"
#include "PWM.h"
#include "CNIT.h"
#include "cap.h"
//-------------------------------------------------------------------------------
// POSCMOD = XT, FNOSC = PRIPLL, FWDTEN = OFF
// PLLIDIV = DIV_1, PLLMUL = MUL_20
// PBDIV = 4 (default)
// Main clock = 4MHz /1 * 20    =  80MHz
// Peripheral clock = 80MHz /4  =  40MHz
// SYSCLK = 80 MHz (4MHz Crystal/FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK  = 40 MHz

#pragma config POSCMOD  = XT         //主振荡器  
#pragma config FNOSC    = PRIPLL     //倍频模式 
#pragma config FPLLIDIV = DIV_2      //输入分频 1:1
#pragma config FPLLMUL  = MUL_16    //PPL 倍频 1:20
#pragma config FPLLODIV = DIV_1      //输出分频 1:1
#pragma config FPBDIV   = DIV_2      //外设时钟
#pragma config FWDTEN   = OFF        //关闭看门狗
#pragma config ICESEL   = ICS_PGx1   //调试端口1

void Delay_xmS(unsigned int i)                
{
	unsigned int j;
	for(;i>0;i--)
	{
		Nop();
		for(j=0;j<255;j++)
		{
			Nop();
			ClearWDT();
		}
	}	
}
void main()
{
    //SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);
    unsigned char i;
    unsigned int temp;
    Delay_xmS(2000);
    All_Init();
    while(1)
    {
        if(send_screen==1)
        {
            SendDataToScreen();
            send_screen=0;
            CAN_TX_BAT_VOLT_1();
            CAN_TX_BAT_VOLT_2();
            CAN_TX_BAT_VOLT_3();	     
            CAN_TX_BAT_VOLT_4();
            CAN_TX_SENSOR_VOLT_1();
            CAN_TX_SENSOR_VOLT_2();
            CAN_TX_CON_DATA();
        }
    }
}
