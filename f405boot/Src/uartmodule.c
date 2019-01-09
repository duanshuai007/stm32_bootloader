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

uint8_t FWVersion = 0x10;

void UartModuleDataProcess(void)
{
  UartModule *um = &gUartMX;
  uint8_t buffer[12] = {0};
  
  if (DataPoolGetNumByte(um->rxDataPool, buffer, 6)) {
    if (buffer[1] == 0x01) {
      //����������
      DeviceReqUpData *dru = (DeviceReqUpData *)buffer;
      //a5 01 version crch crcl 5a
      if ((dru->u8Head == 0xa5) && (dru->u8Tail == 0x5a)) {
        if ( dru->u8Cmd == 0x01 ) {
          uint16_t crc = CRC16_IBM(buffer, 3);
          if (dru->u16CRC == crc) {
            RespUpdate ru;
            if (dru->u8Version < FWVersion) {
              //���͸���ָ�� 
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
      //�л�ȡ������������
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