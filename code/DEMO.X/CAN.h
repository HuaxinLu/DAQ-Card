#ifndef __CAN_H
#define __CAN_H

typedef struct 
{
	unsigned SID:11;
	unsigned FILHIT:5;
	unsigned CMSGTS:16;
}cmsgsid;

typedef struct 
{
	unsigned DLC:4;
	unsigned RB0:1;
	unsigned :3;
	unsigned RB1:1;
	unsigned RTR:1;
	unsigned EID:18;
	unsigned IDE:1;
	unsigned SRR:1;
	unsigned :2;
}cmsgeid;

typedef struct
{
	unsigned Byte0:8;
	unsigned Byte1:8;
	unsigned Byte2:8;
	unsigned Byte3:8;
}cmsgdata0;

typedef struct
{
	unsigned Byte4:8;
	unsigned Byte5:8;
	unsigned Byte6:8;
	unsigned Byte7:8;
}cmsgdata1;

typedef union uCanMsgBuffer 
{
	struct
	{
		cmsgsid CMSGSID;
		cmsgeid CMSGEID;
		cmsgdata0 CMSGDATA0;
		cmsgdata1 CMSGDATA1;
	};
	int messageWord[4];
	
}CanMsgBuffer;

typedef struct
{
	unsigned Water_inlet_temperature:16;
	unsigned Water_outlet_temperature:16;
	unsigned Allowable_load_power:16;
	unsigned Water_inlet_pressure:16;
}cecudata1;

typedef struct
{
	unsigned System_operation_mode:8;
	unsigned System_operation_indicator:8;
	unsigned External_circulating_water_pressure:8;
	unsigned Hydrogen_bottle_pressure:16;
    unsigned System_state:8;
}cecudata2;

typedef struct
{
	unsigned Control_command:8;
	unsigned Set_power:16;
}vmsdata;

extern cecudata1 CECUDATA1;
extern cecudata2 CECUDATA2;
extern vmsdata VMSDATA;
extern unsigned char RX_SID_Msg_Flag;         //接收标志位
extern unsigned char RX_EID_Msg_Flag;

extern unsigned char TXbuffer[16];             //发送数据缓冲区
extern unsigned char RXbuffer[16];             //接收数据缓冲区
extern long int RX_ID;

extern unsigned char FCE_Enable;//FCE使能信号
extern unsigned char FCE_Control;//FCE控制信号

extern void CAN_Init(void);

extern void Write_CAN_Baud(void);
extern void CAN2RxMsgProcess(void);

extern void Write_SID_Filter(unsigned short SendSID);
extern void Write_EID_Filter(unsigned int SendEID);

extern void CAN_TX_EID_Message(unsigned int SendEID);
extern void CAN_TX_SID_Message(unsigned short MsgSID);
	
void CAN_TX_BAT_VOLT_1();
void CAN_TX_BAT_VOLT_2();
void CAN_TX_BAT_VOLT_3();	
void CAN_TX_BAT_VOLT_4();
void CAN_TX_SENSOR_VOLT_1();
void CAN_TX_SENSOR_VOLT_2();
void CAN_TX_CON_DATA();
#endif


