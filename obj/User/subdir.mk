################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/HW_I2C.c \
../User/PCF8574.c \
../User/_main.c \
../User/ch32v00x_it.c \
../User/dht11.c \
../User/lcd1602_i2c.c \
../User/main.c \
../User/system_ch32v00x.c 

C_DEPS += \
./User/HW_I2C.d \
./User/PCF8574.d \
./User/_main.d \
./User/ch32v00x_it.d \
./User/dht11.d \
./User/lcd1602_i2c.d \
./User/main.d \
./User/system_ch32v00x.d 

OBJS += \
./User/HW_I2C.o \
./User/PCF8574.o \
./User/_main.o \
./User/ch32v00x_it.o \
./User/dht11.o \
./User/lcd1602_i2c.o \
./User/main.o \
./User/system_ch32v00x.o 


EXPANDS += \
./User/HW_I2C.c.234r.expand \
./User/PCF8574.c.234r.expand \
./User/_main.c.234r.expand \
./User/ch32v00x_it.c.234r.expand \
./User/dht11.c.234r.expand \
./User/lcd1602_i2c.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v00x.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32ecxw -mabi=ilp32e -msmall-data-limit=0 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"/Users/gmmdtnt/Library/Mobile Documents/com~apple~CloudDocs/Documents/Microcontrollers/CH32_Home_Meteo_Station/Debug" -I"/Users/gmmdtnt/Library/Mobile Documents/com~apple~CloudDocs/Documents/Microcontrollers/CH32_Home_Meteo_Station/Core" -I"/Users/gmmdtnt/Library/Mobile Documents/com~apple~CloudDocs/Documents/Microcontrollers/CH32_Home_Meteo_Station/User" -I"/Users/gmmdtnt/Library/Mobile Documents/com~apple~CloudDocs/Documents/Microcontrollers/CH32_Home_Meteo_Station/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

