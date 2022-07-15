################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../startup/startup_mk64f12.c 

OBJS += \
./startup/startup_mk64f12.o 

C_DEPS += \
./startup/startup_mk64f12.d 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MK64FN1M0VMD12 -DCPU_MK64FN1M0VMD12_cm4 -DPRINTF_ADVANCED_ENABLE=1 -DTWR_K64F120M -DTOWER -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DNDEBUG -D__REDLIB__ -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/board" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/source" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/drivers" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/device" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/utilities" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/component/uart" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/component/serial_manager" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/component/lists" -I"/home/lwq/microAFL_eval/twrk64f120m_enet_txrx_transfer/CMSIS" -Os -fno-common -g -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


