################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_gpio.c \
../drivers/fsl_smc.c \
../drivers/fsl_uart.c 

OBJS += \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_gpio.o \
./drivers/fsl_smc.o \
./drivers/fsl_uart.o 

C_DEPS += \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_gpio.d \
./drivers/fsl_smc.d \
./drivers/fsl_uart.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MK64FN1M0VMD12 -DCPU_MK64FN1M0VMD12_cm4 -DTWR_K64F120M -DTOWER -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=0 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/board" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/source" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/drivers" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/device" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/utilities" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/component/uart" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/component/serial_manager" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/component/lists" -I"/home/lwq/microAFL/microAFL_eval/twrk64f120m_uart_polling/CMSIS" -O0 -fno-common -g3 -Wall -c  -ffunction-sections  -fdata-sections  -ffreestanding  -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


