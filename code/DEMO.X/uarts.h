#ifndef __UARTS_H
#define __UARTS_H 
#include <plib.h>                    //调用通用头文件

#include <GenericTypeDefs.h>
void UART2_init();
void UART5_init();
void SendDataBuffer(char *buffer, UINT32 size);
UINT32 GetMenuChoice(void);
UINT32 GetDataBuffer(char *buffer, UINT32 max_size);
void DMA_Init(void);    
void SendDataToScreen();
void Uart2_SendDataBuffer();
void Uart5_SendDataBuffer();
extern unsigned char send_screen;
extern unsigned char display_control[8];
extern unsigned char light_control[6];
extern unsigned char RX2_Buffer[16];
extern unsigned char TX2_Buffer[42][8];
extern unsigned long CaptureTime1,CaptureTime2;
extern unsigned int Frequency1;
extern unsigned char buffer1[16];
extern unsigned char buffer2[16];
extern unsigned char buffer3[16];
extern unsigned char buffer4[16];
extern unsigned char buffer5[18];
extern unsigned char Test_Buffer[4];
extern unsigned char DataHead[2];
extern int BatVolt[32];
#endif