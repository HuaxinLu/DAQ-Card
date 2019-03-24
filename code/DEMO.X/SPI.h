#ifndef __SPI_H
#define __SPI_H 
#include <plib.h>                    //调用通用头文件

void SPI2_Init(void);
void SPI3_Init(void);
unsigned char SPI2_WriteByte(unsigned char val);
unsigned char SPI2_ReadByte(void);
unsigned char SPI3_ReadByte(void) ;
#endif