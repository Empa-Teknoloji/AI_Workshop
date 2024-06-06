################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/EMPA_DataCollector.c \
../Core/Src/EMPA_MqttAws.c \
../Core/Src/EMPA_Sensor.c \
../Core/Src/common.c \
../Core/Src/custom_bus.c \
../Core/Src/interrupt_handlers.c \
../Core/Src/linked_list.c \
../Core/Src/main.c \
../Core/Src/myESP32AT.c \
../Core/Src/sensirion_common.c \
../Core/Src/sensirion_i2c.c \
../Core/Src/sensirion_i2c_hal.c \
../Core/Src/sht4x_i2c.c \
../Core/Src/stm32u5xx_hal_msp.c \
../Core/Src/stm32u5xx_it.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32u5xx.c 

OBJS += \
./Core/Src/EMPA_DataCollector.o \
./Core/Src/EMPA_MqttAws.o \
./Core/Src/EMPA_Sensor.o \
./Core/Src/common.o \
./Core/Src/custom_bus.o \
./Core/Src/interrupt_handlers.o \
./Core/Src/linked_list.o \
./Core/Src/main.o \
./Core/Src/myESP32AT.o \
./Core/Src/sensirion_common.o \
./Core/Src/sensirion_i2c.o \
./Core/Src/sensirion_i2c_hal.o \
./Core/Src/sht4x_i2c.o \
./Core/Src/stm32u5xx_hal_msp.o \
./Core/Src/stm32u5xx_it.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32u5xx.o 

C_DEPS += \
./Core/Src/EMPA_DataCollector.d \
./Core/Src/EMPA_MqttAws.d \
./Core/Src/EMPA_Sensor.d \
./Core/Src/common.d \
./Core/Src/custom_bus.d \
./Core/Src/interrupt_handlers.d \
./Core/Src/linked_list.d \
./Core/Src/main.d \
./Core/Src/myESP32AT.d \
./Core/Src/sensirion_common.d \
./Core/Src/sensirion_i2c.d \
./Core/Src/sensirion_i2c_hal.d \
./Core/Src/sht4x_i2c.d \
./Core/Src/stm32u5xx_hal_msp.d \
./Core/Src/stm32u5xx_it.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32u5xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U575xx -c -I../Core/Inc -I../X-CUBE-ISPU/Target -I../Drivers/STM32U5xx_HAL_Driver/Inc -I../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/Components/ism330is -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/EMPA_DataCollector.cyclo ./Core/Src/EMPA_DataCollector.d ./Core/Src/EMPA_DataCollector.o ./Core/Src/EMPA_DataCollector.su ./Core/Src/EMPA_MqttAws.cyclo ./Core/Src/EMPA_MqttAws.d ./Core/Src/EMPA_MqttAws.o ./Core/Src/EMPA_MqttAws.su ./Core/Src/EMPA_Sensor.cyclo ./Core/Src/EMPA_Sensor.d ./Core/Src/EMPA_Sensor.o ./Core/Src/EMPA_Sensor.su ./Core/Src/common.cyclo ./Core/Src/common.d ./Core/Src/common.o ./Core/Src/common.su ./Core/Src/custom_bus.cyclo ./Core/Src/custom_bus.d ./Core/Src/custom_bus.o ./Core/Src/custom_bus.su ./Core/Src/interrupt_handlers.cyclo ./Core/Src/interrupt_handlers.d ./Core/Src/interrupt_handlers.o ./Core/Src/interrupt_handlers.su ./Core/Src/linked_list.cyclo ./Core/Src/linked_list.d ./Core/Src/linked_list.o ./Core/Src/linked_list.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/myESP32AT.cyclo ./Core/Src/myESP32AT.d ./Core/Src/myESP32AT.o ./Core/Src/myESP32AT.su ./Core/Src/sensirion_common.cyclo ./Core/Src/sensirion_common.d ./Core/Src/sensirion_common.o ./Core/Src/sensirion_common.su ./Core/Src/sensirion_i2c.cyclo ./Core/Src/sensirion_i2c.d ./Core/Src/sensirion_i2c.o ./Core/Src/sensirion_i2c.su ./Core/Src/sensirion_i2c_hal.cyclo ./Core/Src/sensirion_i2c_hal.d ./Core/Src/sensirion_i2c_hal.o ./Core/Src/sensirion_i2c_hal.su ./Core/Src/sht4x_i2c.cyclo ./Core/Src/sht4x_i2c.d ./Core/Src/sht4x_i2c.o ./Core/Src/sht4x_i2c.su ./Core/Src/stm32u5xx_hal_msp.cyclo ./Core/Src/stm32u5xx_hal_msp.d ./Core/Src/stm32u5xx_hal_msp.o ./Core/Src/stm32u5xx_hal_msp.su ./Core/Src/stm32u5xx_it.cyclo ./Core/Src/stm32u5xx_it.d ./Core/Src/stm32u5xx_it.o ./Core/Src/stm32u5xx_it.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32u5xx.cyclo ./Core/Src/system_stm32u5xx.d ./Core/Src/system_stm32u5xx.o ./Core/Src/system_stm32u5xx.su

.PHONY: clean-Core-2f-Src

