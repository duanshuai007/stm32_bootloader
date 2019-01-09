#include "uartmodule.h"
#include "stdlib.h"
#include "stdint.h"
#include "stm32f4xx.h"
#include "user_config.h"
#include "datapool.h"
#include "crc16.h"
#include "string.h"
#include "update.h"

#define DMA_RECV_BUFFER_SIZE  64

UartModule gUartMX;
static uint8_t dma_rbuffer[DMA_RECV_BUFFER_SIZE];

UartModule *GetUartModule(void)
{
  return &gUartMX;
}

UartModule *UartModuleInit(UART_HandleTypeDef *huart)
{
  gUartMX.dma_rbuff = dma_rbuffer;
  gUartMX.dma_rbuff_size = DMA_RECV_BUFFER_SIZE;
  gUartMX.pos = 0;
  gUartMX.uart = huart;
  gUartMX.rxDataPool = DataPoolInit(128);
  gUartMX.txDataPool = NULL;
  
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
//  __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  HAL_UART_Receive_DMA(huart, dma_rbuffer, DMA_RECV_BUFFER_SIZE);
  
  return &gUartMX;
}

bool CopyDataFromDMAHandler(UART_HandleTypeDef *huart)
{
  uint32_t len;
  uint32_t pos;
  UartModule *um = GetUartModule();
  
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

uint8_t FWVersion = 0x10;

void UartModuleDataProcess(void)
{
  UartModule *um = &gUartMX;
  uint8_t buffer[12] = {0};
  
  if (DataPoolGetNumByte(um->rxDataPool, buffer, 6)) {
    if (buffer[1] == 0x01) {
      //有升级请求
      DeviceReqUpData *dru = (DeviceReqUpData *)buffer;
      //a5 01 version crch crcl 5a
      if ((dru->u8Head == 0xa5) && (dru->u8Tail == 0x5a)) {
        if ( dru->u8Cmd == 0x01 ) {
          uint16_t crc = CRC16_IBM(buffer, 3);
          if (dru->u16CRC == crc) {
            RespUpdate ru;
            if (dru->u8Version < FWVersion) {
              //发送更新指令 
              ru.u8Update = 1;
            } else {
              ru.u8Update = 0;
            }
            ru.u8Head = 0xa5;
            ru.u8Cmd = 0x01;
            ru.u8Version = FWVersion;
            ru.u8TotalNumOfPacket = getFWPacketNum();
            ru.u32TotalSize = getFWTotalSize();
            ru.u16CRC = CRC16_IBM((uint8_t *)&ru, 9);
            ru.u8Tail = 0x5a;
            HAL_UART_Transmit(um->uart, (uint8_t *)&ru, sizeof(RespUpdate), 100);
          }
        }
      }
    } else if (buffer[1] == 0x02) {
      //有获取升级包的请求
      DeviceReqFW *drf = (DeviceReqFW *)buffer;
      uint32_t len;
      
      if((drf->u8Head == 0xa5) && (drf->u8Tail == 0x5a)) {
        uint16_t crc = CRC16_IBM(buffer, 3);
        if(drf->u16CRC == crc) {
          if (drf->u8Cmd == 0x02 ) {
            
            if (drf->u8PacketNum + 1 < getFWPacketNum()) {
              len = 1024;
            } else {
              len = getFWLastPacketSize();
            }

            SendFWToF103(drf->u8PacketNum, len);
          }
        }
      }
    }
  }
}