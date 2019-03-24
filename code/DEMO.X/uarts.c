#include "uarts.h"
#include "config.h"
#include "timers.h"
unsigned char display_control[8]={0X5A,0XA5,0X05,0X82,0X00,0X01,0X00,0X03};
unsigned char light_control[6]={0X5A,0XA5,0X03,0X80,0X01,0X25};
unsigned char buffer1[16]={0};
unsigned char buffer2[16]={0};
unsigned char buffer3[16]={0};
unsigned char buffer4[16]={0};
unsigned char buffer5[18]={0};
int AD7609Data[8]={0};
int BatVolt[32]={0};
int SensorVolt[8]={0};
unsigned long CaptureTime1,CaptureTime2;
unsigned int Frequency1;
unsigned char Test_Buffer[4]={0x00,0x01,0x02,0x03};
unsigned char RX5_Buffer[16] = {0X5A,0XA5,0X05,0X82,0X00,0X01,0X00,0X03};             //DMA0������
unsigned char TX5_Buffer[42][8];
unsigned char DataHead[2]={0xAA,0xBB};
void DATA_init()
{
    unsigned char i;
    for(i=0;i<=42;i++)
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
    //DMA����
    /*
    for(i=0; i<8; i++) 
    {
            TX5_Buffer[i][6]=buffer1[i+i];
            TX5_Buffer[i][7]=buffer1[i+i+1];
        }
        for(i=0; i<8; i++) 
    {
        TX5_Buffer[i+8][6]=buffer2[i+i];
        TX5_Buffer[i+8][7]=buffer2[i+i+1];
    }
    for(i=0; i<8; i++) 
    {
        TX5_Buffer[i+16][6]=buffer3[i+i];
        TX5_Buffer[i+16][7]=buffer3[i+i+1];
    }        
    for(i=0; i<8; i++) 
    {
        TX5_Buffer[i+24][6]=buffer4[i+i];
        TX5_Buffer[i+24][7]=buffer4[i+i+1];
    }
    for(i=0; i<8; i++) 
    {
        TX5_Buffer[i+32][6]=buffer5[i+i];
        TX5_Buffer[i+32][7]=buffer5[i+i+1];
    }
    if(CaptureTime2!=0&&cap1_error==0)
        Frequency1=4000000/CaptureTime2;
    else 
        Frequency1=0;
        TX5_Buffer[40][6]=Frequency1>>8;
        TX5_Buffer[40][7]=Frequency1;
        TX5_Buffer[41][6]=time_flag1>>8;
        TX5_Buffer[41][7]=time_flag1;        
        
        DCH1CONbits.CHEN = 1;               //����DMA ����*/
        
    //��DMA����
    display_control[5]=0;
    for(i=0;i<32;i++)
    {
        BatVolt[i]=(buffer1[i+i]<<8|buffer1[i+i+1])*0.00406f;
        display_control[6]=0;
        display_control[7]=BatVolt[i];
        Uart5_SendDataBuffer(display_control,8);
        display_control[5]++;
    }
    /*
    for(i=0; i<8; i++) 
    {    
        display_control[6]=buffer1[i+i];
        display_control[7]=buffer1[i+i+1];
        Uart5_SendDataBuffer(display_control,8);
        display_control[5]++;
    }
    for(i=0; i<8; i++) 
    {
        display_control[6]=buffer2[i+i];
        display_control[7]=buffer2[i+i+1];
        Uart5_SendDataBuffer(display_control,8);
        display_control[5]++;
    }
    for(i=0; i<8; i++) 
    {    
        display_control[6]=buffer3[i+i];
        display_control[7]=buffer3[i+i+1];
        Uart5_SendDataBuffer(display_control,8);
        display_control[5]++;
    }
    for(i=0; i<8; i++) 
    {
        display_control[6]=buffer4[i+i];
        display_control[7]=buffer4[i+i+1];
        Uart5_SendDataBuffer(display_control,8);
        display_control[5]++;
    }*/
    /*AD7609Data[0]=buffer5[0];
    AD7609Data[1]=buffer5[1];
    AD7609Data[2]=buffer5[2]<<2|buffer5[3]>>6;
    AD7609Data[3]=buffer5[3]<<2|buffer5[4]>>6;
    AD7609Data[4]=buffer5[4]<<4|buffer5[5]>>4;
    AD7609Data[5]=buffer5[5]<<4|buffer5[6]>>4;    
    AD7609Data[6]=buffer5[6]<<6|buffer5[7]>>2;
    AD7609Data[7]=buffer5[7]<<6|buffer5[8]>>2;    
    AD7609Data[8]=buffer5[9];
    AD7609Data[9]=buffer5[10];
    AD7609Data[10]=buffer5[11]<<2|buffer5[12]>>6;
    AD7609Data[11]=buffer5[12]<<2|buffer5[13]>>6;
    AD7609Data[12]=buffer5[13]<<4|buffer5[14]>>4;
    AD7609Data[13]=buffer5[14]<<4|buffer5[15]>>4;    
    AD7609Data[14]=buffer5[15]<<6|buffer5[16]>>2;
    AD7609Data[15]=buffer5[16]<<6|buffer5[17]>>2;        */
    
    AD7609Data[0]=(buffer5[0]<<10|buffer5[1]<<2|buffer5[2]>>6)-10;
    AD7609Data[1]=((buffer5[2]&0x3F)<<12|buffer5[3]<<4|buffer5[4]>>4)-1;  
    AD7609Data[2]=((buffer5[4]&0x0F)<<14|buffer5[5]<<6|buffer5[6]>>2)+3;  
    AD7609Data[3]=((buffer5[6]&0x03)<<16|buffer5[7]<<8|buffer5[8])-9;
    AD7609Data[4]=(buffer5[9]<<10|buffer5[10]<<2|buffer5[11]>>6)-10;
    AD7609Data[5]=(buffer5[11]&0x3F)<<12|buffer5[12]<<4|buffer5[13]>>4;  
    AD7609Data[6]=((buffer5[13]&0x0F)<<14|buffer5[14]<<6|buffer5[15]>>2)-9;  
    AD7609Data[7]=((buffer5[15]&0x03)<<16|buffer5[16]<<8|buffer5[17])-9;
    for(i=0; i<8; i++) 
    {
        SensorVolt[i]=(AD7609Data[i])*0.0763f;
        if(SensorVolt[2]>10000)
            SensorVolt[2]=0;
        display_control[6]=SensorVolt[i]>>8;
        display_control[7]=SensorVolt[i];
        //display_control[6]=AD7609Data[i]>>8;
        //display_control[7]=AD7609Data[i];
        Uart5_SendDataBuffer(display_control,8);
        display_control[5]++;
    }
    display_control[5]=41;
    if(CaptureTime2!=0)
        Frequency1=4000000/CaptureTime2;
    else 
        Frequency1=0;
        
    display_control[6]=Frequency1>>8;
    display_control[7]=Frequency1;
    Uart5_SendDataBuffer(display_control,8);
}
//-------------------------------------------------------------------------------
//	DMA��ʼ������
//-------------------------------------------------------------------------------
void DMA_Init(void)                 //DMA ��ʼ��
{
	DCH0ECONbits.CHSIRQ = 41;       //IRQ ��� U2RXREG
	DCH0ECONbits.SIRQEN = 1;        //IRQ �ж�
	
	DCH0SSA = KVA_TO_PA((void*)&U5RXREG);   //Դ ��ַ
	DCH0DSA = KVA_TO_PA((void*)&RX5_Buffer); //Ŀ���ַ
	DCH0SSIZ = 1;                   //U2RXREG 1�δ���1Byte
	DCH0DSIZ = 24;                  //Buffer �����С
	DCH0CSIZ = 1;                   //���� 1Byte U2RXREG to Buffer

	DCH0INTbits.CHBCIE = 1;         //���ݴ�������ж� 
	DCH0INTbits.CHERIE = 1;         //ͨ����ַ�����ж�  

	IPC9bits.DMA0IP = 4; 
	IPC9bits.DMA0IS = 2; 
	IEC1bits.DMA0IE = 1; 

	DCH0CONbits.CHAEN = 1;          //ͨ���Զ�ʹ��λ 
	DCH0CONbits.CHEN = 1;           //ʹ��DMA0ͨ��
	
	DCH1ECONbits.CHSIRQ = 42;       //�жϱ�� U2TXREG
	DCH1ECONbits.SIRQEN = 1;        //IRQ �ж�
	                            
	DCH1SSA = KVA_TO_PA((void*)&TX5_Buffer); //Դ��ַ
	DCH1DSA = KVA_TO_PA((void*)&U5TXREG);   //Ŀ���ַ                            
	DCH1SSIZ = 336;                  //Buffer ���䳤��
	DCH1DSIZ = 1;                   //U2TXREG����1Byte
	DCH1CSIZ = 1;                   //���� 1Byte Buffer to U2TXREG
	
	DCH1INTbits.CHBCIE = 1;         //���ݴ�������ж� 
	DCH1INTbits.CHERIE = 1;         //ͨ����ַ�����ж�  

	IPC9bits.DMA1IP = 4; 
	IPC9bits.DMA1IS = 2; 
	IEC1bits.DMA1IE = 1; 

//	DCH1CONbits.CHAEN = 1;          //ͨ���Զ�ʹ��λ 
//	DCH1CONbits.CHEN = 1;           //ʹ��DMA1ͨ��

	DMACONbits.ON = 1;              //ʹ��DMA ģ��
}
//-------------------------------------------------------------------------------
//	DMA0 �жϺ���   оƬ�ֲ��7�� 
//-------------------------------------------------------------------------------
void __attribute__((vector(36)))__attribute__((interrupt(IPL4)))_DMA0Interrupt(void)
{
	unsigned char i;
	if(DCH0INTbits.CHBCIF == 1)             //�����־
	{
		/*for(i=0;i<16;i++)                   //��������
		{
			DMATX5_Buffer[i] = DMARX5_Buffer[i];
		}	
		DCH0INTbits.CHBCIF = 0;     
		DCH1CONbits.CHEN = 1;               //����DMA ����*/
	}
	if(DCH0INTbits.CHERIF == 1)             //��ַ����
	{
		DCH0INTbits.CHERIF = 0;     
	}
	IFS1bits.DMA0IF = 0;                    // ����жϱ�־
}

//-------------------------------------------------------------------------------
//	DMA1 �жϺ���   оƬ�ֲ��7�� 
//-------------------------------------------------------------------------------
void __attribute__((vector(37)))__attribute__((interrupt(IPL4)))_DMA1Interrupt(void)
{    
	if(DCH1INTbits.CHBCIF == 1)             //�����־
	{
		DCH1INTbits.CHBCIF = 0;     
	}
	if(DCH1INTbits.CHERIF == 1)             //��ַ����
	{
		DCH1INTbits.CHERIF = 0;     
	}
	IFS1bits.DMA1IF = 0;                    // ����жϱ�־
}