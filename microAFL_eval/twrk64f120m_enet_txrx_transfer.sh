#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_enet_txrx_transfer/Debug/twrk64f120m_enet_txrx_transfer.axf ./twrk64f120m_enet_txrx_transfer.bin
../ETMFuzz_Src/ETMFuzz -t 3000 -i in_twrk64f120m_enet_txrx_transfer -o out_twrk64f120m_enet_txrx_transfer ./twrk64f120m_enet_txrx_transfer.bin
