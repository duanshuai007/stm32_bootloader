#include "bootload.h"
#include "stdint.h"
#include "stm32f1xx.h"
#include "user_config.h"
#include "stdlib.h"
#include "crc16.h"
#include "flash.h"

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

uint8_t uart2_dma_recv_buffer[1024] = {0};
UartModule gUartMx;

/*
*   ��ʼ����F405ͨ�ŵĴ�����ؽṹ��
*/
void BootLoad_Uart2Init(void)
{
  gUartMx.uart = &huart2;
  gUartMx.dma_rbuff_size = 1024;
  gUartMx.dma_rbuff = uart2_dma_recv_buffer;
  gUartMx.uart_rx_hdma = &hdma_usart2_rx;
  
  HAL_UART_Receive_DMA(gUartMx.uart, gUartMx.dma_rbuff, gUartMx.dma_rbuff_size);
  //ʹ�ܿ����жϣ����Խ���������
  __HAL_UART_ENABLE_IT(gUartMx.uart, UART_IT_IDLE);
}

/*
*   ��ȡuart module�ṹ��
*/
UartModule *GetF405UartModule(UART_HandleTypeDef *huart)
{
  if ( huart->Instance == gUartMx.uart->Instance ) {
    return &gUartMx;
  } else 
    return NULL;
}

/*
*   �жϺ���:F405���պ���
*   �ڴ��ڽ�������ж��б�����
*/
void F405ReveiveHandler(UART_HandleTypeDef *huart)
{
  if ( huart->Instance != gUartMx.uart->Instance ) 
    return;
  
//  CopyDataFromDMAHandler(gUartMx.uart);
  HAL_UART_Receive_DMA(gUartMx.uart, 
                       gUartMx.dma_rbuff,
                       gUartMx.dma_rbuff_size);
}
//
//bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart)
//{
//  uint16_t len;
//  uint16_t pos;
//  UartModule *um = &gUartMx;
//  
//  //len �����Ѿ����ڴ�����ݵĳ���(�ռ�)
//  len = (um->dma_rbuff_size - (uint16_t)__HAL_DMA_GET_COUNTER(um->uart_rx_hdma));
//  
//  //��dma����������ȡ�����뵽���ݴ�����ڴ���
//  //��len��pos��ȡ������,��Ϊ�������
//  //1��len=pos=0 û������
//  //2��len > pos ���Գ�ȥ��pos��len������
//  //3��len < pos ˵�������˴��ڽ�������жϣ��ڸû����������ݴ����ˣ���Ҫȡ��һ��������������
//  //4: len = 0, pos > 0 �������ݽ�������жϣ���Ҫȡ����ǰdma�ڵĴ�pos��������������
//  
//  if (((0 == len) && (0 == um->pos)) || (len == um->pos))
//    return true;
//  
//  if (( 0 == len ) && ( um->pos > 0)) {
//    //���ݽ�������жϣ�������д��pool�к󣬽�pos���á�
//    pos = um->pos;
//
//    if (DataPoolWrite(um->rxDataPool, um->dma_rbuff + pos, um->dma_rbuff_size - pos )) {
//      um->pos = 0;
//      return true;
//    }
//  } else  if (len > um->pos) {
//    //�����жϣ�ȡ����ǰ���յ����ݣ�Ȼ����������posֵ
//    pos = um->pos;
//    //len���������ݿռ��������ݵ��ܳ���
//    //pos����һ��ȡ�����ݺ��ָ��λ��
//    //���б������ݳ��Ⱦ���len-pos
//    if (DataPoolWrite(um->rxDataPool, um->dma_rbuff + pos, len - pos)) { 
//      um->pos += (len - pos);
//      if( um->pos == um->dma_rbuff_size ) 
//        um->pos = 0;
//      return true;
//    }
//  }
//  
//  return false;
//}