#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_hello_world/Debug/twrk64f120m_hello_world.axf ./Fibonacci.bin
../ETMFuzz_Src/ETMFuzz -t 50 -i in_hello_world -o out_twrk64f120m_Fibonacci ./Fibonacci.bin
