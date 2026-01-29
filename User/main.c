#include "stm32f10x.h"                  // Device header
#include "timer.h"
#include "math.h"

int main() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 推挽复用
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2ENR_TIM1EN, ENABLE); // 使能定时器1时钟
    RCC_APB2PeriphResetCmd(RCC_APB2ENR_TIM1EN, ENABLE); // 产生复位信号
    RCC_APB2PeriphResetCmd(RCC_APB2ENR_TIM1EN, DISABLE);

    // 初始化时基单元
    TIM_TimeBaseInitTypeDef tim1;
    tim1.TIM_Period = 1000 - 1;
    tim1.TIM_CounterMode = TIM_CounterMode_Up;
    tim1.TIM_Prescaler = 72 - 1;
    tim1.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &tim1);

    // 初始化输出比较通道1
    TIM_OCInitTypeDef oc1;
    oc1.TIM_OCMode = TIM_OCMode_PWM1;
    oc1.TIM_OCPolarity = TIM_OCPolarity_High;
    oc1.TIM_OutputState = TIM_OutputState_Enable;
    oc1.TIM_Pulse = 0;
    TIM_OC1Init(TIM1, &oc1);

    TIM_CtrlPWMOutputs(TIM1, ENABLE); // 闭合MOE

    TIM_GenerateEvent(TIM1, TIM_EventSource_Update); // 手动产生update事件，来更新对应寄存器的值

    TIM_Cmd(TIM1, ENABLE);

    User_Timer_Init();

    while (1){
        float time = User_Timer_GetTick() * 0.001;
        float duty = 0.5 + 0.5 * sin(0.6*3.14*time);
        uint16_t ccr = duty * 1000;
        TIM_SetCompare1(TIM1, ccr);
    }
}
