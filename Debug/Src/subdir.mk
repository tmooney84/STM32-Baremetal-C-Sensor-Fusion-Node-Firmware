################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc_dma.c \
../Src/adxl345.c \
../Src/i2c.c \
../Src/iwdg.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/tim.c \
../Src/uart_dma.c 

OBJS += \
./Src/adc_dma.o \
./Src/adxl345.o \
./Src/i2c.o \
./Src/iwdg.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/tim.o \
./Src/uart_dma.o 

C_DEPS += \
./Src/adc_dma.d \
./Src/adxl345.d \
./Src/i2c.d \
./Src/iwdg.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/tim.d \
./Src/uart_dma.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -DSTM32F411xE -c -I../Inc -I"/home/hanna/STM32CubeIDE/workspace_1.13.0/UART_DMA/chip_headers/CMSIS/Include" -I"/home/hanna/STM32CubeIDE/workspace_1.13.0/UART_DMA/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/adc_dma.cyclo ./Src/adc_dma.d ./Src/adc_dma.o ./Src/adc_dma.su ./Src/adxl345.cyclo ./Src/adxl345.d ./Src/adxl345.o ./Src/adxl345.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/iwdg.cyclo ./Src/iwdg.d ./Src/iwdg.o ./Src/iwdg.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/tim.cyclo ./Src/tim.d ./Src/tim.o ./Src/tim.su ./Src/uart_dma.cyclo ./Src/uart_dma.d ./Src/uart_dma.o ./Src/uart_dma.su

.PHONY: clean-Src

