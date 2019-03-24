#include "DP.h"
unsigned char buffer1[16]={0};
unsigned char buffer2[16]={0};
unsigned char buffer3[16]={0};
unsigned char buffer4[16]={0};
unsigned char buffer5[18]={0};
int AD7609Data[8]={0};
int AD7609Data_AVG[8]={0};
int BatVolt[32]={0};
int SensorVolt[8]={0};
int SensorAmp[8]={0};
long CaptureTime1,CaptureTime2;
unsigned int Frequency1;
unsigned char AD7609_Read;;
unsigned char Frequency1_Read;
int AD7609_BUF[8][AD7609_FILTER_NUM]={0};
int AD7609_Offest[8]={-10,-10,6,1,-5,-5,-7,-6};
int Wfm_Data;
int Air_Flow_Data;
int Air_Psi_Data;
int Air_Motor;
int Water_Input_temp;
int Water_Output_temp;
unsigned char AD7609_filter_cnt=0;
void AD7609_Data_Filter()
{
    unsigned char i,j;
    long int temp[8]={0};
    for(i=0;i<8;i++)
    {
        AD7609_BUF[i][AD7609_filter_cnt]=AD7609Data[i];
    }
    for(i=0;i<8;i++)
    {
        for(j=0;j<AD7609_FILTER_NUM;j++)
            temp[i]+=AD7609_BUF[i][j];
    }    
    for(i=0;i<8;i++)
        AD7609Data[i]=(float)temp[i]/AD7609_FILTER_NUM+0.5f;
    
    AD7609_filter_cnt++;
    if(AD7609_filter_cnt==AD7609_FILTER_NUM)
    {
        AD7609_filter_cnt=0;
    }
}
void Data_Processing()
{
    unsigned char i;
    int aa;
    if(AD7609_Read)//需要进行数据刷新
    {
        AD7609Data[7]=(buffer5[0]<<10|buffer5[1]<<2|buffer5[2]>>6);
        AD7609Data[6]=((buffer5[2]&0x3F)<<12|buffer5[3]<<4|buffer5[4]>>4);  
        AD7609Data[5]=((buffer5[4]&0x0F)<<14|buffer5[5]<<6|buffer5[6]>>2);  
        AD7609Data[4]=((buffer5[6]&0x03)<<16|buffer5[7]<<8|buffer5[8]);
        AD7609Data[3]=(buffer5[9]<<10|buffer5[10]<<2|buffer5[11]>>6);
        AD7609Data[2]=(buffer5[11]&0x3F)<<12|buffer5[12]<<4|buffer5[13]>>4;  
        AD7609Data[1]=((buffer5[13]&0x0F)<<14|buffer5[14]<<6|buffer5[15]>>2);  
        AD7609Data[0]=((buffer5[15]&0x03)<<16|buffer5[16]<<8|buffer5[17]);
        for(i=0;i<8;i++)
        {
            if((AD7609Data[i]&0x00020000)!=0)
                AD7609Data[i]|=0xFFFE0000;
            AD7609Data[i]=AD7609Data[i]+AD7609_Offest[i];
        }
        AD7609_Data_Filter();
        for(i=0;i<8;i++)
        {
            SensorAmp[i]=(int)(AD7609Data[i]*0.03815f+1.5f);
        }        
        AD7609_Read=0;
        
        Wfm_Data=(int)(1.5625f*SensorAmp[0]-625+0.5f);
        Air_Flow_Data=(int)(62.5f*SensorAmp[1]-25000+0.5f);
        Air_Psi_Data=(int)(3.75f*SensorAmp[3]-1500+0.5f);
        Water_Input_temp=(int)(SensorAmp[5]*7.936-4602.3+0.5);
        Water_Output_temp=(int)(SensorAmp[4]*7.936-4602.3+0.5);
    }

    if(Frequency1_Read)//需要进行数据刷新
    {
        if(CaptureTime2>0) 
            Frequency1=(int)(80000000/CaptureTime2+5)/10;
        else if (CaptureTime2==0)
            Frequency1=0;   
        Frequency1_Read=0;
        Air_Motor=(int)(2.5f*Frequency1+0.5f);                                                                                                                                                                                                                                                                                                           
    }

}
