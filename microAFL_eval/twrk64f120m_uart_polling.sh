#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_uart_polling/Debug/twrk64f120m_uart_polling.axf ./twrk64f120m_uart_polling.bin
../ETMFuzz_Src/ETMFuzz -t 800 -i in_twrk64f120m_uart_polling -o out_twrk64f120m_uart_polling ./twrk64f120m_uart_polling.bin
