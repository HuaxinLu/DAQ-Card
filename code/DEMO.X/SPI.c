#include "SPI.h"
#include "uart_dma.h"

void SPI2_Init(void) 
{ 
	SPI2CONbits.CKE = 0;         //�����ط�����
	SPI2CONbits.CKP = 0;         //����ʱ�͵�ƽ	
	SPI2CONbits.MODE32 = 0;      //8λ����ģʽ
	SPI2CONbits.MODE16 = 0;      //
	SPI2BRG = 0;                 //SCK = 10MHz
	SPI2CONbits.MSTEN = 1;       //��ģʽ
	SPI2CONbits.ON    = 1;       //SIP ON
}
void SPI3_Init(void) 
{ 
	SPI3CONbits.CKE = 1;         //�����ط�����
	SPI3CONbits.CKP = 0;         //����ʱ�͵�ƽ
	SPI3CONbits.MODE32 = 0;      //8λ����ģʽ
	SPI3CONbits.MODE16 = 0;      //
	SPI3BRG = 0;                 //SCK = 10MHz
	SPI3CONbits.MSTEN = 1;       //��ģʽ
	SPI3CONbits.ON    = 1;       //SIP ON
}

unsigned char SPI2_WriteByte(unsigned char val) 
{ 
	SPI2BUF = val;                //�������� 
	while(!IFS1bits.SPI2TXIF);    //�ȴ�������� 
	IFS1bits.SPI2TXIF = 0;        //�����־λ 
	return SPI2BUF;               //��ȡ���ռĴ��� 
} 
unsigned char SPI2_ReadByte(void) 
{ 
	SPI2BUF = 0xFF;               //��������,�������ݽ��� 
	while(!IFS1bits.SPI2TXIF);    //�ȴ�������� 
	IFS1bits.SPI2TXIF = 0;        //�����־λ 
	while(!IFS1bits.SPI2RXIF);    //�ȴ�������� 
	IFS1bits.SPI2RXIF = 0;        //�����־λ 
	
	return SPI2BUF;               //��ȡ���յ����� 
} 
unsigned char SPI3_ReadByte(void) 
{ 
	SPI3BUF = 0xFF;               //��������,�������ݽ��� 
	while(!IFS0bits.SPI3TXIF);    //�ȴ�������� 
	IFS0bits.SPI3TXIF = 0;        //�����־λ
	while(!IFS0bits.SPI3RXIF);    // �ȴ�������� 
	IFS0bits.SPI3RXIF = 0;        //�����־λ 
	
	return SPI3BUF;               //��ȡ���յ����� 
} 


