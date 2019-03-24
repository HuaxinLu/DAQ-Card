#include "uart_dma.h"
#include "config.h"
#include "timers.h"
#include "DP.h"
#include "CNIT.h"
unsigned char display_control[8]={0X5A,0XA5,0X05,0X82,0X00,0X01,0X00,0X03};
unsigned char light_control[6]={0X5A,0XA5,0X03,0X80,0X01,0X15};
unsigned char Test_Buffer[4]={0x00,0x01,0x02,0x03};
unsigned char RX5_Buffer[16] = {0X5A,0XA5,0X05,0X82,0X00,0X01,0X00,0X03};             //DMA0缓冲区
unsigned char TX5_Buffer[50][8];
unsigned char DataHead[2]={0xAA,0xBB};
void DATA_init()
{
    unsigned char i;
    for(i=0;i<=47;i++)
    {
        TX5_Buffer[i][0]=0X5A;
        TX5_Buffer[i][1]=0XA5;
        TX5_Buffer[i][2]=0X05;
        TX5_Buffer[i][3]=0X82;
        TX5_Buffer[i][4]=0X00;
        TX5_Buffer[i][5]=i;
        TX5_Buffer[i][6]=0X00;
        TX5_Buffer[i][7]=0X01;
    }
}
void Uart5_SendDataBuffer(char *buffer, UINT32 size)
{
    while(size)
    {
        while(!UARTTransmitterIsReady(UART5))
            ;

        UARTSendDataByte(UART5, *buffer);

        buffer++;
        size--;
    }
    while(!UARTTransmissionHasCompleted(UART5));
}
void Uart2_SendDataBuffer(char *buffer, UINT32 size)
{
    while(size)
    {
        while(!UARTTransmitterIsReady(UART2))
            ;

        UARTSendDataByte(UART2, *buffer);

        buffer++;
        size--;
    }
    while(!UARTTransmissionHasCompleted(UART2));
}
// *****************************************************************************
// UINT32 GetDataBuffer(char *buffer, UINT32 max_size)
// *****************************************************************************
UINT32 GetDataBuffer(char *buffer, UINT32 max_size)
{
    UINT32 num_char;

    num_char = 0;

    while(num_char < max_size)
    {
        UINT8 character;

        while(!UARTReceivedDataIsAvailable(UART2))
            ;

        character = UARTGetDataByte(UART2);

        if(character == '\r')
            break;

        *buffer = character;

        buffer++;
        num_char++;
    }
    return num_char;
}
void UART2_init()
{
    DATA_init();
    UARTConfigure(UART2, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART2, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART2, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART2, GetPeripheralClock(), 250000);
    UARTEnable(UART2, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
}
void UART5_init()
{
    DATA_init();
    UARTConfigure(UART5, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UART5, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UART5, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UART5, GetPeripheralClock(), 250000);
    UARTEnable(UART5, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
}
void SendDataToScreen()
{
    int i;  
    int temp;
    //DMA发送
    for(i=0;i<32;i++)
    {
        TX5_Buffer[i][6]=0;
        TX5_Buffer[i][7]=BatVolt[i];
    }
    for(i=0;i<8;i++)
    {
        TX5_Buffer[i+32][6]=SensorAmp[i]>>8;
        TX5_Buffer[i+32][7]=SensorAmp[i];              
    }
 
    TX5_Buffer[40][6]=Frequency1>>8;
    TX5_Buffer[40][7]=Frequency1;     
    
    temp=(int)((AD5684_A_Data>>4)*1.221f+0.5f);
    TX5_Buffer[44][6]=temp>>8;
    TX5_Buffer[44][7]=temp;    
    temp=(int)((AD5684_B_Data>>4)*1.221f+0.5f);
    TX5_Buffer[45][6]=temp>>8;
    TX5_Buffer[45][7]=temp;    
    temp=(int)((AD5684_C_Data>>4)*1.221f+0.5f);
    TX5_Buffer[46][6]=temp>>8;
    TX5_Buffer[46][7]=temp;    
    temp=(int)((AD5684_D_Data>>4)*1.221f+0.5f);
    TX5_Buffer[47][6]=temp>>8;
    TX5_Buffer[47][7]=temp;        
    DCH1CONbits.CHEN = 1;               //启动DMA 发送
}
//-------------------------------------------------------------------------------
//	DMA初始化函数
//-------------------------------------------------------------------------------
void DMA_Init(void)                 //DMA 初始化
{
	DCH0ECONbits.CHSIRQ = 74;       //IRQ 编号 U2RXREG
	DCH0ECONbits.SIRQEN = 1;        //IRQ 中断
	
	DCH0SSA = KVA_TO_PA((void*)&U5RXREG);   //源 地址
	DCH0DSA = KVA_TO_PA((void*)&RX5_Buffer); //目标地址
	DCH0SSIZ = 1;                   //U2RXREG 1次传输1Byte
	DCH0DSIZ = 24;                  //Buffer 传输大小
	DCH0CSIZ = 1;                   //传输 1Byte U2RXREG to Buffer

	DCH0INTbits.CHBCIE = 1;         //数据传输完成中断 
	DCH0INTbits.CHERIE = 1;         //通道地址错误中断  

	IPC9bits.DMA0IP = 4; 
	IPC9bits.DMA0IS = 2; 
	IEC1bits.DMA0IE = 1; 

	DCH0CONbits.CHAEN = 1;          //通道自动使能位 
	DCH0CONbits.CHEN = 1;           //使能DMA0通道
	
	DCH1ECONbits.CHSIRQ = 75;       //中断编号 U2TXREG
	DCH1ECONbits.SIRQEN = 1;        //IRQ 中断
	                            
	DCH1SSA = KVA_TO_PA((void*)&TX5_Buffer); //源地址
	DCH1DSA = KVA_TO_PA((void*)&U5TXREG);   //目标地址                            
	DCH1SSIZ = 384;                  //Buffer 传输长度
	DCH1DSIZ = 1;                   //U2TXREG传输1Byte
	DCH1CSIZ = 1;                   //传输 1Byte Buffer to U2TXREG
	
	DCH1INTbits.CHBCIE = 1;         //数据传输完成中断 
	DCH1INTbits.CHERIE = 1;         //通道地址错误中断  

	IPC9bits.DMA1IP = 4; 
	IPC9bits.DMA1IS = 2; 
	IEC1bits.DMA1IE = 1; 

//	DCH1CONbits.CHAEN = 1;          //通道自动使能位 
//	DCH1CONbits.CHEN = 1;           //使能DMA1通道

	DMACONbits.ON = 1;              //使能DMA 模块
}
//-------------------------------------------------------------------------------
//	DMA0 中断函数   芯片手册第7章 
//-------------------------------------------------------------------------------
void __attribute__((vector(36)))__attribute__((interrupt(IPL4)))_DMA0Interrupt(void)
{
	unsigned char i;
	if(DCH0INTbits.CHBCIF == 1)             //传输标志
	{
		/*for(i=0;i<16;i++)                   //复制数据
		{
			DMATX5_Buffer[i] = DMARX5_Buffer[i];
		}	
		DCH0INTbits.CHBCIF = 0;     
		DCH1CONbits.CHEN = 1;               //启动DMA 发送*/
	}
	if(DCH0INTbits.CHERIF == 1)             //地址错误
	{
		DCH0INTbits.CHERIF = 0;     
	}
	IFS1bits.DMA0IF = 0;                    // 清除中断标志
}

//-------------------------------------------------------------------------------
//	DMA1 中断函数   芯片手册第7章 
//-------------------------------------------------------------------------------
void __attribute__((vector(37)))__attribute__((interrupt(IPL4)))_DMA1Interrupt(void)
{    
	if(DCH1INTbits.CHBCIF == 1)             //传输标志
	{
		DCH1INTbits.CHBCIF = 0;     
	}
	if(DCH1INTbits.CHERIF == 1)             //地址错误
	{
		DCH1INTbits.CHERIF = 0;     
	}
	IFS1bits.DMA1IF = 0;                    // 清除中断标志
}