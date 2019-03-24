#include "CNIT.h"
#include "DP.h"
#include "uart_dma.h"
unsigned char int_flag1,int_flag2;
unsigned char Write_AD5684_A_Flag;
unsigned char Write_AD5684_B_Flag;
unsigned char Write_AD5684_C_Flag;
unsigned char Write_AD5684_D_Flag;
unsigned int AD5684_A_Data;
unsigned int AD5684_B_Data;
unsigned int AD5684_C_Data;
unsigned int AD5684_D_Data;
void CNIT_Init(void)
{
	CNENbits.CNEN7   = 1;         //CN7 使能
    CNENbits.CNEN12   = 1;         //CN12 使能
	CNPUEbits.CNPUE7 = 1;         // CN7 弱上拉
    CNPUEbits.CNPUE12 = 1;         // CN12 弱上拉
	IFS1bits.CNIF    = 0;         // 清除标志位
	IPC6bits.CNIP    = 4;         // 中断主优先级
	IPC6bits.CNIS    = 1;         // 中断子优先级
	IEC1bits.CNIE    = 1;         // 电平变化中断	
	CNCONbits.ON = 1;             //使能 CN 模块	
} 
void AD7606_Read_Data()
{
    unsigned char i;
    _LATA2=0;
    if(_RD10==0&&_RD11==0)
    {
        for(i=0; i<16; i++)                
            buffer1[i] = SPI3_ReadByte();   //读取接收到的数据 	          
        for(i=0; i<8; i++) 
            BatVolt[i]=(int)((buffer1[i+i]<<8|buffer1[i+i+1])*0.00406f+0.5f);
    }
    else if(_RD10==0&&_RD11==1)
    {
        for(i=0; i<16; i++)                
            buffer2[i] = SPI3_ReadByte();   //读取接收到的数据 	
    for(i=0; i<8; i++) 
            BatVolt[i+8]=(int)((buffer3[i+i]<<8|buffer1[i+i+1])*0.00406f+0.5f);
    }
    else if(_RD10==1&&_RD11==0)
    {
        for(i=0; i<16; i++)                
            buffer3[i] = SPI3_ReadByte();   //读取接收到的数据 	  
        for(i=0; i<8; i++) 
            BatVolt[i+16]=(int)((buffer2[i+i]<<8|buffer1[i+i+1])*0.00406f+0.5f); 
    }
    else 
    {
        for(i=0; i<16; i++)                
            buffer4[i] = SPI3_ReadByte();   //读取接收到的数据 	
        for(i=0; i<8; i++) 
            BatVolt[i+24]=(int)((buffer4[i+i]<<8|buffer1[i+i+1])*0.00406f+0.5f);
    }
        _LATA2=1;
}
void AD7609_Read_Data()
{
    unsigned char i;
    _LATC2=0;
    for(i=0; i<18; i++)                   //读255个数据 
    { 
        buffer5[i] = SPI2_ReadByte();   //读取接收到的数据 	
    } 
    _LATC2=1;     
    AD7609_Read=1;
}
void AD5684_Write_Data()
{
    if(Write_AD5684_A_Flag)
    {
        _LATC1=0;
        SPI2_WriteByte(0x31);
        SPI2_WriteByte(AD5684_A_Data>>8);
        SPI2_WriteByte(AD5684_A_Data);
        _LATC1=1;
        Write_AD5684_A_Flag=0;
    }
    if(Write_AD5684_B_Flag)
    {
        _LATC1=0;
        SPI2_WriteByte(0x32);
        SPI2_WriteByte(AD5684_B_Data>>8);
        SPI2_WriteByte(AD5684_B_Data);
        _LATC1=1;
        Write_AD5684_B_Flag=0;
    }
    if(Write_AD5684_C_Flag)
    {
        _LATC1=0;
        SPI2_WriteByte(0x34);
        SPI2_WriteByte(AD5684_C_Data>>8);
        SPI2_WriteByte(AD5684_C_Data);
        _LATC1=1;
        Write_AD5684_C_Flag=0;
    }
    if(Write_AD5684_D_Flag)
    {
        _LATC1=0;
        SPI2_WriteByte(0x38);
        SPI2_WriteByte(AD5684_D_Data>>8);
        SPI2_WriteByte(AD5684_D_Data);
        _LATC1=1;
        Write_AD5684_D_Flag=0;
    }
}
//-------------------------------------------------------------------------------
//	电平变化中断函数  注意中断编号
//-------------------------------------------------------------------------------
void __attribute__((vector(26)))__attribute__((interrupt(IPL4)))_CNInterrupt(void)
{	
	char i;
    if(_RB15 == 1)
    {
        int_flag1=1;
    }
    else if(_RB15 == 0 && int_flag1==1)
	{ 
        int_flag1=0;
        AD7606_Read_Data();
	}
    if(_RB5 == 1)
    {
        int_flag2=1;
    }
    else if(_RB5 == 0 && int_flag2==1)
	{ 
        int_flag2=0;
        AD7609_Read_Data();
        AD5684_Write_Data();
    }
	IFS1bits.CNIF = 0;            //清除中断
}