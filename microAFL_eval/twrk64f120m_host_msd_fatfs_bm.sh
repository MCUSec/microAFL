#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_host_msd_fatfs_bm/Debug/twrk64f120m_host_msd_fatfs_bm.axf ./twrk64f120m_host_msd_fatfs_bm.bin
../ETMFuzz_Src/ETMFuzz -t 800 -i in_twrk64f120m_host_msd_fatfs_bm -o out ./twrk64f120m_host_msd_fatfs_bm.bin
