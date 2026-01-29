#include "IIC.h"
void User_IIC_Init(void) {
    // 初始化SCL和SDA引脚
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 复用开漏
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE); // 复位I2C1外设
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_ClockSpeed = 400000; // 400KHZ
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);
}


/**
 * @brief 
 * 
 * @param write_addr 七位地址+写位
 * @param data 数据指针
 * @param len 数据长度
 */
ErrorStatus User_IIC_SendDatas(uint8_t write_addr, uint8_t *data, uint16_t len) {
    ErrorStatus return_val = SUCCESS;
    // 等待总线空闲
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    // 清除AF标志位
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    // 发送起始信号
    I2C_GenerateSTART(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET); // 等待起始信号发送完成

    I2C_SendData(I2C1, write_addr); // 发送从机地址
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET) { // 只有接收到ack才会将ADDR位置1
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) { // 地址发送失败 没有应答
            return_val = ERROR; // 返回错误
            goto Stop;
        }
    }
    // 清除ADDR标志位
    I2C_ReadRegister(I2C1, I2C_Register_SR1);
    I2C_ReadRegister(I2C1, I2C_Register_SR2);

    I2C_ClearFlag(I2C1, I2C_FLAG_AF); // 清除AF标志位
    for (u16 i = 0; i < len; i++) {
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET) {  // 等待TDR寄存器为空
            if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) { // 数据发送失败 没有应答
                return_val = ERROR;
                goto Stop;
            }
        }
        I2C_SendData(I2C1, *data); // 向fa
        data++;
    }
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET) { // 等待数据发送完成
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) { // 数据发送失败 没有应答
            return_val = ERROR;
            goto Stop;
        }
    }
    
    Stop: // 发送停止信号
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待总线空闲

    return return_val;
}



ErrorStatus User_Receive_Datas(uint8_t read_addr, uint8_t *data, uint16_t len) {
    ErrorStatus return_val = SUCCESS;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); // 等待总线空闲
    I2C_ClearFlag(I2C1, I2C_FLAG_AF); // 清除AF标志位
    I2C_GenerateSTART(I2C1, ENABLE); // 产生起始信号
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET); // 等待起始信号发送完成
    I2C_SendData(I2C1, read_addr); // 发送从机读地址
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET) {
        if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF) == SET) { // 地址发送失败 没有应答
            return_val = ERROR;
            goto Stop;
        }
    }
    I2C_ReadRegister(I2C1, I2C_Register_SR1);
    I2C_ReadRegister(I2C1, I2C_Register_SR2); // 清除ADDR标志位

    // 由于IIC接收数据采用了双缓冲结构所以再等待数据到达RDR寄存器之前必须先将Ack置位 防止误读

    if (len == 1) {
        I2C_AcknowledgeConfig(I2C1, DISABLE); // 提前准备发送ack
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET); // 等待数据到达RDR寄存器
				I2C_GenerateSTOP(I2C1, ENABLE); // 提前准备发送停止信号
        *data = I2C_ReceiveData(I2C1);
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
        return return_val;
    } else if (len == 2) {
        I2C_AcknowledgeConfig(I2C1, ENABLE); // 置位第一个数据的ack
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET); // 等待数据到达RDR寄存器
        I2C_AcknowledgeConfig(I2C1, DISABLE); // 在接收到最后一个数据之前提前准备发送nack
        I2C_GenerateSTOP(I2C1, ENABLE); // 提前准备发送停止信号
        *data = I2C_ReceiveData(I2C1);
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
        *(data + 1) = I2C_ReceiveData(I2C1);
        while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
        return return_val;
    } else {
        I2C_AcknowledgeConfig(I2C1, ENABLE); // 置位第一个数据的ack
        while (len) {
            if (len == 2) { // 倒数第二个数据时提前准备发送nack
                while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET); // 等待倒数第二数据到达RDR寄存器    
                I2C_AcknowledgeConfig(I2C1, DISABLE); // 提前准备发送nack
                I2C_GenerateSTOP(I2C1, ENABLE); // 提前准备发送停止信号
                *data = I2C_ReceiveData(I2C1); // 接收倒数第二个数据
                data++;
                while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET); // 等待最后一个数据到达RDR寄存器 
                *data = I2C_ReceiveData(I2C1);
                while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待总线空闲
                return return_val;
            }
            while (I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET); // 等待数据到达RDR寄存器
            *data = I2C_ReceiveData(I2C1);
            data++;
            len--;
        }
        
    }
    
    Stop: // 发送停止信号
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET); // 等待总线空闲
    return return_val;
}
