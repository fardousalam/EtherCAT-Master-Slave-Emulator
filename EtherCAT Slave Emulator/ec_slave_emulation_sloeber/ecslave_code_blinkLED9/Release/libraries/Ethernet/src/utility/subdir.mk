################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src\utility\w5100.cpp 

LINK_OBJ += \
.\libraries\Ethernet\src\utility\w5100.cpp.o 

CPP_DEPS += \
.\libraries\Ethernet\src\utility\w5100.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\Ethernet\src\utility\w5100.cpp.o: C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src\utility\w5100.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino5/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\cores\arduino" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\variants\standard" -I"C:\Users\FAL\Documents\Arduino\libraries\SoftPWM" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\libraries\SPI\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0\utility" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '


