#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include "stdint.h"
#include "stm32f4xx.h"

typedef enum {
  false,
  true,
}bool;

#define MD5_LEN 32
#define SECTOR_SIZE 1024

#define READ_FLASH_BYTE(addr) (*(volatile uint8_t  *)(addr))
#define READ_FLASH_WORD(addr) (*(volatile uint32_t *)(addr))

#pragma pack(1)
typedef struct {
  uint8_t u8Version;
  uint8_t u8NumOfPacket;  //һ�����ٸ���
  uint8_t u8MD5[MD5_LEN];
  uint32_t u32FWSize;     //����Ĵ�С
} FirmWare;

//����������͵Ĳ�ѯ�Ƿ��������̼�������
typedef struct {
  uint8_t   u8Head;
  uint8_t   u8Cmd;
  uint8_t   u8Version;
  uint16_t  u16CRC;
  uint8_t   u8Tail;
} DeviceReqUpData;

//�ӷ��������յ��������̼���Ϣ�������ʽ
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8Update;
  uint8_t u8Version;
  uint8_t u8TotalNumOfPacket;
  uint32_t u32TotalSize;
//  uint8_t u8MD5[MD5_LEN];
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespUpdate;

//�ӷ�������ȡ�̼�����
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8PacketNum;
  uint16_t u16CRC;
  uint8_t u8Tail;
} DeviceReqFW;

//���յ��������������Ĺ̼�����
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8CurNum;         //��ǰ����
  uint32_t u32PacketSize;   //��ǰ����С
//  uint8_t u8FW[SECTOR_SIZE];       //�����1024
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespFW;
#pragma pack()

#if 0

#define INFO_DBG  0
#define WARN_DBG  1
#define ERROR_DBG 1

#define DEBUG_INFO(format, ...)   do{if(INFO_DBG){printf("[INFO]:"format, ##__VA_ARGS__);}}while(0)
#define DEBUG_WARN(format, ...)   do{if(WARN_DBG){printf("[WARN][%s]:"format, __FUNCTION__ , ##__VA_ARGS__);}}while(0)
#define DEBUG_ERROR(format, ...)  do{if(ERROR_DBG){printf("[ERROR][%s]:"format, __FUNCTION__ , ##__VA_ARGS__);}}while(0)
#else

#define DEBUG_INFO(format, ...) 
#define DEBUG_WARN(format, ...)
#define DEBUG_ERROR(format, ...)
#endif


#define APPLICATION_ADDR              0x08004000


#endif
