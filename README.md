# EV-ESP32-Workspace  
* EV-ESP32-Workspace 开发平台可以用于开发开关，调光灯等设备
* EV-ESP32-Workspace 开发平台也可以用于开发网关，目前提供zigbee网关的例程，例程中实现遵寻HA标准的开关、窗帘、传感器等设备。  

## ESP32 平台开发包包含：  
* xtensa-esp32-elf开发工具链。  
* esp-idf SDK  

# 使用方法  
* 在ubuntu中打开终端依次执行以下命令
* sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing
* git clone https://github.com/bingoiot/EV-ESP32-Workspace.git  
* 大陆可以用git clone https://gitee.com/bingoiot/EV-ESP32-Workspace.git
* cd EV-ESP32-Workspace/

* [网络不好时解压esp32_package/下的esp-idf，执行下面5步]
* cd esp32_package
* cat x*>>esp-idf.tar.gz
* tar -xvf esp-idf.tar.gz
* mv esp-idf ../
* cd .. 

* [网络正常执行以下2个命令]
* git submodule init esp-idf
* git submodule update 

* source SetEnvironment.sh  
* cd app/gwzb-esp32-v2.0/  
* make clean  
* make menuconfig:  
![image](https://github.com/bingoiot/EV-ESP32-Workspace/blob/master/img/1.png)  
![image](https://github.com/bingoiot/EV-ESP32-Workspace/blob/master/img/2.png)  
![image](https://github.com/bingoiot/EV-ESP32-Workspace/blob/master/img/3.png)  
![image](https://github.com/bingoiot/EV-ESP32-Workspace/blob/master/img/4.png)  
![image](https://github.com/bingoiot/EV-ESP32-Workspace/blob/master/img/5.png)  
* Save & Exit
* make -j5  
* make flash  
* make monitor 接收显示串口调试信息，ctrl + ] 关闭调试信息


