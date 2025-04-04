#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "mymain.h"
#include "led.h"
#include "lcd.h"
#include "interrupt.h"
#include "tim.h"
#include "usart.h"

uchar led_mark=0x00;
uint32_t time100ms;
uint32_t time5000ms;
uint32_t time5000ms1;
bool led_sta=0;
bool views=0;
int b1=1,b2=2,b3=3,b1_old,b2_old,b3_old,b1_new,b2_new,b3_new;
int b1_temp=-1,b2_temp=-1,b3_temp=-1;
int frq,d;
extern struct keys key[];
bool pass[3]={0};
int failcount=0;
extern char rxbuf[7];
extern uint8_t rxdata;
extern int rx_pos;
void view_proc(void);
void key_proc(void);
void rx_proc(void);

void setup(void){
	led_dspl(led_mark);
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
	HAL_UART_Receive_IT(&huart1,&rxdata,1);
}
void loop(void){
	rx_proc();
	key_proc();
	view_proc();
}

void view_proc(void){
	char text[30];
	if(views==0){
		sprintf(text,"        PSD");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		if(pass[0]) sprintf(text,"     B1:%d",b1_temp);
		else sprintf(text,"     B1:@");
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		if(pass[1]) sprintf(text,"     B2:%d",b2_temp);
		else sprintf(text,"     B2:@");
	  LCD_DisplayStringLine(Line4, (unsigned char *)text);
		if(pass[2]) sprintf(text,"     B3:%d",b3_temp);
		else sprintf(text,"     B3:@");
		LCD_DisplayStringLine(Line5, (unsigned char *)text);	
	}
	if(views==1){
		sprintf(text,"        STA");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);
		frq=80000000/80/(__HAL_TIM_GetAutoreload(&htim2)+1);
		sprintf(text,"     F:%dHz",frq);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);
		d=__HAL_TIM_GetCompare(&htim2,TIM_CHANNEL_2);
		sprintf(text,"     D:%d%%",d);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);
	}
}

void key_proc(void){
	if(views==0){
		if(key[0].single_sta==1){
			pass[0]=1;
			b1_temp++;
			if(b1_temp>=10) b1_temp=0;
			key[0].single_sta=0;
			LCD_Clear(Black);
		}
		if(key[1].single_sta==1){
			pass[1]=1;
			b2_temp++;
			if(b2_temp>=10) b2_temp=0;
			key[1].single_sta=0;
			LCD_Clear(Black);
		}
		if(key[2].single_sta==1){
			pass[2]=1;
			b3_temp++;
			if(b3_temp>=10) b3_temp=0;
			key[2].single_sta=0;
			LCD_Clear(Black);
		}
		if(key[3].single_sta==1){
			if(b1_temp==b1 && b2_temp==b2 && b3_temp==b3){
				views=1;
				key[3].single_sta=0;
				LCD_Clear(Black);
				__HAL_TIM_SET_AUTORELOAD(&htim2,500-1);
				__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,10);
				time5000ms=uwTick;
				led_mark |=0x01;
				led_dspl(led_mark);
			}
			else{
				failcount++;
				time100ms=uwTick;
				time5000ms1=uwTick;
				pass[0]=0;pass[1]=0;pass[2]=0;
				b1_temp=-1;b2_temp=-1;b3_temp=-1;
				key[3].single_sta=0;
				LCD_Clear(Black);
			}
		}
	}
	if(views==1 && (uwTick-time5000ms>5000)){
		led_mark &=~0x01;
		led_dspl(led_mark);
		__HAL_TIM_SetAutoreload(&htim2,1000-1);
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,50);
		views=0;
		pass[0]=0;pass[1]=0;pass[2]=0;
		b1_temp=-1;b2_temp=-1;b3_temp=-1;
		LCD_Clear(Black);
	}
	if(failcount>=3 &&(time100ms-uwTick>100)&& (uwTick-time5000ms1<5000)){
		if(led_sta) led_mark|=0x02;
		else led_mark&=~0x02;
		led_dspl(led_mark);
		led_sta=!led_sta;
		time100ms=uwTick;
	}else {
		led_mark&=~0x02;
		led_dspl(led_mark);
	}
}

void rx_proc(void){
	if(rx_pos>0){
		if(rx_pos==7){
			sscanf(rxbuf,"%1d%1d%1d-%1d%1d%1d",
						 &b1_old, &b2_old, &b3_old,
						 &b1_new, &b2_new, &b3_new);
			if(b1_old==b1 && b2_old==b2 && b3_old==b3){
				b1=b1_new; b2=b2_new; b3=b3_new;
			}
			HAL_UART_Transmit(&huart1,(uint8_t *)rxbuf, 8, 50);
		}
		rx_pos=0;
		memset(rxbuf,0,7);
	}
}
