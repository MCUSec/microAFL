#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_mmcau/Debug/twrk64f120m_mmcau.axf ./twrk64f120m_mmcau.bin
../ETMFuzz_Src/ETMFuzz -t 50 -i in_twrk64f120m_mmcau_aes -o out_twrk64f120m_mmcau ./twrk64f120m_mmcau.bin
