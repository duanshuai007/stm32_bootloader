#ifndef _BOOTLOAD_103_H_
#define _BOOTLOAD_103_H_

#include "stdint.h"
#include "user_config.h"

typedef struct {
  UART_HandleTypeDef  *uart;    //串口
  DMA_HandleTypeDef   *uart_rx_hdma;  //串口接收dma句柄
  uint16_t dma_rbuff_size;  //dma接收缓冲区的大小
  uint8_t *dma_rbuff;         //指向dma接收缓冲区
} UartModule;

/*
*   初始化与F405通信的串口相关结构体
*/
void BootLoad_Uart2Init(void);

/*
*   获取uart module结构体
*/
UartModule *GetF405UartModule(UART_HandleTypeDef *huart);

/*
*   中断函数:F405接收函数
*   在串口接收完成中断中被调用
*/
void F405ReveiveHandler(UART_HandleTypeDef *huart);

bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart);

#endif



