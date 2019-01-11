#include "bootload.h"
#include "stdint.h"
#include "stm32f1xx.h"
#include "user_config.h"
#include "stdlib.h"
#include "string.h"
#include "crc16.h"
#include "flash.h"
#include "datapool.h"

static FirmWare gFW;
static uint8_t *recv = NULL;
static UART_HandleTypeDef *gBootLoadUart = NULL;
/*
*   ��ʼ����F405ͨ�ŵĴ�����ؽṹ��
*/
void BootLoad_UartInit(UART_HandleTypeDef *huart)
{
  gFW.u32FWSize = 0;
  gFW.u8NumOfPacket = 0;
  gFW.u8Version = 0;
  gBootLoadUart = huart;
}

uint8_t CheckUpdata(uint8_t ver)
{
  FirmWare *fw = &gFW;
  //����Ƿ��и���,��������Ĺ̼��汾��Ϣ
  RespUpdate ru;
  DeviceReq dru;
  
  dru.u8Head = PACKET_HEAD;
  dru.u8Cmd = REQ_UPDATE;
  dru.Message.u8Version = ver;
  dru.u16CRC = CRC16_IBM((uint8_t *)&dru, sizeof(DeviceReq) - 3);
  dru.u8Tail = PACKET_TAIL;
  
  HAL_UART_Transmit(gBootLoadUart, (uint8_t *)&dru, sizeof(DeviceReq), 100);
  if (HAL_UART_Receive(gBootLoadUart, (uint8_t *)&ru, sizeof(RespUpdate), 2000) == HAL_OK) {
    //�Խ��յ������ݽ��д���
    uint16_t crc = CRC16_IBM((uint8_t *)&ru, sizeof(RespUpdate) - 3);
    if ((ru.u8Head == PACKET_HEAD) && (ru.u8Tail == PACKET_TAIL)) {
      if ( crc == ru.u16CRC ) {
        if ( ru.u8Cmd == REQ_UPDATE ) {

          fw->u32FWSize = ru.u32TotalSize;
          fw->u8NumOfPacket = ru.u8TotalNumOfPacket;
          fw->u8Version = ru.u8Version;
          
          if ( ru.u8Update ) 
            return 1;
          else 
            return 2;
        }
      }
    }
  }
  
  return 0;
}

uint8_t getFWVersion(void)
{
  return gFW.u8Version;
}

void iap_load_app(void)
{
  pFunction     Jump_To_Application;
  uint32_t      JumpAddress;
  
  //���ջ����ַ�Ƿ�Ϸ�
  if (((*(__IO uint32_t*)APPLICATION_ADDR) & 0x2FFE0000 ) == 0x20000000)
  {
    /* Jump to user application */
    //�û��������ڶ������ǳ���ʼ�ĵ�ַ(��λ��ַ)
    JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDR + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    //��ʼ��APP��ջָ��
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDR);
    //��ת��APP
    Jump_To_Application();
  }
}

bool GetFileBin(void)
{
#define RECV_SIZE 1034
  //����bin�ļ�
  FirmWare *fw = &gFW;
  uint32_t size = 0;
  uint8_t curPacketNum = 0;
  
  if (recv == NULL) {
    recv = (uint8_t *)malloc(RECV_SIZE);
    if(recv == NULL)
      return false;
  }
  
  DeviceReq drf;
  RespFW *rf = (RespFW *)recv;
  
  //��ʼ���ع̼�
  while(1) {
    drf.u8Head = PACKET_HEAD;
    drf.u8Cmd = REQ_GETFW;
    drf.Message.u8PacketNum = curPacketNum;
    drf.u16CRC = CRC16_IBM((uint8_t *)&drf, sizeof(DeviceReq) - 3);
    drf.u8Tail = PACKET_TAIL;
    HAL_UART_Transmit(gBootLoadUart, (uint8_t *)&drf, sizeof(DeviceReq), 100);
    
    memset(recv, 0, RECV_SIZE);
    
    HAL_UART_Receive(gBootLoadUart, recv, sizeof(RespFW), 2000);
    
    //�Խ��յ������ݽ��д���
    //��Ҫ�������˶����һ�����������⴦�������Ч�ַ�����֤���ĳ����ǹ̶���
    if ((rf->u8Head == PACKET_HEAD) && (rf->u8Tail == PACKET_TAIL)) {
      uint16_t crc = CRC16_IBM(rf->u8FW, rf->u32PacketSize);
      if ( crc == rf->u16CRC ) {
        if (rf->u8Cmd == REQ_GETFW) {
          if (rf->u8CurNum == curPacketNum) {
            //���յİ�������ȷ
            size += rf->u32PacketSize;
            //д�뵽flash��
            FlashWrite((APPLICATION_ADDR + (curPacketNum*SECTOR_SIZE)), rf->u8FW, rf->u32PacketSize);
            //��ȡ��һ����
            curPacketNum++;
            //�Ѿ���ȡ�����еİ�
            if ( curPacketNum == fw->u8NumOfPacket )
              break;
          }
        }
      }
    }
    
//    HAL_Delay(20);  //debug
  }
  
  //�����յ��İ��Ĵ�С
  if (size == fw->u32FWSize)
    return true;
  else
    return false;
}