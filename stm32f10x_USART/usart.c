#include "usart.h"

usart_receive_data_callback usart1_receive_data_callback = send_string; // 默认的接收回调函数为将接收到的数据原封不动地发送回去

static USART_FullInitTypeDef default_usart_full_init_structure = {
    .USART_InitStructure = {
        .USART_BaudRate = 9600, 
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_WordLength = USART_WordLength_8b
    },
    .NVIC_InitStructure = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0,
        .NVIC_IRQChannelSubPriority = 0,
        .NVIC_IRQChannelCmd = ENABLE
    },
    .receive_data_callback = NULL
};

u8 send_buff[100] = {0};
u8 receive_buff[100] = {0};
u8 receive_idx = 0;
u8 end_idx = 0;

void send_string(char* str, u8 len, USART_TypeDef * usartx) {
	while (len--) {
		send_buff[end_idx] = *str;
		end_idx += 1;
		str++;
	}
	USART_ITConfig(usartx, USART_IT_TXE, ENABLE); // 开启发送中断
}

u8 get_current_send_data() {
	u8 res = send_buff[0];

	for (u8 i = 0; i < end_idx - 1; i++) {
		send_buff[i] = send_buff[i + 1];
	}

	end_idx--;
	return res;
}

void usart1_init(USART_FullInitTypeDef* usart_full_init_structure) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpioa_init_structure;
	gpioa_init_structure.GPIO_Pin = GPIO_Pin_9;
	gpioa_init_structure.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioa_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioa_init_structure);

	gpioa_init_structure.GPIO_Pin = GPIO_Pin_10;
	gpioa_init_structure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &gpioa_init_structure);

	USART_Init(USART1, &usart_full_init_structure->USART_InitStructure); 
	
    GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 

	NVIC_Init(&usart_full_init_structure->NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

int fputc(int ch, FILE * file) {
	char tmp = (char) ch;
	send_string(&tmp, 1, USART1);
	while (USART_GetFlagStatus(USART1, USART_IT_TXE));
	return ch;
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_TXE) == SET) { // 如果发送寄存器为空
		if (end_idx == 0) {
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			return;
		}
		USART_SendData(USART1, get_current_send_data());
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
	}

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		receive_buff[receive_idx] = USART_ReceiveData(USART1);
		receive_idx ++;
		if (receive_idx > 2 && receive_buff[receive_idx - 2] == '\r' && receive_buff[receive_idx - 1] == '\n') { // 说明接收数据结束
            usart1_receive_data_callback((char *) receive_buff, receive_idx - 2, USART1); // 调用回调函数处理接收到的数据
			receive_idx = 0;
		}
	}
}


/**
 * @brief Get the default usart full init structure object
 * 
 * @return USART_FullInitTypeDef* 
 */
USART_FullInitTypeDef* get_default_usart1_full_init_structure(void) {
    return &default_usart_full_init_structure;
}
