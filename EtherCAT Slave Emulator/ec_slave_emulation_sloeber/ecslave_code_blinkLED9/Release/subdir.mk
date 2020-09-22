################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
..\ec_util.cpp \
..\ec_w5100.cpp \
..\sloeber.ino.cpp 

C_SRCS += \
..\ec_sii.c 

C_DEPS += \
.\ec_sii.c.d 

LINK_OBJ += \
.\ec_sii.c.o \
.\ec_util.cpp.o \
.\ec_w5100.cpp.o \
.\sloeber.ino.cpp.o 

CPP_DEPS += \
.\ec_util.cpp.d \
.\ec_w5100.cpp.d \
.\sloeber.ino.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
ec_sii.c.o: ..\ec_sii.c
	@echo 'Building file: $<'
	@echo 'Starting C compile'
	"C:\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino5/bin/avr-gcc" -c -g -Os -Wall -Wextra -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\cores\arduino" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\variants\standard" -I"C:\Sloeber\arduinoPlugin\libraries\SoftPWM\1.0.1" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\libraries\SPI\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0\utility" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0" -I"C:\Sloeber\arduinoPlugin\libraries\LibPrintf\1.0.0\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

ec_util.cpp.o: ..\ec_util.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino5/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\cores\arduino" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\variants\standard" -I"C:\Sloeber\arduinoPlugin\libraries\SoftPWM\1.0.1" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\libraries\SPI\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0\utility" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0" -I"C:\Sloeber\arduinoPlugin\libraries\LibPrintf\1.0.0\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

ec_w5100.cpp.o: ..\ec_w5100.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino5/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\cores\arduino" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\variants\standard" -I"C:\Sloeber\arduinoPlugin\libraries\SoftPWM\1.0.1" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\libraries\SPI\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0\utility" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0" -I"C:\Sloeber\arduinoPlugin\libraries\LibPrintf\1.0.0\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

sloeber.ino.cpp.o: ..\sloeber.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Sloeber\arduinoPlugin\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino5/bin/avr-g++" -c -g -Os -Wall -Wextra -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR     -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\cores\arduino" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\variants\standard" -I"C:\Sloeber\arduinoPlugin\libraries\SoftPWM\1.0.1" -I"C:\Sloeber\arduinoPlugin\packages\arduino\hardware\avr\1.8.2\libraries\SPI\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet\2.0.0\src" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0\utility" -I"C:\Sloeber\arduinoPlugin\libraries\Ethernet_Shield_W5200\1.0.0" -I"C:\Sloeber\arduinoPlugin\libraries\LibPrintf\1.0.0\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -D__IN_ECLIPSE__=1 -x c++ "$<"   -o "$@"

	@echo 'Finished building: $<'
	@echo ' '


