################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Mylib/Button.c 

OBJS += \
./Mylib/Button.o 

C_DEPS += \
./Mylib/Button.d 


# Each subdirectory must supply rules for building sources it contributes
Mylib/%.o Mylib/%.su Mylib/%.cyclo: ../Mylib/%.c Mylib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32/Button/Mylib" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Mylib

clean-Mylib:
	-$(RM) ./Mylib/Button.cyclo ./Mylib/Button.d ./Mylib/Button.o ./Mylib/Button.su

.PHONY: clean-Mylib

