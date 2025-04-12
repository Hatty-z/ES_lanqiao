#include "mymain.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "led.h"
#include "lcd.h"
#include "interrupt.h"
#include "tim.h"
#include "usart.h"

int views=0;
bool mode=0;
extern int frqA,frqB,tA,tB;
extern bool is_nullA,is_nullB;
extern int fmaxA,fminA,fmaxB,fminB,frqA_true,frqB_true;
int PD=1000,PH=5000,PX=0;
int param=0;
extern int NDA,NDB,NHA,NHB;
extern struct keys key[];
uchar led_mark;
int time100ms;

char temp[30];

void view_proc(void);
void key_proc(void);
void led_proc(void);
void lcd_update(void);

void setup(void){
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim17);
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
	time100ms=uwTick;
}
	
void loop(void){
	key_proc();
	led_proc();
	view_proc();
	lcd_update();
}

void view_proc(void){
	char text[30];
	if(views==0){
		sprintf(text,"         DATA ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);  
		if(!mode){
			if(is_nullA){
				sprintf(text,"      A=NULL ");
				LCD_DisplayStringLine(Line3, (unsigned char *)text);  
			}else{
				if(frqA>=1000){
					sprintf(text,"      A=%.2fKHz ",(float)frqA_true/1000);
					LCD_DisplayStringLine(Line3, (unsigned char *)text); 
				}else{
					sprintf(text,"      A=%dHz ",frqA_true);
					LCD_DisplayStringLine(Line3, (unsigned char *)text);   
				}
			}
			if(is_nullB){
				sprintf(text,"      B=NULL ");
				LCD_DisplayStringLine(Line4, (unsigned char *)text);   
			}else{
				if(frqB>=1000){
					sprintf(text,"      B=%.2fKHz ",(float)frqB_true/1000);
					LCD_DisplayStringLine(Line4, (unsigned char *)text);   
				}else{
					sprintf(text,"      B=%dHz ",frqB_true);
					LCD_DisplayStringLine(Line4, (unsigned char *)text);   
				}
			}
			
		}else{
			if(is_nullA){
				sprintf(text,"      A=NULL ");
				LCD_DisplayStringLine(Line3, (unsigned char *)text);   
			}else{
				if(tA>=1000){
					sprintf(text,"      A=%.2fmS ",(float)tA/1000);
					LCD_DisplayStringLine(Line3, (unsigned char *)text);  
				}else{
					 sprintf(text,"      A=%duS ",tA);
					 LCD_DisplayStringLine(Line3, (unsigned char *)text); 
				}
			}
			if(is_nullB){
				sprintf(text,"      B=NULL ");
				LCD_DisplayStringLine(Line4, (unsigned char *)text);  
			}else{
				if(tB>=1000){
					sprintf(text,"      B=%.2fmS ",(float)tB/1000);
					LCD_DisplayStringLine(Line4, (unsigned char *)text);   
				}else{
					sprintf(text,"      B=%duS ",tB);
					LCD_DisplayStringLine(Line4, (unsigned char *)text);   
				}
			}
		}
		
	}
	if(views==1){
		sprintf(text,"         PARA ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);  
		sprintf(text,"      PD=%dHz ",PD);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);  
		sprintf(text,"      PH=%dHz ",PH);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);  
		sprintf(text,"      PX=%dHz ",PX);
		LCD_DisplayStringLine(Line5, (unsigned char *)text);  
	}
	if(views==2){
		sprintf(text,"         RECD ");
		LCD_DisplayStringLine(Line1, (unsigned char *)text);  
		sprintf(text,"      NDA=%d ",NDA);
		LCD_DisplayStringLine(Line3, (unsigned char *)text);  
		sprintf(text,"      NDB=%d ",NDB);
		LCD_DisplayStringLine(Line4, (unsigned char *)text);  
		sprintf(text,"      NHA=%d ",NHA);
		LCD_DisplayStringLine(Line5, (unsigned char *)text); 
		sprintf(text,"      NHB=%d ",NHB);
		LCD_DisplayStringLine(Line6, (unsigned char *)text);  		
	}
}

void key_proc(void){
	if(key[0].single_sta==1){
		if(param==0){
			PD+=100;
			if(PD>1000) PD=1000;
		}
		if(param==1){
			PH+=100;
			if(PH>10000) PH=10000;
		}
		if(param==2){
			PX+=100;
			if(PX>1000) PX=1000;
		}
		key[0].single_sta=0;
		LCD_Clear(Black);
	}
	if(key[1].single_sta==1){
		if(param==0){
			PD-=100;
			if(PD<100) PD=100;
		}
		if(param==1){
			PH-=100;
			if(PH<1000) PH=1000;
		}
		if(param==2){
			PX-=100;
			if(PX<-1000) PX=-1000;
		}
		key[1].single_sta=0;
		LCD_Clear(Black);
	}
	if(key[2].single_sta==1){
		if(views==0){
			mode=!mode;
		}
		if(views==1){
			param++;
			if(param>3) param=0;
		}
		key[2].single_sta=0;
		LCD_Clear(Black);
	}
	if(key[2].long_sta==1){
		if(views==2){
			NDA=0;NDB=0;NHA=0;NHB=0;
		}
		key[2].long_sta=0;
		LCD_Clear(Black);
	}
	if(key[3].single_sta==1){
		if(views==0){
			views=1;
			param=0;
		}else if(views==1){
			views=2;
		}else if(views==2){
			views=0;
			mode=0;
		}
		key[3].single_sta=0;
		LCD_Clear(Black);
	}
}
void led_proc(void){
	if(views==0) led_mark|=0x01;
	else led_mark &=~0x01;
	if(frqA_true>PH) led_mark|=0x02;
	else led_mark &=~0x02;
	if(frqB_true>PH) led_mark|=0x04;
	else led_mark &=~0x04;
	if( NDA>=3 || NDB>=3 || NHA>=3 || NHB>=3) led_mark|=0x80;
	else led_mark &=~0x80;
	led_disp(led_mark);
}

void lcd_update(void){
	if(uwTick-time100ms>100){
		LCD_Clear(Black);
		time100ms=uwTick;
	}
}
