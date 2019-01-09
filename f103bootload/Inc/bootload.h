#ifndef _BOOTLOAD_103_H_
#define _BOOTLOAD_103_H_

#include "stdint.h"
#include "user_config.h"
#include "flash.h"

typedef void (* pFunction)(void);

#define VERSION_LEN   4
#define MD5_LEN       32

typedef struct {
  volatile uint32_t pos;
  UART_HandleTypeDef  *uart;    //����
  uint16_t dma_rbuff_size;      //dma���ջ������Ĵ�С
  uint8_t *dma_rbuff;           //ָ��dma���ջ�����
} UartModule;

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
  uint8_t u8FW[SECTOR_SIZE];       //�����1024
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespFW;

typedef struct 
{
  uint8_t ucBootType;
  uint8_t ucTryTimes;
  uint8_t ucBootSeccessFlg;
  uint8_t ucRev[1];
  
  uint8_t ucFWVer[VERSION_LEN];
  uint8_t ucNewFWVer[VERSION_LEN];
  
}T_BootConfig;

#pragma pack()

/*
*   ��ʼ����F405ͨ�ŵĴ�����ؽṹ��
*/
void BootLoad_UartInit(UART_HandleTypeDef *huart);

/*
*     ���������ѯ�Ƿ��и��°汾
*     ����ֵ��0 ������û��Ӧ��
*             1 �и��°汾
*             2 �޸��°汾
*/
uint8_t CheckUpdata(uint8_t ver);

bool GetFileBin(void);

void iap_load_app(void);

uint8_t getFWVersion(void);

#endif



