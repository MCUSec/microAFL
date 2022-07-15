#!/bin/bash
# arm-none-eabi-objcopy -O binary ./twrk64f120m_i2c_polling_b2b_transfer_master/Debug/twrk64f120m_i2c_polling_b2b_transfer_master.axf ./twrk64f120m_i2c_polling_b2b_transfer_master.bin
../ETMFuzz_Src/ETMFuzz -t 50 -i in_twrk64f120m_i2c_polling_b2b_transfer_master -o out_twrk64f120m_i2c_polling_b2b_transfer_master ./twrk64f120m_i2c_polling_b2b_transfer_master.bin
