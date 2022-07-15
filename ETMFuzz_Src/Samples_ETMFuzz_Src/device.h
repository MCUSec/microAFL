#ifndef __DEVICE_H__
#define __DEVICE_H__
#define ETM_MAJOR_VERSION 3
#define ETM_MINOR_VERSION 5

#define HASH_VERSION    1 // 0 for old hash and 1 for new as our paper
#if HASH_VERSION==1
    #define MAX_STREAM_SIZE     4096
#endif

#define PERSISTENT_MODE         0
#define TESTCASE_LIMIT_SIZE     1024
#define FEED_WITH_RESET         1
#define SCRIPTEFILE             "empty.JLinkScript"
#define JLINK_SPEED             4000
#define JLINK_SERIAL_NO         932000275
// 0:Fibonacci; 
// 1:I2C; 
// 2:UART;
// 3:MMCAU;
// 4:USB:
// 5:Ethernet
// 6:SD Card
#define TARGET_PROJECT      1 
#define FIRST_WAIT          2000
#define HOST_INTERFACE      JLINKARM_HOSTIF_IP // JLINKARM_HOSTIF_USB is an alternative without IP configuration needed, but less performance in some projects.

#define DEVICE_NAME         "MK64FN1M0XXX12" //"TWR-K64F"
#define NEED_DOWNLOAD       1
#define IMAGEPOS            0x0
#ifdef JLINK_SPEED
#undef JLINK_SPEED
#endif
#define JLINK_SPEED         4000 
#ifdef SCRIPTEFILE
#undef SCRIPTEFILE
#endif
#define SCRIPTEFILE         "NXP_Kinetis_K64FN1M0_TraceExample.pex"
#if TARGET_PROJECT==0 // Fibonacci Project -> twrk64f120m_hello_world
    #define TESTCASEADDR            0x20000014
    #define TESTCASELENADDR         0x20000010
#elif TARGET_PROJECT==1 // I2C Project -> twrk64f120m_i2c_polling_b2b_transfer_master
    #define TESTCASEADDR            0x20000014
    #define TESTCASELENADDR         0x20000010
#elif TARGET_PROJECT==2 // UART Project -> twrk64f120m_uart_polling
    #define TESTCASEADDR            0x20000014
    #define TESTCASELENADDR         0x20000010
#elif TARGET_PROJECT==3 // MMCAU Project -> twrk64f120m_mmcau
    #define TESTCASEADDR            0x2000036c
    #define TESTCASELENADDR         0x20000368
#elif TARGET_PROJECT==4 // USB Project -> twrk64f120m_host_msd_fatfs_bm
    #define TESTCASEADDR         0x20000014
    #define TESTCASELENADDR      0x20000010
#elif TARGET_PROJECT==5 // Ethernet Project -> twrk64f120m_enet_txrx_transfer
    #define TESTCASELENADDR         0x20000018
    #define TESTCASEADDR            0x2000001c
#elif TARGET_PROJECT==6 // SD Card Project -> twrk64f120m_sdcard_polling
    #define TESTCASEADDR            0x20000054
    #define TESTCASELENADDR         0x20000050
#else
    #error Unknown project
#endif

#define TRACE_DELAY_TIME "0"

// registers for bkpt
#define PC_REG JLINKARM_CM4_REG_R15 // ARM_REG_R15
#define R0_REG JLINKARM_CM4_REG_R0 //ARM_REG_R0
// Reset register and its special value
#define DEMCR 0xE000EDFC // use for segger vector catch API(maybe) 
#define AIRCR 0xE000ED0C // Application Interrupt and Reset Control Register
#define ICSR 0xE000ED04 // Interrupt Control and State Register, [8:0] VECTACTIVEa RO Contains the active exception number:
// Debug registers used for single step
#define DHCSR 0xE000EDF0 // C_DEBUGEN, bit[0] Halting debug enable bit:
// Vector Catch
// DEMCR
#define DFSR 0xE000ED30 // Debug Fault Status Register
#define MAIN_FLAG 0xEF
// #define BKPT_END_OF_TESTCASE 0xABCD
#define END_FLAG 0xFF

#if (ETM_MAJOR_VERSION == 3)
    #if(ETM_MINOR_VERSION == 5)
        // ETM registers and their special value
        #define ETMLAR 0xE0041FB0
        #define UNLOCK_VALUE 0xC5ACCE55
        #define ETMCR 0xE0041000
        #define ETMTEEVR 0xE0041020
        #define ETMTEEVR_TRUE 0x6F
        #define ETMTRACEIDR 0xE0041200
        #define ETMTRACEIDR_ID 0x10 // 0x21 in raw trace
        // additional ETM
        #define ETMSYNCFR 0xE00411E0
        #define ETMTRIGGER 0xE0041008
        #define ETMTECR1 0xE0041024
        #define ETMTESSEICR 0xE00411F0
        // TPIU registers and their special value
        #define TPIU_CSPSR 0xE0040004
        #define TPIU_CSPSR_PORT_SIZE 0x8
        #define TPIU_SPPR 0xE00400F0
        #define TPIU_SPPR_PIN_PROTOCOL 0x0
    #else
        #error No target ETM minor version config is implemented now.
    #endif
#else
    #error No target ETM major version config is implemented now.
#endif

#endif