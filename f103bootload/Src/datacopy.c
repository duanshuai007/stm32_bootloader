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
  
  //len 等于已经用于存放数据的长度(空间)
  len = (um->dma_rbuff_size - (uint16_t)__HAL_DMA_GET_COUNTER(um->uart->hdmarx));
  
  //将dma缓冲区数据取出放入到数据处理的内存内
  //用len和pos来取出数据,分为几种情况
  //1：len=pos=0 没有数据
  //2：len > pos 可以出去从pos到len的数据
  //3：len < pos 说明产生了串口接收完成中断，在该缓冲区内数据存满了，需要取下一个缓冲区内数据
  //4: len = 0, pos > 0 产生数据接收完成中断，需要取出当前dma内的从pos到最后的所有数据
  
  if (((0 == len) && (0 == um->pos)) || (len == um->pos))
    return true;
  
  if (( 0 == len ) && ( um->pos > 0)) {
    //数据接收完成中断，将数据写入pool中后，将pos重置。
    pos = um->pos;

    if (DataPoolWrite(um->rxDataPool, um->dma_rbuff + pos, um->dma_rbuff_size - pos )) 
    {
      um->pos = 0;
      return true;
    }
  } else  if (len > um->pos) {
    //空闲中断，取出当前接收的数据，然后重新设置pos值
    pos = um->pos;
    //len是整个数据空间已有数据的总长度
    //pos是上一次取完数据后的指针位置
    //所有本次数据长度就是len-pos
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