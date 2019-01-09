#include "datacopy.h"
#include "stdint.h"
#include "stm32f1xx.h"
#include "user_config.h"
#include "stdlib.h"
#include "bootload.h"
#include "datapool.h"

bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart)
{
  uint32_t len;
  uint32_t pos;
  UartModule *um = GetBootUartModule();
  
  //len �����Ѿ����ڴ�����ݵĳ���(�ռ�)
  len = (um->dma_rbuff_size - (uint16_t)__HAL_DMA_GET_COUNTER(um->uart->hdmarx));
  
  //��dma����������ȡ�����뵽���ݴ�����ڴ���
  //��len��pos��ȡ������,��Ϊ�������
  //1��len=pos=0 û������
  //2��len > pos ���Գ�ȥ��pos��len������
  //3��len < pos ˵�������˴��ڽ�������жϣ��ڸû����������ݴ����ˣ���Ҫȡ��һ��������������
  //4: len = 0, pos > 0 �������ݽ�������жϣ���Ҫȡ����ǰdma�ڵĴ�pos��������������
  
  if (((0 == len) && (0 == um->pos)) || (len == um->pos))
    return true;
  
  if (( 0 == len ) && ( um->pos > 0)) {
    //���ݽ�������жϣ�������д��pool�к󣬽�pos���á�
    pos = um->pos;

    if (DataPoolWrite(um->rxDataPool, um->dma_rbuff + pos, um->dma_rbuff_size - pos )) 
    {
      um->pos = 0;
      return true;
    }
  } else  if (len > um->pos) {
    //�����жϣ�ȡ����ǰ���յ����ݣ�Ȼ����������posֵ
    pos = um->pos;
    //len���������ݿռ��������ݵ��ܳ���
    //pos����һ��ȡ�����ݺ��ָ��λ��
    //���б������ݳ��Ⱦ���len-pos
    if (DataPoolWrite(um->rxDataPool, um->dma_rbuff + pos, len - pos)) 
    { 
      um->pos += (len - pos);
      if( um->pos == um->dma_rbuff_size ) 
        um->pos = 0;
      return true;
    }
  }
  
  return false;
}