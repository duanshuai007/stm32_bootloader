#include "bootload.h"
#include "stdint.h"
#include "stm32f1xx.h"
#include "user_config.h"
#include "stdlib.h"
#include "string.h"
#include "crc16.h"
#include "flash.h"
#include "datapool.h"

#define UART2_DMA_RECV_SIZE       64

static uint8_t uart2_dma_recv_buffer[UART2_DMA_RECV_SIZE] = {0};
static UartModule gUartMx;
static FirmWare gFW;
uint8_t *recv = NULL;

UartModule *GetBootUartModule(void)
{
  return &gUartMx;
}

/*
*   ��ʼ����F405ͨ�ŵĴ�����ؽṹ��
*/
void BootLoad_UartInit(UART_HandleTypeDef *huart)
{
  UartModule *um = &gUartMx;
  FirmWare *fw = &gFW;
  
  um->uart = huart;
  um->dma_rbuff_size = UART2_DMA_RECV_SIZE;
  um->dma_rbuff = uart2_dma_recv_buffer;
  
  fw->u32FWSize = 0;
  fw->u8NumOfPacket = 0;
  fw->u8Version = 0;
  memset(fw->u8MD5, 0, MD5_LEN);
}

uint8_t CheckUpdata(uint8_t ver)
{
  UartModule *um = GetBootUartModule();
  FirmWare *fw = &gFW;
  //����Ƿ��и���,��������Ĺ̼��汾��Ϣ
  RespUpdate ru;
  DeviceReqUpData dru;
  
  dru.u8Head = 0xa5;
  dru.u8Cmd = 0x01;
  dru.u8Version = ver;
  dru.u16CRC = CRC16_IBM((uint8_t *)&dru, 3);
  dru.u8Tail = 0x5a;
  
  HAL_UART_Transmit(um->uart, (uint8_t *)&dru, sizeof(DeviceReqUpData), 100);
  if (HAL_UART_Receive(um->uart, (uint8_t *)&ru, sizeof(RespUpdate), 2000) == HAL_OK) {
    //�Խ��յ������ݽ��д���
    uint16_t crc = CRC16_IBM((uint8_t *)&ru, 9);
    if ((ru.u8Head == 0xa5) && (ru.u8Tail == 0x5a)) {
      if ( crc == ru.u16CRC ) {
        if ( ru.u8Cmd == 0x01 ) {

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
  UartModule *um = GetBootUartModule();
  FirmWare *fw = &gFW;
  uint32_t size = 0;
  uint8_t curPacketNum = 0;
  
  if (recv == NULL) {
    recv = (uint8_t *)malloc(RECV_SIZE);
    if(recv == NULL)
      return false;
  }
  
  DeviceReqFW drf;
  RespFW *rf = (RespFW *)recv;
  
  //��ʼ���ع̼�
  while(1) {
    drf.u8Head = 0xa5;
    drf.u8Cmd = 0x02;
    drf.u8PacketNum = curPacketNum;
    drf.u16CRC = CRC16_IBM((uint8_t *)&drf, 3);
    drf.u8Tail = 0x5a;
    HAL_UART_Transmit(um->uart, (uint8_t *)&drf, sizeof(DeviceReqFW), 100);
    
    memset(recv, 0, RECV_SIZE);
    
    HAL_UART_Receive(um->uart, recv, sizeof(RespFW), 2000);
    
    //�Խ��յ������ݽ��д���
    //��Ҫ�������˶����һ�����������⴦�������Ч�ַ�����֤���ĳ����ǹ̶���
    if ((rf->u8Head == 0xa5) && (rf->u8Tail == 0x5a)) {
      uint16_t crc = CRC16_IBM(rf->u8FW, rf->u32PacketSize);
      if ( crc == rf->u16CRC ) {
        if (rf->u8Cmd == 0x02) {
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
    
    HAL_Delay(500);  //debug
  }
  
  //�����յ��İ��Ĵ�С
  if (size == fw->u32FWSize)
    return true;
  else
    return false;
}
