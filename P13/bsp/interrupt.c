#include "interrupt.h"
#include "usart.h"

struct keys key[4]={0};

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
						key[i].single_sta=1;
					}else{
						key[i].judge_sta=0;
					}
					break;
				case 2:
					if(key[i].key_sta==1){
						key[i].judge_sta=0;
					}
					break;
			}
		}
	}
}


char rxbuf[7];
uint8_t rxdata;
int rx_pos;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){	
	rxbuf[rx_pos++]=rxdata;
	HAL_UART_Receive_IT(&huart1,&rxdata,1);
}
