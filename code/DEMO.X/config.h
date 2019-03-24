#ifndef __CONFIG_H
#define __CONFIG_H 
#include <plib.h>                    //调用通用头文件

#define	GetPeripheralClock()		(GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define	GetInstructionClock()		(GetSystemClock())
#define	GetSystemClock() 			(64000000ul)
#endif