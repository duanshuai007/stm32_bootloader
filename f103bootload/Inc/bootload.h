#ifndef _BOOTLOAD_103_H_
#define _BOOTLOAD_103_H_

#include "stdint.h"
#include "user_config.h"
#include "flash.h"

//����һ���޲����޷���ֵ�ĺ���ָ��
typedef void (* pFunction)(void);

#define PACKET_HEAD   0xa5
#define PACKET_TAIL   0x5a
#define REQ_UPDATE    0x01
#define REQ_GETFW     0x02

#pragma pack(1)
typedef struct {
  uint8_t u8Version;
  uint8_t u8NumOfPacket;  //һ�����ٸ���
//  uint8_t u8MD5[MD5_LEN];
  uint32_t u32FWSize;     //����Ĵ�С
} FirmWare;

//����������͵Ĳ�ѯ�Ƿ��������̼�������
typedef struct {
  uint8_t   u8Head;
  uint8_t   u8Cmd;
  union {
    uint8_t u8Version;
    uint8_t u8PacketNum;
  }Message;
  uint16_t  u16CRC;
  uint8_t   u8Tail;
} DeviceReq;

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

/*
*   ����������ع̼�
*   ����ֵ:�ɹ�����true��ʧ�ܷ���false
*/
bool GetFileBin(void);

/*
*   ��ת��app��ִ��
*/
void iap_load_app(void);

/*
*   ��ȡ�̼��汾��Ϣ
*/
uint8_t getFWVersion(void);

#endif



