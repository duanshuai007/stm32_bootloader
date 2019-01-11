#ifndef _BOOTLOAD_103_H_
#define _BOOTLOAD_103_H_

#include "stdint.h"
#include "user_config.h"
#include "flash.h"

//声明一个无参数无返回值的函数指针
typedef void (* pFunction)(void);

#define PACKET_HEAD   0xa5
#define PACKET_TAIL   0x5a
#define REQ_UPDATE    0x01
#define REQ_GETFW     0x02

#pragma pack(1)
typedef struct {
  uint8_t u8Version;
  uint8_t u8NumOfPacket;  //一共多少个包
//  uint8_t u8MD5[MD5_LEN];
  uint32_t u32FWSize;     //整体的大小
} FirmWare;

//向服务器发送的查询是否有升级固件的命令
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

//从服务器接收到的升级固件信息的命令格式
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

//接收到服务器发送来的固件内容
typedef struct {
  uint8_t u8Head;
  uint8_t u8Cmd;
  uint8_t u8CurNum;         //当前包号
  uint32_t u32PacketSize;   //当前包大小
  uint8_t u8FW[SECTOR_SIZE];       //包最大1024
  uint16_t u16CRC;
  uint8_t u8Tail;
} RespFW;

#pragma pack()

/*
*   初始化与F405通信的串口相关结构体
*/
void BootLoad_UartInit(UART_HandleTypeDef *huart);

/*
*     向服务器问询是否有更新版本
*     返回值：0 服务器没有应答
*             1 有更新版本
*             2 无更新版本
*/
uint8_t CheckUpdata(uint8_t ver);

/*
*   向服务器下载固件
*   返回值:成功返回true，失败返回false
*/
bool GetFileBin(void);

/*
*   跳转到app处执行
*/
void iap_load_app(void);

/*
*   获取固件版本信息
*/
uint8_t getFWVersion(void);

#endif



