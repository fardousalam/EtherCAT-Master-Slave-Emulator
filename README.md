# EtherCAT Master Slave Emulator

The goal of this project is to execute the EtherCAT protocol stack software on Raspberry pi and Arduino to identify different test cases. The test cases are:

a) Diagnosis/analysis

b) Prototyping

c) Simulating multiple device

Project outline:

There are various steps to be taken to develop an emulator for analyzing and prototyping issues.Before stepping into real tasks proper working environment must be created. For this reason, a real time kernel system is to be developed in a Raspberry pi. After installing Raspbian operating system in Raspberry pi, a real time patch should be installed and configured in Raspberry pi. After that EtherCAT master will be implemented. IgH has developed an EtherCAT master
software which is free of charge. So, this master software is implemented on a test system. The test system is a turn table which contains different types of slaves. To simulate these slave devices a “c” code is written. All of the necessary configurations of slaves are done in that code. Slaves are connected with a Raspberry pi (master) via EtherCAT. To develop an EtherCAT slave emulator a Raspberry pi is used here along with free software named as “light ethercat slave”. This software includes a slave stack. By using this stack, it is possible to simulate many virtual slaves in a Raspberry pi real time operating system. Arduino is a part of this emulator. Arduino slave is connected with different peripherals. This Arduino slave is used as prototype to check whether different types of peripherals like analog input, analog output, digital input or digital output can be interfaced properly or not. 

![block 1](https://user-images.githubusercontent.com/48807609/95467301-9331ab80-097d-11eb-8ba1-ac4a41c799e6.PNG)

The turn table machine is shown in below.

![Turn tisch](https://user-images.githubusercontent.com/48807609/95468115-79dd2f00-097e-11eb-8a3a-d26108aad2ec.PNG)

The projects can be divided into two major parts. One is EtherCAT Master Emulator and another is EtherCAT Slave emulator. 

The master Emulator includes Raspberry Pi, IgH EtherCAT Master Software, Turn Table machine which contains Etercat Slave devices. The main task for the master emulater is to emulating EtherCAT Master under a Real time linux system(raspberry Pi real time system) to communicate with the slave devices via EtherCAT. There are four slave devices in the Turn table machine. One is Beckhoff Ethercat Analog device , one is Beckhoff EtherCAT Digital device and two faulhaber motor devices. Here Analog device and Degital devices are configured. The analog slave device is connected to an analog sensor which measures the distance betwenn the sensor and a object(white box as a object in the picture). The degital slave device is connected to a digital sensor where the digital sensor is used to positioning the rotating plate upon which the object lies and moves forward and reverse with the help of conveyer belt. 

As the slave devices are used to collect the real time data which updates automatically over time, we need an real time environment setup before implenting the project. For this reason, a real time patch is installed at first in the raspberry pi. After succesfull compilation of real time patch IgH EtherCAT Software is installed and compilled. Now the configuration can be started. The configuration is done by writing a c program. The "C" programe is written from user space and kernel space both. The Configuration file contains all the configurable variables and functions to configure the slave devics. After succesful configuration the slave devices sent real time data to the master and the developer can see the data in the PC consol.

The slave emulator includes raspberry pi, Light EtherCAT Software, Arduino with peripherals. Just like as the Master emulator the slave emulator also needs real time environment. So first real time patch is installed. Then some reorganization of Light EtherCAT Software was done to adjust with the project. To emulate in the Slave platform some programs are written in C and C++ langauge. This files are basically etherCAT protocol files and configuration files for the slave devices.  








