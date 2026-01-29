#ifndef __TIMER_H__
#define __TIMER_H__
#include "stm32f10x.h"

void User_Timer_Init(void);
uint64_t User_Timer_GetTick(void);
uint64_t User_Timer_GetTick_us(void);
void User_Timer_Delay_ms(uint32_t ms);
void User_Timer_Delay_us(uint32_t us);
#endif 
