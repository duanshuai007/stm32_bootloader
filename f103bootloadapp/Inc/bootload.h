#ifndef _BOOTLOAD_103_H_
#define _BOOTLOAD_103_H_

#include "stdint.h"
#include "user_config.h"

typedef struct {
  UART_HandleTypeDef  *uart;    //����
  DMA_HandleTypeDef   *uart_rx_hdma;  //���ڽ���dma���
  uint16_t dma_rbuff_size;  //dma���ջ������Ĵ�С
  uint8_t *dma_rbuff;         //ָ��dma���ջ�����
} UartModule;

/*
*   ��ʼ����F405ͨ�ŵĴ�����ؽṹ��
*/
void BootLoad_Uart2Init(void);

/*
*   ��ȡuart module�ṹ��
*/
UartModule *GetF405UartModule(UART_HandleTypeDef *huart);

/*
*   �жϺ���:F405���պ���
*   �ڴ��ڽ�������ж��б�����
*/
void F405ReveiveHandler(UART_HandleTypeDef *huart);

bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart);

#endif



