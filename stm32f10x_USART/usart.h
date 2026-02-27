#ifndef __USART_H__
#define __USART_H__
#include "stm32f10x.h"          
#include "Delay.h"
#include "stdio.h"
typedef void (*usart_receive_data_callback)(char* str, u8 len, USART_TypeDef * usartx); // 定义一个函数指针类型用于接收数据的回调函数

typedef struct  {
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    usart_receive_data_callback receive_data_callback;
} USART_FullInitTypeDef;
void usart1_init(USART_FullInitTypeDef* usart_full_init_structure); // 由于这里使用的是中断进行发送和接收，所以在初始化之前需要先确定nvic的优先级分组
void send_string(char* str, u8 len, USART_TypeDef * usartx);
USART_FullInitTypeDef* get_default_usart1_full_init_structure(void);
#endif
