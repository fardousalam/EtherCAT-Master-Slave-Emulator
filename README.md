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

