#include "timer.h"

static volatile uint64_t current_time = 0; // ms

void User_Timer_Init(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // TIM3时钟使能
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE); // 复位
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);

    TIM_ARRPreloadConfig(TIM3, ENABLE); // 使能timer3的ARR自动装载
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // 预分频寄存器值为71 实际分频系数为72
    TIM_TimeBaseStructure.TIM_Period = 999; // 计时1000
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_GenerateEvent(TIM3, TIM_EventSource_Update); // 手动产生update事件，来更新对应寄存器的值

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); // 开启update中断

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure); // 配置中断

    TIM_Cmd(TIM3, ENABLE); // 使能TIM3    
}

void TIM3_IRQHandler(void) { 
    if (TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == SET) {
        current_time++;
        TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
    }
    
}

uint64_t User_Timer_GetTick(void) {
	return current_time;
}

uint64_t User_Timer_GetTick_us(void) {
    // cnt寄存器的值是每1us 加1所以可以通过获取当前的ms*1000加上cnt寄存器的值得到对应的us
	return current_time * 1000 + TIM_GetCounter(TIM3);
}

void User_Timer_Delay_ms(uint32_t delay_ms) {
    uint64_t start_time = current_time;
	while (current_time - start_time < delay_ms);
}

void User_Timer_Delay_us(uint32_t us) {
    uint64_t start_time = User_Timer_GetTick_us();
    while (User_Timer_GetTick_us() - start_time < us);
}
