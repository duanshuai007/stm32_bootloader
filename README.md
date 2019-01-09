##BOOTLOAD项目介绍

###1.f103bootload文件夹
```
该项目是一个基于stm32f103c8t6的bootloader程序。
其中f103bootload是stm32f103c8t6的工程，在该工程中实现了bootloader的简单功能。
通过串口发送数据向服务器发送更新请求，服务器通过对比请求中的版本号与最新的版本号决定是否使能更新应答。
发送更新应答后，103解析接收到的数据，如果使能更新，则会发起获取固件的请求，对固件的获取是按照每包1024字
节固件内容进行获取的，获取到固件后计算crc，比对成功后则写入到flash中，同时通过size进行接收到包大小的计
算，在接受完成后将size与固件大小做对比，如果相同，则启动app。否则就一直执行发送更新请求申请固件的操作。

在IAR工程中右键项目打开option->Linker，选中Override Default，点击Edit，修改Memory Regions中Rom的Start
为0x08000000，End修改为0x08004000,Stack/Heap Sizes可以根据程序需要适当修改。
为了尽量少占用空间，便以优化选择了最高，同时lib库选择了normal。所以无法使用printf。
```

####数据格式

#####1.更新请求

|Head|Cmd|Version|CRC|Tail|
|:-:|:-:|:-:|:-:|:-:|
|1 Byte|1 Byte|1 Byte|2 Byte|1 Byte|
|0xA5|1|version|CRC16_IBM|0x5A|

#####服务器的应答信息格式

|Head|Cmd|Update|Version|TotalPacketNum|TotalSize|CRC|Tail|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|1 Byte|1 Byte|1 Byte|1 Byte|1 Byte|4 Byte|2 Byte|1 Byte|
|0xA5|1|1/0|version|num|size| CRC16_IBM|0x5A|

#####update:1表示允许升级，0表示不允许升级
#####version:十六进制数据,例如版本2.3 则version等于0x23
#####num:固件包按照1024字节能够拆分成的包的数量
#####size:固件包的总大小

#####2.获取固件

|Head|Cmd|PacketNum|CRC|Tail|
|:-:|:-:|:-:|:-:|:-:|
|1 Byte|1 Byte|1 Byte|2 Byte|1 Byte|
|0xA5|2|num|CRC16_IBM|0x5A|

#####服务器的应答信息格式

|Head|Cmd|PacketNO|PacketSize|CRC|Tail|
|:-:|:-:|:-:|:-:|:-:|:-:|
|1 Byte|1 Byte|1 Byte|4 Byte|2 Byte |1 Byte|
|0xA5|2| number |size| CRC16_IBM|0x5A|

#####PacketNum:对应的是固件的第几个包
#####PacketNO:服务器返回的是第几个固件包
#####PacketSize:本次返回的固件包的大小

`需要注意的是，为了使bootload程序尽量简化，这里每一个包的总长度是固定的。所以即使PacketSize并不等
于1024，但是整个包的长度也仍然是固定的长度，所以使用0xaa对空白处进行填充。`
·

```
在本工程中将固件版本信息保存在Flash内的0x08003C00位置处。
本工程占用FLASH空间0x08000000-0x08004000之间的空间。
APP需要特殊设置中断向量表的地址，除了在IAR中重设以外，还需要在system_stm32f1xx.c中修改宏
VECT_TAB_OFFSET的值，在本项目中不需要修改，
在APP项目中将该值修改为项目Bin文件在FLASH内的起始地址的偏移值，也就是0x00004000
```

###2.F405boot

```
该工程是服务器端工程，根据协议要求与f103进行通信，将固件信息和内容发送给f103.
```

###3.F103bootloadapp/server

```
这两个工程都是满足要求能够正常运行的工程

在IAR工程中右键项目打开option->Linker，选中Override Default，点击Edit，修改Vector Table(中断向量
        表)为0x08004000(需要跟芯片的FLASH相匹配，还需要与bootloader程序匹配)，修改Memory Regions中
Rom的Start为0x08004000，End保持原值不用改动。Stack/Heap Sizes可以根据程序需要适当修改。

在测试中因为F405内还不能通过下载方式获取固件，所以是直接将固件内容烧入到F405的0x08004000 FLASH地址
空间的。然后在根据固件的大小修改udate.h中宏F103_FW_SIZE的值。
```

###测试方式

```
1.打开SEGGER J-Flash V4.90，将编译好的固件烧入到F405 FLASH空间的0x08004000处。
2.打开F405工程，修改update.h中的F103_FW_SIZE为固件的实际大小,单位:字节,使用IAR工程调试模式直接烧
入工程。
3.第三步主要是为了确认一下F103是否能够正常工作。打开F103bootload工程，联结调试，在调试模式下可以
观察想要观察的信息。（主要是因为我的板子上没有灯，也没有调试串口所以只能通过这种方式来看效果。使
用虚拟串口能够看到printf的信息，这只能在f103bootloadapp中能够看到。）
```


