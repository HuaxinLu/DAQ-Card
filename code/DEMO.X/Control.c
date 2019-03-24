#include "Control.h"
#include "uart_dma.h"
#include "config.h"
#include "timers.h"
#include "can.h"
#include "SPI.h"
#include "PWM.h"
#include "CNIT.h"
#include "cap.h"
#include "DP.h"
unsigned char Temp_Start=0;
int Pump_Vol;
PID_DATA Temperature={0,0,0,0,0,0,0,0,4990,3000};
void Water_Control()
{
    Temperature.error[0]=Temperature.error[1];
    Temperature.error[1]=Temperature.error[2];
    Temperature.error[2]=Water_Output_temp/100.0f-Temperature.Expect;
    Temperature.Output=(Temperature.Kp+Temperature.Ki+Temperature.Kd)*Temperature.error[2]-(Temperature.Kp+Temperature.Kd*2)*Temperature.error[1]+Temperature.Kd*Temperature.error[0];
    Pump_Vol+=(int)Temperature.Output;
    if(Pump_Vol>Temperature.OutputMax) Pump_Vol=Temperature.OutputMax;
    if(Pump_Vol<Temperature.OutputMin) Pump_Vol=Temperature.OutputMin;
    AD5684_A_Data=(int)(Pump_Vol* 0.82f)<<4;
    Write_AD5684_A_Flag=1;
    if((Temperature.error[2]<0.5f)&&(Temperature.error[2]<Temperature.error[1])) _LATG12=1; //降温过程，温度小于0.5就关
    else if(Temperature.error[2]>0.6f) _LATG12=0; //无论如何温度大于0.8就开
    else if(((Temperature.error[2]>0.2)&&(Temperature.error[2]>Temperature.error[1]))) _LATG12=0; //升温过程，温差小于0.5就开
    else if (Temperature.error[2]<-0.6f) _LATG12=1; 
}
void Clear_Output()
{
    //关闭所有继电器
    _LATE4=1;
    _LATE3=1;
    _LATE2=1;
    _LATG13=1;
    
    _LATG12=1;
    _LATG14=1;
    _LATE0=1;
    _LATE1=1;
    
    _LATA7=1;
    _LATA6=1;
    _LATD3=1;
    _LATD2=1;
    //DA输出清零
    AD5684_A_Data=0;
    AD5684_B_Data=0;
    AD5684_C_Data=0;
    AD5684_D_Data=0;
}