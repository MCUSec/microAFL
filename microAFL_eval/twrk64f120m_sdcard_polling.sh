#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_sdcard_polling/Debug/twrk64f120m_sdcard_polling.axf ./twrk64f120m_sdcard_polling.bin
../ETMFuzz_Src/ETMFuzz -t 3000 -i in_twrk64f120m_sdcard_polling -o out_twrk64f120m_sdcard_polling ./twrk64f120m_sdcard_polling.bin
