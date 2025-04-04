#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include "main.h"
#include "stdbool.h"

struct keys{
	uchar judge_sta;
	bool key_sta;
	bool single_sta;
};
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif
