#include <plib.h>                      //����ͨ��ͷ�ļ�
#include "can_bus.h"

unsigned char RX_SID_Msg_Flag = 0;     //���ձ�־λ
unsigned char RX_EID_Msg_Flag = 0;

CanMsgBuffer *TxMessage;               //���ͽṹָ��
CanMsgBuffer *RxMessage;               //���սṹָ��
  
unsigned char TXbuffer[16];             //�������ݻ�����
unsigned char RXbuffer[16];             //�������ݻ�����
int RX_ID;
unsigned char FCE_Enable;//FCEʹ���ź�
unsigned char FCE_Control;//FCE�����ź�
unsigned char Can2MsgFiFo[3 * 2 * 16]; //FIFO ������
                                       //���Ŀռ� ----> 16 Byte
                                       //�������� ----> 2��
                                       //�������� ----> 3��
		                               
		                            
cecudata1 CECUDATA1;
cecudata2 CECUDATA2;
vmsdata VMSDATA;
//-------------------------------------------------------------------------------
//	SID ����  ���� ->�����ձ�׼��ʶ������
//-------------------------------------------------------------------------------
void Write_SID_Filter(unsigned short SendSID)
{
	C2FLTCON0bits.FSEL0 = 1;  //�洢 use FIFO 1
	C2FLTCON0bits.MSEL0 = 0;  //���� use Mask 0
	C2RXF0bits.SID  = SendSID&0x7FF; //���� 0x100
	C2RXF0bits.EXID = 0;             //��׼ SID
	C2RXM0bits.SID  = 0x3E8; //���� ��3λ,����8�� ID����
	C2RXM0bits.MIDE = 1;      //ƥ������� MSG
	C2FLTCON0bits.FLTEN0 = 1; //ʹ�ܹ����� 0
}

//-------------------------------------------------------------------------------
//	EID ����  ���� ->��������չ��ʶ������
//-------------------------------------------------------------------------------
void Write_EID_Filter(unsigned int SendEID)
{	
	C2FLTCON0bits.FSEL1 = 2;  //�洢 use FIFO 2
	C2FLTCON0bits.MSEL1 = 1;  //���� use Mask 1
	C2RXF1bits.SID  = (SendEID>>18)&0x7FF;  //11 bit 
	C2RXF1bits.EID  = SendEID&0x3FFFF;      //18 bit
	C2RXF1bits.EXID = 1;                    //��չ EID
	C2RXM1bits.SID  = (SendEID>>18)&0x7FF;  //11 bit   ���� ��3λ ����8��ID����
	C2RXM1bits.EID  = SendEID&0x3FFF8;      //18 bit 
	C2RXM1bits.MIDE = 1;      //ƥ������� MSG
	C2FLTCON0bits.FLTEN1 = 1; //ʹ�ܹ����� 1
}

//-------------------------------------------------------------------------------
//	CAN ������  ����      FBaud = 250Kb/S  ����λ����
//-------------------------------------------------------------------------------
void Write_CAN_Baud(void)      //FTQ = N * FBuad  N = 10
{                          	   //FBaud = 250K
                          	   //BRP = 40000000/(2*FTQ)-1

//	C2CFGbits.SEG2PHTS = 1;    //��λ�����2λ�ɱ��   1Mb
//	C2CFGbits.SEG2PH = 2;      //��λ�����2λ 3TQ
//	C2CFGbits.SEG1PH = 2;      //��λ�����1λ 3TQ
//	C2CFGbits.PRSEG = 2;       //����ʱ���λ  3TQ
//	C2CFGbits.SAM = 1;         //�ڲ��������  3��
//	C2CFGbits.SJW = 0;         //ͬ����ת      1TQ
//	C2CFGbits.BRP = 1;         //Ԥ��Ƶ��  2*2/FSYS

/*	C2CFGbits.SEG2PHTS = 1;    //��λ�����2λ�ɱ��   500K
	C2CFGbits.SEG2PH = 2;      //��λ�����2λ 3TQ
	C2CFGbits.SEG1PH = 2;      //��λ�����1λ 3TQ
	C2CFGbits.PRSEG = 2;       //����ʱ���λ  3TQ
	C2CFGbits.SAM = 1;         //�ڲ��������  3��
	C2CFGbits.SJW = 0;         //ͬ����ת      1TQ
	C2CFGbits.BRP = 3;         //Ԥ��Ƶ��  2*4/FSYS
*/	
/*	C2CFGbits.SEG2PHTS = 1;    //��λ�����2λ�ɱ��   250K
	C2CFGbits.SEG2PH = 2;      //��λ�����2λ 3TQ
	C2CFGbits.SEG1PH = 2;      //��λ�����1λ 3TQ
	C2CFGbits.PRSEG = 2;       //����ʱ���λ  3TQ
	C2CFGbits.SAM = 1;         //�ڲ��������  3��
	C2CFGbits.SJW = 0;         //ͬ����ת      1TQ
	C2CFGbits.BRP = 7;         //Ԥ��Ƶ��  2*8/FSYS
*/
	C2CFGbits.SEG2PHTS = 1;    //��λ�����2λ�ɱ��   125K
	C2CFGbits.SEG2PH = 2;      //��λ�����2λ 3TQ
	C2CFGbits.SEG1PH = 2;      //��λ�����1λ 3TQ
	C2CFGbits.PRSEG = 2;       //����ʱ���λ  3TQ
	C2CFGbits.SAM = 1;         //�ڲ��������  3��
	C2CFGbits.SJW = 0;         //ͬ����ת      1TQ
	C2CFGbits.BRP = 15;        //Ԥ��Ƶ��  2*16/FSYS

//	C2CFGbits.SEG2PHTS = 1;    //��λ�����2λ�ɱ��   100K
//	C2CFGbits.SEG2PH = 2;      //��λ�����2λ 3TQ
//	C2CFGbits.SEG1PH = 2;      //��λ�����1λ 3TQ
//	C2CFGbits.PRSEG = 2;       //����ʱ���λ  3TQ
//	C2CFGbits.SAM = 1;         //�ڲ��������  3��
//	C2CFGbits.SJW = 0;         //ͬ����ת      1TQ
//	C2CFGbits.BRP = 19;        //Ԥ��Ƶ��  2*20/FSYS

//	C2CFGbits.SEG2PHTS = 1;    //��λ�����2λ�ɱ��   50K
//	C2CFGbits.SEG2PH = 2;      //��λ�����2λ 3TQ
//	C2CFGbits.SEG1PH = 2;      //��λ�����1λ 3TQ
//	C2CFGbits.PRSEG = 2;       //����ʱ���λ  3TQ
//	C2CFGbits.SAM = 1;         //�ڲ��������  3��
//	C2CFGbits.SJW = 0;         //ͬ����ת      1TQ
//	C2CFGbits.BRP = 39;        //Ԥ��Ƶ��  2*40/FSYS	
}
	
//-------------------------------------------------------------------------------
//	CAN ��ʼ��                FBaud = 250Kb/S
//-------------------------------------------------------------------------------
void CAN_Init(void)
{
	C2CONbits.ON = 1;          //ʹ��CANģ��
	C2CONbits.REQOP = 0x04;         //��������ģʽ
	while(C2CONbits.OPMOD != 0x04); //ȷ������ģʽ

	C2FIFOBA = KVA_TO_PA(Can2MsgFiFo); //�����ַת�����ַ

	Write_CAN_Baud();              //������ 250kb/S

	Write_SID_Filter(0x503);       //SID ��ʼ�� ����(0x100 - 0x107)
	Write_EID_Filter(0x1FFFFF00);  //EID ��ʼ�� ����(0x1FFFFF00 - 0x1FFFFF07)
	
	C2FIFOCON0bits.FSIZE = 1;      //�������  2
	C2FIFOCON0bits.TXEN  = 1;      //FIFO0->����
	
	C2FIFOCON1bits.FSIZE = 1;      //�������  2
	C2FIFOCON1bits.TXEN  = 0;      //FIFO1->����
	
	C2FIFOCON2bits.FSIZE = 1;      //�������  2
	C2FIFOCON2bits.TXEN  = 0;      //FIFO2->����
		
	C2FIFOINT1bits.RXNEMPTYIE = 1; //FIFO 1 ���շǿ��ж�
	C2FIFOINT2bits.RXNEMPTYIE = 1; //FIFO 2 ���շǿ��ж�
	
	C2INTbits.RBIE = 1;            //�����ж�����
	C2INTbits.TBIE = 0;            //�����жϽ�ֹ
	
	IPC11bits.CAN2IP = 4;          //CAN2 �ж�
	IPC11bits.CAN2IS = 1;
	IEC1bits.CAN2IE = 1;
	
//	C2CONbits.REQOP = 0x2;         //���󻷻�ģʽ   �����ڵ� ����,ʹ�����
//	while (C2CONbits.OPMOD!=0x2);  //ȷ�ϻ���ģʽ

	C2CONbits.REQOP = 0x0;         //��������ģʽ   ����ڵ� ����,ʹ�����
	while (C2CONbits.OPMOD!=0x0);  //ȷ������ģʽ 
}  

//-------------------------------------------------------------------------------
//	CAN ���� SID ����
//-------------------------------------------------------------------------------
void CAN_TX_SID_Message(unsigned short MsgSID)
{
	TxMessage = PA_TO_KVA1(C2FIFOUA0);  //��ȡ���ĵ�ַ
	
	TxMessage->CMSGSID.SID = MsgSID&0x7FFF;  //���� ID
	TxMessage->CMSGEID.IDE = 0;         //��׼����
	TxMessage->CMSGEID.DLC = 0x08;      //���ݳ��� 8 Byte
	
	TxMessage->CMSGDATA0.Byte0 = 0x01;  //CMSGDAT0
	TxMessage->CMSGDATA0.Byte1 = 0x02;
	TxMessage->CMSGDATA0.Byte2 = 0x04;
	TxMessage->CMSGDATA0.Byte3 = 0x08;
	
	TxMessage->CMSGDATA1.Byte4 = 0x0A;  //CMSGDAT1
	TxMessage->CMSGDATA1.Byte5 = 0x0C;
	TxMessage->CMSGDATA1.Byte6 = 0x0E;
	TxMessage->CMSGDATA1.Byte7 = TXbuffer[7]++;
	
	_LATE7 = 1;                         //LEDָʾ
	
	C2FIFOCON0SET = 0x00002000;         //UINC = 1
	C2FIFOCON0SET = 0x00000008;         //������
}
//-------------------------------------------------------------------------------
//	CAN ���� SID ����
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
    TxMessage = PA_TO_KVA1(C2FIFOUA0);  //��ȡ���ĵ�ַ
	
	TxMessage->CMSGSID.SID = MsgSID&0x7FFF;  //���� ID
	TxMessage->CMSGEID.IDE = 0;         //��׼����
	TxMessage->CMSGEID.DLC = 0x08;      //���ݳ��� 8 Byte
	
	TxMessage->CMSGDATA0.Byte0 = VMSDATA.Control_command;  //CMSGDAT0
	TxMessage->CMSGDATA0.Byte1 = VMSDATA.Set_power&0xFF;
	TxMessage->CMSGDATA0.Byte2 = VMSDATA.Set_power>>8;
	TxMessage->CMSGDATA0.Byte3 = 0;
	
	TxMessage->CMSGDATA1.Byte4 = 0;  //CMSGDAT1
	TxMessage->CMSGDATA1.Byte5 = 0;
	TxMessage->CMSGDATA1.Byte6 = 0;
	TxMessage->CMSGDATA1.Byte7 = TXbuffer[7]++;
	
	_LATE7 = 1;                         //LEDָʾ
	
	C2FIFOCON0SET = 0x00002000;         //UINC = 1
	C2FIFOCON0SET = 0x00000008;         //������
}
//-------------------------------------------------------------------------------
//	CAN ���� SID ����
//-------------------------------------------------------------------------------
void CAN_TX_EID_Message(unsigned int SendEID)
{
	TxMessage = PA_TO_KVA1(C2FIFOUA0);  //��ȡ���ĵ�ַ
	
	TxMessage->CMSGSID.SID = (SendEID>>18)&0x7FF;  //11 bit 
	TxMessage->CMSGEID.EID = SendEID&0x3FFFF;      //18 bit
	
	TxMessage->CMSGEID.IDE = 1;         //��׼����
	TxMessage->CMSGEID.DLC = 0x08;      //���ݳ��� 8 Byte
	
	TxMessage->CMSGDATA0.Byte0 = 0x10;  //CMSGDAT0
	TxMessage->CMSGDATA0.Byte1 = 0x20;
	TxMessage->CMSGDATA0.Byte2 = 0x40;
	TxMessage->CMSGDATA0.Byte3 = 0x80;
	
	TxMessage->CMSGDATA1.Byte4 = 0xA0;  //CMSGDAT1
	TxMessage->CMSGDATA1.Byte5 = 0xC0;
	TxMessage->CMSGDATA1.Byte6 = 0xE0;
	TxMessage->CMSGDATA1.Byte7 = TXbuffer[7]++;
	
	_LATE6 = 1;                         //LEDָʾ
	
	C2FIFOCON0SET = 0x00002000;         //UINC = 1
	C2FIFOCON0SET = 0x00000008;         //������
}

//-------------------------------------------------------------------------------
//	CAN ���ձ��Ĵ���
//-------------------------------------------------------------------------------
void CAN2RxMsgProcess(void)
{
	if(RX_SID_Msg_Flag == 1)               //FIFO0 �յ�����
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
            DCH1CONbits.CHEN = 1;               //����DMA ����*/
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
            DCH1CONbits.CHEN = 1;               //����DMA ����*/
        }        
        
		RX_SID_Msg_Flag = 0;
		
		C2FIFOCON1SET = 0x00002000;        //UINC = 1  �Ѷ�
		C2FIFOINT1bits.RXNEMPTYIE = 1;     //���ջ������ж�
	}
	
	if(RX_EID_Msg_Flag == 1)               //FIFO2 �յ�����
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

		C2FIFOCON2SET = 0x00002000;        //UINC = 1  �Ѷ�
		C2FIFOINT2bits.RXNEMPTYIE = 1;     //���ջ������ж�
	}		
}	
//-------------------------------------------------------------------------------
//	CAN2 �жϺ���  ע���жϱ��
//-------------------------------------------------------------------------------
void __attribute__((vector(47)))__attribute__((interrupt(IPL4)))_CAN2Interrupt(void)
{
	if(C2INTbits.RBIF == 1)                   //�����ж� 
	{
		if(C2FIFOINT1bits.RXNEMPTYIF == 1)    //FIFO 1
		{
			_LATE1 = 1;                       //LEDָʾ
			RX_SID_Msg_Flag = 1;              //�յ�����
			C2FIFOINT1bits.RXNEMPTYIE = 0;    //�жϽ�ֹ
		}
		if(C2FIFOINT2bits.RXNEMPTYIF == 1)    //FIFO 2
		{
			_LATE0 = 1;                       //LEDָʾ
			RX_EID_Msg_Flag = 1;              //�յ�����
			C2FIFOINT2bits.RXNEMPTYIE = 0;    //�жϽ�ֹ
		}
		C2INTbits.RBIF = 0;
	}
	
	if(C2INTbits.TBIF == 1)                   //�����ж�
	{
		_LATE0 = !_LATE0;
		C2INTbits.TBIF = 0;
	}	
	IFS1bits.CAN2IF = 0;
}

	

//-------------------------------------------------------------------------------

