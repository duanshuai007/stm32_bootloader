#include "update.h"
#include "stdlib.h"
#include "user_config.h"
#include "uartmodule.h"
#include "crc16.h"

uint8_t getFWPacketNum(void)
{
  if (F103_FW_SIZE%1024 == 0)
    return F103_FW_SIZE/1024;
  else
    return F103_FW_SIZE/1024 + 1;
}

uint32_t getFWTotalSize(void)
{
  return F103_FW_SIZE;
}

uint32_t getFWLastPacketSize(void)
{
  return F103_FW_SIZE%1024;
}

#define READ_FLASH_BYTE(addr) (*(volatile uint8_t *)(addr))

extern UartModule gUartMX;

/*
//���յ��������������Ĺ̼�����
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8CurNum;         //��ǰ����
  uint32_t u32PacketSize;   //��ǰ����С
  uint8_t u8FW[SECTOR_SIZE];       //�����1024
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespFW;
*/
void SendFWToF103(uint8_t packetnum, uint32_t len)
{
  UartModule *um = GetUartModule();

  uint8_t *ptr = NULL;
  ptr = (uint8_t *)(APPLICATION_ADDR + (packetnum * 1024));
  
  RespFW rf;
  rf.u8Head = 0xa5;
  rf.u8Cmd = 0x02;
  rf.u8CurNum = packetnum;
  rf.u32PacketSize = len;
  rf.u16CRC = CRC16_IBM(ptr, len);
  rf.u8Tail = 0x5a;
  HAL_UART_Transmit(um->uart, (uint8_t *)&rf, 7, 100);
  HAL_UART_Transmit(um->uart, ptr, len, 2000);
  //Ϊ��ʹbootload�д��뾡���٣��������������䣬
  //��֤ÿ�����Ĵ�Сһ��(���������һ����)
  if(len < 1024) {
    uint32_t i;
    uint8_t dummy = 0xaa;
    for(i=0; i < (1024 - len); i++) {
      HAL_UART_Transmit(um->uart, &dummy, 1, 5);
    }
  }
  HAL_UART_Transmit(um->uart, (uint8_t *)&rf.u16CRC, 3, 100);
}
