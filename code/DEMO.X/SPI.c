#include "SPI.h"
#include "uart_dma.h"

void SPI2_Init(void) 
{ 
	SPI2CONbits.CKE = 0;         //上升沿发数据
	SPI2CONbits.CKP = 0;         //空闲时低电平	
	SPI2CONbits.MODE32 = 0;      //8位数据模式
	SPI2CONbits.MODE16 = 0;      //
	SPI2BRG = 0;                 //SCK = 10MHz
	SPI2CONbits.MSTEN = 1;       //主模式
	SPI2CONbits.ON    = 1;       //SIP ON
}
void SPI3_Init(void) 
{ 
	SPI3CONbits.CKE = 1;         //上升沿发数据
	SPI3CONbits.CKP = 0;         //空闲时低电平
	SPI3CONbits.MODE32 = 0;      //8位数据模式
	SPI3CONbits.MODE16 = 0;      //
	SPI3BRG = 0;                 //SCK = 10MHz
	SPI3CONbits.MSTEN = 1;       //主模式
	SPI3CONbits.ON    = 1;       //SIP ON
}

unsigned char SPI2_WriteByte(unsigned char val) 
{ 
	SPI2BUF = val;                //发送数据 
	while(!IFS1bits.SPI2TXIF);    //等待发送完成 
	IFS1bits.SPI2TXIF = 0;        //清除标志位 
	return SPI2BUF;               //读取接收寄存器 
} 
unsigned char SPI2_ReadByte(void) 
{ 
	SPI2BUF = 0xFF;               //发送数据,启动数据接收 
	while(!IFS1bits.SPI2TXIF);    //等待接收完成 
	IFS1bits.SPI2TXIF = 0;        //清除标志位 
	while(!IFS1bits.SPI2RXIF);    //等待接收完成 
	IFS1bits.SPI2RXIF = 0;        //清除标志位 
	
	return SPI2BUF;               //读取接收到数据 
} 
unsigned char SPI3_ReadByte(void) 
{ 
	SPI3BUF = 0xFF;               //发送数据,启动数据接收 
	while(!IFS0bits.SPI3TXIF);    //等待接收完成 
	IFS0bits.SPI3TXIF = 0;        //清除标志位
	while(!IFS0bits.SPI3RXIF);    // 等待接收完成 
	IFS0bits.SPI3RXIF = 0;        //清除标志位 
	
	return SPI3BUF;               //读取接收到数据 
} 


