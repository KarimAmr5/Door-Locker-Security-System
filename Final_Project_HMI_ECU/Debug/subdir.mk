################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Final_Project_HMI_ECU.c \
../LCD.c \
../TIMER1.c \
../USART.c \
../gpio.c \
../keypad.c 

OBJS += \
./Final_Project_HMI_ECU.o \
./LCD.o \
./TIMER1.o \
./USART.o \
./gpio.o \
./keypad.o 

C_DEPS += \
./Final_Project_HMI_ECU.d \
./LCD.d \
./TIMER1.d \
./USART.d \
./gpio.d \
./keypad.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


