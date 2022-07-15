################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include utilities/subdir.mk
-include startup/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/serial_manager/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: twrk64f120m_enet_txrx_transfer.axf

# Tool invocations
twrk64f120m_enet_txrx_transfer.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -Xlinker -Map="twrk64f120m_enet_txrx_transfer.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -T twrk64f120m_enet_txrx_transfer_Release.ld -o "twrk64f120m_enet_txrx_transfer.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) twrk64f120m_enet_txrx_transfer.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "twrk64f120m_enet_txrx_transfer.axf"; # arm-none-eabi-objcopy -v -O binary "twrk64f120m_enet_txrx_transfer.axf" "twrk64f120m_enet_txrx_transfer.bin" ; # checksum -p MK64FN1M0xxx12 -d "twrk64f120m_enet_txrx_transfer.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
