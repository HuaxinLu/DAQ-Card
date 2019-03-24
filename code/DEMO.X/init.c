#include "uart_dma.h"
#include "config.h"
#include "timers.h"
#include "can.h"
#include "SPI.h"
#include "PWM.h"
#include "CNIT.h"
#include "cap.h"
#include "Control.h"

void IO_Init(void)
{
    AD1PCFG = 0xFFFF;             //����Ϊ����I/O
    //AD7606��������
    PORTSetPinsDigitalOut(IOPORT_D, BIT_15);//RD15 SCK
    PORTSetPinsDigitalOut(IOPORT_A, BIT_2);//RA2 CS
    PORTSetPinsDigitalIn(IOPORT_F, BIT_2);//RF2 DI
    PORTSetPinsDigitalIn(IOPORT_B, BIT_15);//RB15 BUSY
    PORTSetPinsDigitalOut(IOPORT_A, BIT_3);//RA3 RESET
    PORTSetPinsDigitalOut(IOPORT_D, BIT_0);//RD0 CONV
    mPORTAClearBits(BIT_3);//RESET�õ�
     Delay_xmS(500);
    mPORTASetBits(BIT_3);//RESET�õ�
    Delay_xmS(500);
    mPORTAClearBits(BIT_3);//RESET�õ�
    mPORTASetBits(BIT_2);//CS�ø�
    //ģ�⿪������
    PORTSetPinsDigitalOut(IOPORT_B, BIT_14);//RB14 EN
    //PORTSetPinsDigitalOut(IOPORT_D, BIT_10);//RD10 A0
    //PORTSetPinsDigitalOut(IOPORT_D, BIT_11);//RD11 A1
    PORTSetPinsDigitalIn(IOPORT_D, BIT_10);//RD10 A0
    PORTSetPinsDigitalIn(IOPORT_D, BIT_11);//RD11 A1
    mPORTBSetBits(BIT_14);//EN�ø�
    //mPORTDClearBits(BIT_10);//A0�õ�
    //mPORTDClearBits(BIT_11);//A1�õ�
    //AD7609��AD5686����
    PORTSetPinsDigitalOut(IOPORT_G, BIT_6);//RG6 SCK
    PORTSetPinsDigitalOut(IOPORT_C, BIT_1);//RC1 CS2(AD5686Ƭѡ)
    PORTSetPinsDigitalOut(IOPORT_C, BIT_2);//RC2 CS1(AD7609Ƭѡ)
    PORTSetPinsDigitalIn(IOPORT_G, BIT_7);//RG7 DI
    PORTSetPinsDigitalOut(IOPORT_G, BIT_8);//RG8 DO
    PORTSetPinsDigitalIn(IOPORT_B, BIT_5);//RB5 BUSY
    PORTSetPinsDigitalOut(IOPORT_B, BIT_4);//RB4 RESET
    PORTSetPinsDigitalOut(IOPORT_D, BIT_1);//RD0 CONV
    _LATB4=0;
    Delay_xmS(500); 
    _LATB4=1;
    Delay_xmS(500);
    _LATB4=0;
    mPORTCSetBits(BIT_1);//CS�ø�
    mPORTCSetBits(BIT_2);//CS�ø�
    //LED����
    PORTSetPinsDigitalOut(IOPORT_B, BIT_8|BIT_9|BIT_10);
    mPORTBClearBits(BIT_8|BIT_9|BIT_10);
    //��������
    PORTSetPinsDigitalIn(IOPORT_F, BIT_4);//RF4 U2RX
    PORTSetPinsDigitalOut(IOPORT_F, BIT_5);//RF5 U2TX  
    PORTSetPinsDigitalIn(IOPORT_F, BIT_12);//RF12 U2RX
    PORTSetPinsDigitalOut(IOPORT_F, BIT_13);//RF13 U2TX  
    //����IO
    PORTSetPinsDigitalOut(IOPORT_E, BIT_4);
    PORTSetPinsDigitalOut(IOPORT_E, BIT_3);
    PORTSetPinsDigitalOut(IOPORT_E, BIT_2);
    PORTSetPinsDigitalOut(IOPORT_G, BIT_13);
    
    PORTSetPinsDigitalOut(IOPORT_G, BIT_12);
    PORTSetPinsDigitalOut(IOPORT_G, BIT_14);
    PORTSetPinsDigitalOut(IOPORT_E, BIT_0);
    PORTSetPinsDigitalOut(IOPORT_E, BIT_1);
    
    PORTSetPinsDigitalOut(IOPORT_A, BIT_7);
    PORTSetPinsDigitalOut(IOPORT_A, BIT_6); 
    PORTSetPinsDigitalOut(IOPORT_D, BIT_3);
    PORTSetPinsDigitalOut(IOPORT_D, BIT_2);     
    
    PORTSetPinsDigitalIn(IOPORT_D, BIT_8);
    PORTSetPinsDigitalIn(IOPORT_D, BIT_9);     
    _LATE4=1;
    _LATE3=1;
    _LATE2=1;
    _LATG13=1;
    
    _LATG12=1;
    _LATG14=1;
    _LATE0=1;
    _LATE1=1;
    
    _LATA7=1;
    _LATA6=1;
    _LATD3=1;
    _LATD2=1;
    
}
void Clock_Init()
{
    OSCConfig(OSC_POSC, OSC_PLL_MULT_20, OSC_PLL_POST_1, OSC_FRC_POST_1);					//ѡ���ڲ�FRC 8M ����
	mOSCSetPBDIV(OSC_PB_DIV_8);				//����ʱ�ӷ�Ƶ��ѡ��
}
void Display_Init()
{
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
    Uart5_SendDataBuffer(light_control,6);
}
void All_Init()
{
    IO_Init();//IO�ڳ�ʼ��
    Timer1_init();
    Cap_Init();
    Timer3_Init();
    PWM_Init();
    CNIT_Init();
    SPI2_Init();
    SPI3_Init();
    //UART2_init();
    UART5_init();
    CAN_Init();
    DMA_Init();
    Display_Init();
    Clear_Output();
    INTEnableSystemMultiVectoredInt(); //ʹ���ж϶�������  
}