/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_sd.h"
#include "fsl_sysmpu.h"
#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*! @brief Data block count accessed in card */
#define DATA_BLOCK_COUNT (5U)
/*! @brief Start data block number accessed in card */
#define DATA_BLOCK_START (2U)
/*! @brief Data buffer size. */
#define DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * DATA_BLOCK_COUNT)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief printf the card information log.
 *
 * @param card Card descriptor.
 */
static void CardInformationLog(sd_card_t *card);
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor. */
sd_card_t g_sd;

/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
/*! @brief Data written to the card */
SDK_ALIGN(uint8_t g_dataWrite[SDK_SIZEALIGN(DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_dataRead[SDK_SIZEALIGN(DATA_BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

/*! @brief SDMMC host detect card configuration */
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};

/*! @brief SDMMC card power control configuration */
#if defined DEMO_SDCARD_POWER_CTRL_FUNCTION_EXIST
static const sdmmchost_pwr_card_t s_sdCardPwrCtrl = {
    .powerOn          = BOARD_PowerOnSDCARD,
    .powerOnDelay_ms  = 500U,
    .powerOff         = BOARD_PowerOffSDCARD,
    .powerOffDelay_ms = 0U,
};
#endif
#if defined DEMO_SDCARD_SWITCH_VOLTAGE_FUNCTION_EXIST
static const sdmmchost_card_switch_voltage_func_t s_sdCardVoltageSwitch = {
    .cardSignalLine1V8 = BOARD_USDHC_Switch_VoltageTo1V8,
    .cardSignalLine3V3 = BOARD_USDHC_Switch_VoltageTo3V3,
};
#endif
/*******************************************************************************
 * Code
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t AccessCard(sd_card_t *card, bool isReadOnly)
{
    if (isReadOnly)
    {
        PRINTF("\r\nRead one data block......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            PRINTF("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        PRINTF("Read multiple data blocks......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }
    else
    {
        memset(g_dataWrite, 0x67U, sizeof(g_dataWrite));

        PRINTF("\r\nWrite/read one data block......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, 1U))
        {
            PRINTF("Write one data block failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            PRINTF("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        PRINTF("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        PRINTF("The read/write content is consistent.\r\n");

        PRINTF("Write/read multiple data blocks......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Write multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));
        SDMMCHOST_Delay(5);
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        PRINTF("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        PRINTF("The read/write content is consistent.\r\n");

        PRINTF("Erase multiple data blocks......\r\n");
        if (kStatus_Success != SD_EraseBlocks(card, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Erase multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }

    return kStatus_Success;
}

/*!
 * @brief Main function
 */
unsigned int etm_tc_len __attribute__((section(".non_init")));
unsigned char etm_tc[10000] __attribute__((section(".non_init")));
unsigned int etm_tc_idx = 0;
unsigned int etm_exit = 0;
int Delay_SD_ApplicationSendOperationCondition = 0;
int main(void)
{
    sd_card_t *card = &g_sd;
    char ch         = '0';
    bool isReadOnly;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    SYSMPU_Enable(SYSMPU, false);

    card->host.base           = SD_HOST_BASEADDR;
    card->host.sourceClock_Hz = SD_HOST_CLK_FREQ;
    /* card detect type */
    card->usrParam.cd = &s_sdCardDetect;
#if defined DEMO_SDCARD_POWER_CTRL_FUNCTION_EXIST
    card->usrParam.pwr = &s_sdCardPwrCtrl;
#endif
#if defined DEMO_SDCARD_SWITCH_VOLTAGE_FUNCTION_EXIST
    card->usrParam.cardVoltage = &s_sdCardVoltageSwitch;
#endif
    PRINTF("\r\nSDCARD polling example.\r\n");

    /* SD host init function */
    if (SD_HostInit(card) != kStatus_Success)
    {
        PRINTF("\r\nSD host init fail\r\n");
        return -1;
    }

//    while (ch != 'q')
//    {
        PRINTF("\r\nPlease insert a card into board.\r\n");
        /* power off card */
        SD_PowerOffCard(card->host.base, card->usrParam.pwr);

        if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR, &s_sdCardDetect, true) == kStatus_Success)
        {
            PRINTF("\r\nCard inserted.\r\n");
            /* reset host once card re-plug in */
            SD_HostReset(&(card->host));
            /* power on the card */
            SD_PowerOnCard(card->host.base, card->usrParam.pwr);
        }
        else
        {
            PRINTF("\r\nCard detect fail.\r\n");
            return -1;
        }
//        etm_tc_len = 0;
//        char temp[] = {0xaa, 0x01, 0x00, 0x00, 0x20, 0x01, 0x00, 0x00, 0x00, 0x80, 0xff, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x80, 0xff, 0x40, 0x20, 0x01, 0x00, 0x00, 0x00, 0x80, 0xff, 0xc0, 0x28, 0x01, 0x80, 0xcc, 0x19, 0x4d, 0x80, 0x47, 0x36, 0x31, 0x43, 0x53, 0x20, 0x05, 0xaa, 0xaa, 0x40, 0x40, 0x0a, 0x80, 0x7f, 0xb2, 0x76, 0x00, 0x00, 0x59, 0x5b, 0x32, 0x00, 0x07, 0x00, 0x00, 0x20, 0x09, 0x00, 0x00, 0x20, 0x09, 0x00, 0x00, 0x20, 0x09, 0x00, 0x00, 0x20, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x20, 0x09, 0x00, 0x00, 0x20, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00};
//        etm_tc_len = sizeof(temp);
//        memcpy(etm_tc, temp, etm_tc_len);
        __asm("bkpt 0xEF\n\t");
        /* Init card. */
        if (SD_CardInit(card))
        {
            PRINTF("\r\nSD card init failed.\r\n");
            // ETMFuzz
            __asm("bkpt 0xFF\n\t");
            // ETMFuzz end
            return -1;
        }
        /* card information log */
        CardInformationLog(card);

        /* Check if card is readonly. */
        isReadOnly = SD_CheckReadOnly(card);

        PRINTF("\r\nRead/Write/Erase the card continuously until encounter error......\r\n");

//        for (;;)
//        {
            if (kStatus_Success != AccessCard(card, isReadOnly))
            {
//                /* access card fail, due to card remove. */
//                if (SD_WaitCardDetectStatus(SD_HOST_BASEADDR, &s_sdCardDetect, false) == kStatus_Success)
//                {
//                    PRINTF("\r\nCard removed\r\n");
////                    PRINTF(
////                        "\r\nInput 'q' to quit read/write/erase process.\
////                \r\nInput other char to wait card re-insert.\r\n");
////                    ch = GETCHAR();
////                    PUTCHAR(ch);
//                }
//                /* access card fail, due to transfer error */
////                else
////                {
////                    ch = 'q';
////                }
////
////                break;
            }
//            else
//            {
//                PRINTF(
//                    "\r\nInput 'q' to quit read/write/erase process.\
//                \r\nInput other char to read/write/erase data blocks again.\r\n");
//                ch = GETCHAR();
//                PUTCHAR(ch);
//                if (ch == 'q')
//                {
//                    break;
//                }
//            }
//        }
//    }
	// ETMFuzz
	__asm("bkpt 0xFF\n\t");
	// ETMFuzz end
    PRINTF("\r\nThe example will not read/write data blocks again.\r\n");
    SD_Deinit(card);

    while (true)
    {
    }
}

static void CardInformationLog(sd_card_t *card)
{
    assert(card);

    PRINTF("\r\nCard size %d * %d bytes\r\n", card->blockCount, card->blockSize);
    PRINTF("\r\nWorking condition:\r\n");
    if (card->operationVoltage == kCARD_OperationVoltage330V)
    {
        PRINTF("\r\n  Voltage : 3.3V\r\n");
    }
    else if (card->operationVoltage == kCARD_OperationVoltage180V)
    {
        PRINTF("\r\n  Voltage : 1.8V\r\n");
    }

    if (card->currentTiming == kSD_TimingSDR12DefaultMode)
    {
        if (card->operationVoltage == kCARD_OperationVoltage330V)
        {
            PRINTF("\r\n  Timing mode: Default mode\r\n");
        }
        else if (card->operationVoltage == kCARD_OperationVoltage180V)
        {
            PRINTF("\r\n  Timing mode: SDR12 mode\r\n");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR25HighSpeedMode)
    {
        if (card->operationVoltage == kCARD_OperationVoltage180V)
        {
            PRINTF("\r\n  Timing mode: SDR25\r\n");
        }
        else
        {
            PRINTF("\r\n  Timing mode: High Speed\r\n");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR50Mode)
    {
        PRINTF("\r\n  Timing mode: SDR50\r\n");
    }
    else if (card->currentTiming == kSD_TimingSDR104Mode)
    {
        PRINTF("\r\n  Timing mode: SDR104\r\n");
    }
    else if (card->currentTiming == kSD_TimingDDR50Mode)
    {
        PRINTF("\r\n  Timing mode: DDR50\r\n");
    }

    PRINTF("\r\n  Freq : %d HZ\r\n", card->busClock_Hz);
}
