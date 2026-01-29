#ifndef __IIC_H__
#define __IIC_H__
#include "stm32f10x.h"
void User_IIC_Init(void);
ErrorStatus User_IIC_SendDatas(uint8_t addr, uint8_t* data, uint16_t len);
ErrorStatus User_Receive_Datas(uint8_t read_addr, uint8_t *data, uint16_t len);
#endif // !__IIC_H__
