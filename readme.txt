需要的调试工具：
putty:使用telnet或串口打印log，SSH端口登录，
tftpd32.exe：建立一个tftp服务器，用于母板与PC端的文件传输。


使用的TI库
simplelink_cc2640r2_sdk_2_20_00_49

编译选项C99
properties->Build->Language Options->C99

把编译后的子板bin文件放到如下目录中：
/home/elinker/bin/rf.bin
/home/elinker/bin/ble.bin

linux端常用指令：
mv
tftp -g -r xxx IPaddress
chmod


linux登录账户：root，密码：hanshow-imx6


cp2tftp.bat批处理文件：
删除tftp目录的下的bin文件，把自己目录下的文件复制到tftp目录下。


使用GRAM
http://dev.ti.com/tirex/explore/node?node=AKS5oOWR4K-9ijBgtvOWNw__krol.2c__LATEST
Using Cache as GPRAM
In CCS; Project -> Properties -> ARM Linker -> Advanced Options -> Command File Preprocessing.  Add 
GPRAM_AS_RAM=1

In CCS; Project -> Properties -> ARM Compiler-> Predefined Symbols. Add
GPRAM_AS_RAM


Using AUX RAM as GPRAM
In CCS; Project -> Properties -> ARM Linker -> Advanced Options -> Command File Preprocessing.  Add 
AUX_AS_RAM=1

Hex包头 ack
f0f0f0f00100010001004110010001000000151600003aa9
f0f0f0f00100010001004110010001000000151600027889


HEX msg
F0 F0 F0 F0 01 00 01 00 02 00 41 10 01 00 01 00 00 00 15 16 00 00 13 CB

