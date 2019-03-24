#include <plib.h>                      //调用通用头文件
#include "can_bus.h"

unsigned char RX_SID_Msg_Flag = 0;     //接收标志位
unsigned char RX_EID_Msg_Flag = 0;

CanMsgBuffer *TxMessage;               //发送结构指针
CanMsgBuffer *RxMessage;               //接收结构指针
  
unsigned char TXbuffer[16];             //发送数据缓冲区
unsigned char RXbuffer[16];             //接收数据缓冲区
int RX_ID;
unsigned char FCE_Enable;//FCE使能信号
unsigned char FCE_Control;//FCE控制信号
unsigned char Can2MsgFiFo[3 * 2 * 16]; //FIFO 缓冲区
                                       //报文空间 ----> 16 Byte
                                       //报文数量 ----> 2个
                                       //缓冲区数 ----> 3个
		                               
		                            
cecudata1 CECUDATA1;
cecudata2 CECUDATA2;
vmsdata VMSDATA;
//-------------------------------------------------------------------------------
//	SID 接收  配置 ->仅接收标准标识符报文
//-------------------------------------------------------------------------------
void Write_SID_Filter(unsigned short SendSID)
{
	C2FLTCON0bits.FSEL0 = 1;  //存储 use FIFO 1
	C2FLTCON0bits.MSEL0 = 0;  //屏蔽 use Mask 0
	C2RXF0bits.SID  = SendSID&0x7FF; //过滤 0x100
	C2RXF0bits.EXID = 0;             //标准 SID
	C2RXM0bits.SID  = 0x3E8; //屏蔽 低3位,接收8个 ID报文
	C2RXM0bits.MIDE = 1;      //匹配过滤器 MSG
	C2FLTCON0bits.FLTEN0 = 1; //使能过滤器 0
}

//-------------------------------------------------------------------------------
//	EID 接收  配置 ->仅接收扩展标识符报文
//-------------------------------------------------------------------------------
void Write_EID_Filter(unsigned int SendEID)
{	
	C2FLTCON0bits.FSEL1 = 2;  //存储 use FIFO 2
	C2FLTCON0bits.MSEL1 = 1;  //屏蔽 use Mask 1
	C2RXF1bits.SID  = (SendEID>>18)&0x7FF;  //11 bit 
	C2RXF1bits.EID  = SendEID&0x3FFFF;      //18 bit
	C2RXF1bits.EXID = 1;                    //扩展 EID
	C2RXM1bits.SID  = (SendEID>>18)&0x7FF;  //11 bit   屏蔽 低3位 接收8个ID报文
	C2RXM1bits.EID  = SendEID&0x3FFF8;      //18 bit 
	C2RXM1bits.MIDE = 1;      //匹配过滤器 MSG
	C2FLTCON0bits.FLTEN1 = 1; //使能过滤器 1
}

//-------------------------------------------------------------------------------
//	CAN 比特率  配置      FBaud = 250Kb/S  比特位速率
//-------------------------------------------------------------------------------
void Write_CAN_Baud(void)      //FTQ = N * FBuad  N = 10
{                          	   //FBaud = 250K
                          	   //BRP = 40000000/(2*FTQ)-1

//	C2CFGbits.SEG2PHTS = 1;    //相位缓冲段2位可编程   1Mb
//	C2CFGbits.SEG2PH = 2;      //相位缓冲段2位 3TQ
//	C2CFGbits.SEG1PH = 2;      //相位缓冲段1位 3TQ
//	C2CFGbits.PRSEG = 2;       //传播时间段位  3TQ
//	C2CFGbits.SAM = 1;         //在采样点采样  3次
//	C2CFGbits.SJW = 0;         //同步跳转      1TQ
//	C2CFGbits.BRP = 1;         //预分频比  2*2/FSYS

/*	C2CFGbits.SEG2PHTS = 1;    //相位缓冲段2位可编程   500K
	C2CFGbits.SEG2PH = 2;      //相位缓冲段2位 3TQ
	C2CFGbits.SEG1PH = 2;      //相位缓冲段1位 3TQ
	C2CFGbits.PRSEG = 2;       //传播时间段位  3TQ
	C2CFGbits.SAM = 1;         //在采样点采样  3次
	C2CFGbits.SJW = 0;         //同步跳转      1TQ
	C2CFGbits.BRP = 3;         //预分频比  2*4/FSYS
*/	
/*	C2CFGbits.SEG2PHTS = 1;    //相位缓冲段2位可编程   250K
	C2CFGbits.SEG2PH = 2;      //相位缓冲段2位 3TQ
	C2CFGbits.SEG1PH = 2;      //相位缓冲段1位 3TQ
	C2CFGbits.PRSEG = 2;       //传播时间段位  3TQ
	C2CFGbits.SAM = 1;         //在采样点采样  3次
	C2CFGbits.SJW = 0;         //同步跳转      1TQ
	C2CFGbits.BRP = 7;         //预分频比  2*8/FSYS
*/
	C2CFGbits.SEG2PHTS = 1;    //相位缓冲段2位可编程   125K
	C2CFGbits.SEG2PH = 2;      //相位缓冲段2位 3TQ
	C2CFGbits.SEG1PH = 2;      //相位缓冲段1位 3TQ
	C2CFGbits.PRSEG = 2;       //传播时间段位  3TQ
	C2CFGbits.SAM = 1;         //在采样点采样  3次
	C2CFGbits.SJW = 0;         //同步跳转      1TQ
	C2CFGbits.BRP = 15;        //预分频比  2*16/FSYS

//	C2CFGbits.SEG2PHTS = 1;    //相位缓冲段2位可编程   100K
//	C2CFGbits.SEG2PH = 2;      //相位缓冲段2位 3TQ
//	C2CFGbits.SEG1PH = 2;      //相位缓冲段1位 3TQ
//	C2CFGbits.PRSEG = 2;       //传播时间段位  3TQ
//	C2CFGbits.SAM = 1;         //在采样点采样  3次
//	C2CFGbits.SJW = 0;         //同步跳转      1TQ
//	C2CFGbits.BRP = 19;        //预分频比  2*20/FSYS

//	C2CFGbits.SEG2PHTS = 1;    //相位缓冲段2位可编程   50K
//	C2CFGbits.SEG2PH = 2;      //相位缓冲段2位 3TQ
//	C2CFGbits.SEG1PH = 2;      //相位缓冲段1位 3TQ
//	C2CFGbits.PRSEG = 2;       //传播时间段位  3TQ
//	C2CFGbits.SAM = 1;         //在采样点采样  3次
//	C2CFGbits.SJW = 0;         //同步跳转      1TQ
//	C2CFGbits.BRP = 39;        //预分频比  2*40/FSYS	
}
	
//-------------------------------------------------------------------------------
//	CAN 初始化                FBaud = 250Kb/S
//-------------------------------------------------------------------------------
void CAN_Init(void)
{
	C2CONbits.ON = 1;          //使能CAN模块
	C2CONbits.REQOP = 0x04;         //请求配置模式
	while(C2CONbits.OPMOD != 0x04); //确认配置模式

	C2FIFOBA = KVA_TO_PA(Can2MsgFiFo); //虚拟地址转物理地址

	Write_CAN_Baud();              //比特率 250kb/S

	Write_SID_Filter(0x503);       //SID 初始化 接收(0x100 - 0x107)
	Write_EID_Filter(0x1FFFFF00);  //EID 初始化 接收(0x1FFFFF00 - 0x1FFFFF07)
	
	C2FIFOCON0bits.FSIZE = 1;      //报文深度  2
	C2FIFOCON0bits.TXEN  = 1;      //FIFO0->发送
	
	C2FIFOCON1bits.FSIZE = 1;      //报文深度  2
	C2FIFOCON1bits.TXEN  = 0;      //FIFO1->接收
	
	C2FIFOCON2bits.FSIZE = 1;      //报文深度  2
	C2FIFOCON2bits.TXEN  = 0;      //FIFO2->接收
		
	C2FIFOINT1bits.RXNEMPTYIE = 1; //FIFO 1 接收非空中断
	C2FIFOINT2bits.RXNEMPTYIE = 1; //FIFO 2 接收非空中断
	
	C2INTbits.RBIE = 1;            //接收中断允许
	C2INTbits.TBIE = 0;            //发送中断禁止
	
	IPC11bits.CAN2IP = 4;          //CAN2 中断
	IPC11bits.CAN2IS = 1;
	IEC1bits.CAN2IE = 1;
	
//	C2CONbits.REQOP = 0x2;         //请求环回模式   单个节点 测试,使能语句
//	while (C2CONbits.OPMOD!=0x2);  //确认环回模式

	C2CONbits.REQOP = 0x0;         //请求正常模式   多个节点 测试,使能语句
	while (C2CONbits.OPMOD!=0x0);  //确认正常模式 
}  

//-------------------------------------------------------------------------------
//	CAN 发送 SID 报文
//-------------------------------------------------------------------------------
void CAN_TX_SID_Message(unsigned short MsgSID)
{
	TxMessage = PA_TO_KVA1(C2FIFOUA0);  //获取报文地址
	
	TxMessage->CMSGSID.SID = MsgSID&0x7FFF;  //发送 ID
	TxMessage->CMSGEID.IDE = 0;         //标准报文
	TxMessage->CMSGEID.DLC = 0x08;      //数据长度 8 Byte
	
	TxMessage->CMSGDATA0.Byte0 = 0x01;  //CMSGDAT0
	TxMessage->CMSGDATA0.Byte1 = 0x02;
	TxMessage->CMSGDATA0.Byte2 = 0x04;
	TxMessage->CMSGDATA0.Byte3 = 0x08;
	
	TxMessage->CMSGDATA1.Byte4 = 0x0A;  //CMSGDAT1
	TxMessage->CMSGDATA1.Byte5 = 0x0C;
	TxMessage->CMSGDATA1.Byte6 = 0x0E;
	TxMessage->CMSGDATA1.Byte7 = TXbuffer[7]++;
	
	_LATE7 = 1;                         //LED指示
	
	C2FIFOCON0SET = 0x00002000;         //UINC = 1
	C2FIFOCON0SET = 0x00000008;         //请求发送
}
//-------------------------------------------------------------------------------
//	CAN 发送 SID 报文
//-------------------------------------------------------------------------------
void Send_CECU(unsigned short MsgSID)
{
	unsigned char FCECmdByte;
    if(FCE_Enable==1)
        VMSDATA.Control_command=0x08;
    else
        VMSDATA.Control_command=0x04;
    if(FCE_Control==1)
        VMSDATA.Control_command=VMSDATA.Control_command&0x01;
    else
        VMSDATA.Control_command=VMSDATA.Control_command&0x02;
    TxMessage = PA_TO_KVA1(C2FIFOUA0);  //获取报文地址
	
	TxMessage->CMSGSID.SID = MsgSID&0x7FFF;  //发送 ID
	TxMessage->CMSGEID.IDE = 0;         //标准报文
	TxMessage->CMSGEID.DLC = 0x08;      //数据长度 8 Byte
	
	TxMessage->CMSGDATA0.Byte0 = VMSDATA.Control_command;  //CMSGDAT0
	TxMessage->CMSGDATA0.Byte1 = VMSDATA.Set_power&0xFF;
	TxMessage->CMSGDATA0.Byte2 = VMSDATA.Set_power>>8;
	TxMessage->CMSGDATA0.Byte3 = 0;
	
	TxMessage->CMSGDATA1.Byte4 = 0;  //CMSGDAT1
	TxMessage->CMSGDATA1.Byte5 = 0;
	TxMessage->CMSGDATA1.Byte6 = 0;
	TxMessage->CMSGDATA1.Byte7 = TXbuffer[7]++;
	
	_LATE7 = 1;                         //LED指示
	
	C2FIFOCON0SET = 0x00002000;         //UINC = 1
	C2FIFOCON0SET = 0x00000008;         //请求发送
}
//-------------------------------------------------------------------------------
//	CAN 发送 SID 报文
//-------------------------------------------------------------------------------
void CAN_TX_EID_Message(unsigned int SendEID)
{
	TxMessage = PA_TO_KVA1(C2FIFOUA0);  //获取报文地址
	
	TxMessage->CMSGSID.SID = (SendEID>>18)&0x7FF;  //11 bit 
	TxMessage->CMSGEID.EID = SendEID&0x3FFFF;      //18 bit
	
	TxMessage->CMSGEID.IDE = 1;         //标准报文
	TxMessage->CMSGEID.DLC = 0x08;      //数据长度 8 Byte
	
	TxMessage->CMSGDATA0.Byte0 = 0x10;  //CMSGDAT0
	TxMessage->CMSGDATA0.Byte1 = 0x20;
	TxMessage->CMSGDATA0.Byte2 = 0x40;
	TxMessage->CMSGDATA0.Byte3 = 0x80;
	
	TxMessage->CMSGDATA1.Byte4 = 0xA0;  //CMSGDAT1
	TxMessage->CMSGDATA1.Byte5 = 0xC0;
	TxMessage->CMSGDATA1.Byte6 = 0xE0;
	TxMessage->CMSGDATA1.Byte7 = TXbuffer[7]++;
	
	_LATE6 = 1;                         //LED指示
	
	C2FIFOCON0SET = 0x00002000;         //UINC = 1
	C2FIFOCON0SET = 0x00000008;         //请求发送
}

//-------------------------------------------------------------------------------
//	CAN 接收报文处理
//-------------------------------------------------------------------------------
void CAN2RxMsgProcess(void)
{
	if(RX_SID_Msg_Flag == 1)               //FIFO0 收到报文
	{
		RxMessage = (CanMsgBuffer *)(PA_TO_KVA1(C2FIFOUA1));
		
		RX_ID = RxMessage->CMSGSID.SID;
        RXbuffer[3] = RxMessage->CMSGDATA0.Byte0;  //CMSGDAT0
		RXbuffer[4] = RxMessage->CMSGDATA0.Byte1;
		RXbuffer[5] = RxMessage->CMSGDATA0.Byte2;
		RXbuffer[6] = RxMessage->CMSGDATA0.Byte3; 
		
		RXbuffer[7] = RxMessage->CMSGDATA1.Byte4;  //CMSGDAT1
		RXbuffer[8] = RxMessage->CMSGDATA1.Byte5; 
		RXbuffer[9] = RxMessage->CMSGDATA1.Byte6;
		RXbuffer[10] = RxMessage->CMSGDATA1.Byte7;
        if(RX_ID==0x114)
        {
            CECUDATA1.Water_inlet_temperature=(RXbuffer[3])|(RXbuffer[4]<<8);
            CECUDATA1.Water_outlet_temperature=(RXbuffer[5])|(RXbuffer[6]<<8);        
            CECUDATA1.Allowable_load_power=(RXbuffer[7])|(RXbuffer[8]<<8);
            CECUDATA1.Water_inlet_pressure=(RXbuffer[9])|(RXbuffer[10]<<8);
            RXbuffer[0]=0xAA;
            RXbuffer[1]=0xAA;
            DCH0INTbits.CHBCIF = 0;     
            DCH1CONbits.CHEN = 1;               //启动DMA 发送*/
        }
        else if(RX_ID==0x503)
        {
            CECUDATA2.System_operation_mode=RXbuffer[3];
            CECUDATA2.System_operation_indicator=RXbuffer[41];        
            CECUDATA2.External_circulating_water_pressure=RXbuffer[5];
            CECUDATA2.Hydrogen_bottle_pressure=(RXbuffer[6])|(RXbuffer[7]<<8);
            CECUDATA2.System_state=RXbuffer[8];
            RXbuffer[0]=0xBB;
            RXbuffer[1]=0xBB;
            DCH0INTbits.CHBCIF = 0;     
            DCH1CONbits.CHEN = 1;               //启动DMA 发送*/
        }        
        
		RX_SID_Msg_Flag = 0;
		
		C2FIFOCON1SET = 0x00002000;        //UINC = 1  已读
		C2FIFOINT1bits.RXNEMPTYIE = 1;     //接收缓冲区中断
	}
	
	if(RX_EID_Msg_Flag == 1)               //FIFO2 收到报文
	{
		RxMessage = (CanMsgBuffer *)(PA_TO_KVA1(C2FIFOUA2));
		
		/*RXbuffer[0] = RxMessage->CMSGDATA0.Byte0;  //CMSGDAT0
		RXbuffer[1] = RxMessage->CMSGDATA0.Byte1;
		RXbuffer[2] = RxMessage->CMSGDATA0.Byte2;
		RXbuffer[3] = RxMessage->CMSGDATA0.Byte3; 
		
		RXbuffer[4] = RxMessage->CMSGDATA1.Byte4;  //CMSGDAT1
		RXbuffer[5] = RxMessage->CMSGDATA1.Byte5; 
		RXbuffer[6] = RxMessage->CMSGDATA1.Byte6;
		RXbuffer[7] = RxMessage->CMSGDATA1.Byte7;*/
		
		RX_EID_Msg_Flag = 0;

		C2FIFOCON2SET = 0x00002000;        //UINC = 1  已读
		C2FIFOINT2bits.RXNEMPTYIE = 1;     //接收缓冲区中断
	}		
}	
//-------------------------------------------------------------------------------
//	CAN2 中断函数  注意中断编号
//-------------------------------------------------------------------------------
void __attribute__((vector(47)))__attribute__((interrupt(IPL4)))_CAN2Interrupt(void)
{
	if(C2INTbits.RBIF == 1)                   //接收中断 
	{
		if(C2FIFOINT1bits.RXNEMPTYIF == 1)    //FIFO 1
		{
			_LATE1 = 1;                       //LED指示
			RX_SID_Msg_Flag = 1;              //收到报文
			C2FIFOINT1bits.RXNEMPTYIE = 0;    //中断禁止
		}
		if(C2FIFOINT2bits.RXNEMPTYIF == 1)    //FIFO 2
		{
			_LATE0 = 1;                       //LED指示
			RX_EID_Msg_Flag = 1;              //收到报文
			C2FIFOINT2bits.RXNEMPTYIE = 0;    //中断禁止
		}
		C2INTbits.RBIF = 0;
	}
	
	if(C2INTbits.TBIF == 1)                   //发送中断
	{
		_LATE0 = !_LATE0;
		C2INTbits.TBIF = 0;
	}	
	IFS1bits.CAN2IF = 0;
}

	

//-------------------------------------------------------------------------------

