################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BUZZER.c \
../DCMotor.c \
../Final_Project_Control_ECU.c \
../PWM.c \
../TIMER1.c \
../USART.c \
../external_eeprom.c \
../gpio.c \
../twi.c 

OBJS += \
./BUZZER.o \
./DCMotor.o \
./Final_Project_Control_ECU.o \
./PWM.o \
./TIMER1.o \
./USART.o \
./external_eeprom.o \
./gpio.o \
./twi.o 

C_DEPS += \
./BUZZER.d \
./DCMotor.d \
./Final_Project_Control_ECU.d \
./PWM.d \
./TIMER1.d \
./USART.d \
./external_eeprom.d \
./gpio.d \
./twi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


