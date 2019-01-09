#ifndef __UARTMODULE_H__
#define __UARTMODULE_H__

#include "stdint.h"
#include "stm32f4xx.h"
#include "user_config.h"
#include "datapool.h"

typedef struct {
  volatile uint32_t pos;
  UART_HandleTypeDef  *uart;    //串口
  uint16_t dma_rbuff_size;      //dma接收缓冲区的大小
  uint8_t *dma_rbuff;           //指向dma接收缓冲区
  
  DataPool *rxDataPool;
  DataPool *txDataPool;
} UartModule;

UartModule *GetUartModule(void);

UartModule *UartModuleInit(UART_HandleTypeDef *huart);

void UartModuleDataProcess(void);

bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart);

#endif



