#include "interrupt.h"

struct keys key[4]={0};
int fA[30]={0},fB[30]={0};
int i=0;
int fmaxA,fminA,fmaxB,fminB,frqA_true=0,frqB_true=0;
extern int PD,PH,PX;
int NDA=0,NDB=0,NHA=0,NHB=0;
bool is_nullA=0,is_nullB=0;
int frqA=0,frqB=0,tA=0,tB=0;
int ccrA,ccrB;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM4){
			key[0].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0);
			key[1].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
			key[2].key_sta=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
			key[3].key_sta=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0);
			for(int i=0;i<4;i++){
				switch(key[i].judge_sta){
					case 0:
						if(key[i].key_sta==0){
							key[i].judge_sta=1;
						}
						break;
					case 1:
						if(key[i].key_sta==0){
							key[i].judge_sta=2;
							key[i].key_time=0;
						}else{
							key[i].judge_sta=0;
						}
						break;
					case 2:
						if(key[i].key_sta==1){
							key[i].judge_sta=0;
							if(key[i].key_time<900){
								key[i].single_sta=1;
							}
						}else{
							key[i].key_time++;
							if(key[i].key_time>1000){
								key[i].long_sta=1;
							}
						}
				}
			}
	}

	if(htim->Instance==TIM17){
		frqA_true=frqA+PX;
		frqB_true=frqB+PX;
		tA=1000000/frqA_true;
		tB=1000000/frqB_true;
		if(frqA_true<0) is_nullA=1;
		else is_nullA=0;
		if(frqB_true<0) is_nullB=1;
		else is_nullB=0;
		fA[i%30]=frqA_true;
		fB[i%30]=frqB_true;
		i++;
		fmaxA=fA[0];fminA=fA[0];fmaxB=fB[0];fminB=fB[0];
		for(int j=0;j<30;j++){
			if(fA[j]>fmaxA){
				fmaxA=fA[j];
			}
			if(fA[j]<fminA){
				fminA=fA[j];
			}
			if(fB[j]>fmaxB){
				fmaxB=fB[j];
			}
			if(fB[j]<fminB){
				fminB=fB[j];
			}
		}
		if(frqA_true>PH) NHA++;
		if(frqB_true>PH) NHB++;
		if(fmaxA-fminA>PD) NDA++;
		if(fmaxB-fminB>PD) NDB++;
	}
}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM2){
		ccrA=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		__HAL_TIM_SetCounter(htim,0);
		frqA=80000000/80/ccrA;
		HAL_TIM_IC_Start_IT(htim,TIM_CHANNEL_1);
	}
	if(htim->Instance==TIM3){
		ccrB=HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1);
		__HAL_TIM_SetCounter(htim,0);
		frqB=80000000/80/ccrB;
		HAL_TIM_IC_Start_IT(htim,TIM_CHANNEL_1);
	}
	
}
