# EV-ESP32-Workspace  
* EV-ESP32-Workspace ����ƽ̨�������ڿ������أ�����Ƶ��豸
* EV-ESP32-Workspace ����ƽ̨Ҳ�������ڿ������أ�Ŀǰ�ṩzigbee���ص����̣�������ʵ����ѰHA��׼�Ŀ��ء����������������豸��  

## ESP32 ƽ̨������������  
* xtensa-esp32-elf������������  
* esp-idf SDK  

# ʹ�÷���  
* ��ubuntu�д��ն�����ִ����������
* sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing
* git clone https://github.com/bingoiot/EV-ESP32-Workspace.git  
* ��½������git clone https://gitee.com/bingoiot/EV-ESP32-Workspace.git
* cd EV-ESP32-Workspace/

* [���粻��ʱ��ѹesp32_package/�µ�esp-idf��ִ������5��]
* cd esp32_package
* cat x*>>esp-idf.tar.gz
* tar -xvf esp-idf.tar.gz
* mv esp-idf ../
* cd .. 

* [��������ִ������2������]
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
* make monitor ������ʾ���ڵ�����Ϣ��ctrl + ] �رյ�����Ϣ


