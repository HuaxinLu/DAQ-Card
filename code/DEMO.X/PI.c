#include "PI.h"
struct PID PID_Vo={12.0,0.015,0.0,{0,0,0},35.05,0,1.0};
struct PID PID_Io={40,0.8,0.000,{0,0,0},1500.0,0,1.0};

int PIFlag0=0;
//PIFlag 0=不调整PI并且关闭PWMgenerator
//1=CVS
//2=CCS
/**PID**/
double Vo_PID_DeltaU(double nextpoint)//增量型PID
{
	double output;
	PID_Vo.ADC_error[0]=PID_Vo.ADC_error[1];
	PID_Vo.ADC_error[1]=PID_Vo.ADC_error[2];
	PID_Vo.ADC_error[2]=(nextpoint-PID_Vo.setpoint);//(int)((Vo_av-Set.Vo_set)*100);//10mV
	output=(PID_Vo.K_p+PID_Vo.K_i+PID_Vo.K_d)*PID_Vo.ADC_error[2]-(PID_Vo.K_d*2+PID_Vo.K_p)*PID_Vo.ADC_error[1]-PID_Vo.K_d*PID_Vo.ADC_error[0];
	return output;//PID增量
}
void Vo_PID(double nextpoint)
{
	PID_Vo.Plusewidth_U += Vo_PID_DeltaU(nextpoint);//PID修正量
	PID_Vo.Ratio_U = (1+((double)PID_Vo.Plusewidth_U*0.001));
	//if (PID_Vo.Plusewidth_U>20) PID_Vo.Plusewidth_U=20;
	//if (PID_Vo.Plusewidth_U<-20) PID_Vo.Plusewidth_U=-20;
	//if(PID_Vo.TBC_U>PID_Vo.TBC_Umax)
	//{PID_Vo.TBC_U=PID_Vo.TBC_Umax;flag.flag_OVF=1;}
	//if(PID_Vo.TBC_U<-PID_Vo.TBC_Umax)
	//{PID_Vo.TBC_U=-PID_Vo.TBC_Umax;flag.flag_OVF=1;}//PID限幅
	//PID_Vo.Ratio_U = (1+((double)PID_Vo.TBC_U*0.001));
	/*if(PID_Vo.Ratio_U>PID_Vo.Ratio_Umax)
		PID_Vo.Ratio_U=PID_Vo.Ratio_Umax;
	if(PID_Vo.Ratio_U<PID_Vo.Ratio_Umin)
		PID_Vo.Ratio_U=PID_Vo.Ratio_Umin;//PID限幅*/
}

double Io_PID_DeltaU(double nextpoint)//增量型PID
{
	double output;
	PID_Io.ADC_error[0]=PID_Io.ADC_error[1];
	PID_Io.ADC_error[1]=PID_Io.ADC_error[2];
	PID_Io.ADC_error[2]=(nextpoint-PID_Io.setpoint);//(int)((Vo_av-Set.Vo_set)*100);//10mV
	output=(PID_Io.K_p+PID_Io.K_i+PID_Io.K_d)*PID_Io.ADC_error[2]-(PID_Io.K_d*2+PID_Io.K_p)*PID_Io.ADC_error[1]-PID_Io.K_d*PID_Io.ADC_error[0];
	return output;//PID增量
}

void Io_PID(double nextpoint)
{
	PID_Io.Plusewidth_U += Io_PID_DeltaU(nextpoint);//PID修正量
	PID_Io.Ratio_U = (1+((double)PID_Io.Plusewidth_U*0.001));
	//if (PID_Vo.Plusewidth_U>20) PID_Vo.Plusewidth_U=20;
	//if (PID_Vo.Plusewidth_U<-20) PID_Vo.Plusewidth_U=-20;
	//if(PID_Vo.TBC_U>PID_Vo.TBC_Umax)
	//{PID_Vo.TBC_U=PID_Vo.TBC_Umax;flag.flag_OVF=1;}
	//if(PID_Vo.TBC_U<-PID_Vo.TBC_Umax)
	//{PID_Vo.TBC_U=-PID_Vo.TBC_Umax;flag.flag_OVF=1;}//PID限幅
	//PID_Vo.Ratio_U = (1+((double)PID_Vo.TBC_U*0.001));
	/*if(PID_Vo.Ratio_U>PID_Vo.Ratio_Umax)
		PID_Vo.Ratio_U=PID_Vo.Ratio_Umax;
	if(PID_Vo.Ratio_U<PID_Vo.Ratio_Umin)
		PID_Vo.Ratio_U=PID_Vo.Ratio_Umin;//PID限幅*/
}
