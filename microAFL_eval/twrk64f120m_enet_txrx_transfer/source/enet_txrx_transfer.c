/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_enet.h"
#include "fsl_phy.h"
#if defined(FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET) && FSL_FEATURE_MEMORY_HAS_ADDRESS_OFFSET
#include "fsl_memory.h"
#endif
#include "pin_mux.h"
#include "fsl_common.h"
#include "fsl_sysmpu.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* ENET base address */
#define EXAMPLE_ENET ENET
#define EXAMPLE_PHY 0x00U
#define CORE_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define ENET_RXBD_NUM (4)
#define ENET_TXBD_NUM (4)
#define ENET_RXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#define ENET_TXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#define ENET_DATA_LENGTH (1000)
#define ENET_TRANSMIT_DATA_NUM (20)
#ifndef APP_ENET_BUFF_ALIGNMENT
#define APP_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*! @brief Build ENET broadcast frame. */
static void ENET_BuildBroadCastFrame(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Buffer descriptors should be in non-cacheable region and should be align to "ENET_BUFF_ALIGNMENT". */
AT_NONCACHEABLE_SECTION_ALIGN(enet_rx_bd_struct_t g_rxBuffDescrip[ENET_RXBD_NUM], ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(enet_tx_bd_struct_t g_txBuffDescrip[ENET_TXBD_NUM], ENET_BUFF_ALIGNMENT);
/*! @brief The data buffers can be in cacheable region or in non-cacheable region.
 * If use cacheable region, the alignment size should be the maximum size of "CACHE LINE SIZE" and "ENET_BUFF_ALIGNMENT"
 * If use non-cache region, the alignment size is the "ENET_BUFF_ALIGNMENT".
 */
SDK_ALIGN(uint8_t g_rxDataBuff[ENET_RXBD_NUM][SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)],
          APP_ENET_BUFF_ALIGNMENT);
SDK_ALIGN(uint8_t g_txDataBuff[ENET_TXBD_NUM][SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT)],
          APP_ENET_BUFF_ALIGNMENT);

enet_handle_t g_handle;
uint8_t g_frame[ENET_DATA_LENGTH];
uint32_t g_testTxNum = 0;

/*! @brief The MAC address for ENET device. */
uint8_t g_macAddr[6] = {0xd4, 0xbe, 0xd9, 0x45, 0x22, 0x60};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*! @brief Build Frame for transmit. */
static void ENET_BuildBroadCastFrame(void)
{
    uint32_t count  = 0;
    uint32_t length = ENET_DATA_LENGTH - 14;

    for (count = 0; count < 6U; count++)
    {
        g_frame[count] = 0xFFU;
    }
    memcpy(&g_frame[6], &g_macAddr[0], 6U);
    g_frame[12] = (length >> 8) & 0xFFU;
    g_frame[13] = length & 0xFFU;

    for (count = 0; count < length; count++)
    {
        g_frame[count + 14] = count % 0xFFU;
    }
}

/*!
 * @brief Main function
 */
unsigned int etm_tc_len __attribute__((section(".non_init")));
unsigned char etm_tc[2000] __attribute__((section(".non_init")));
unsigned int etm_tc_idx = 0;
unsigned int etm_exit = 0;
int main(void)
{
    enet_config_t config;
    uint32_t length = 0;
    uint32_t sysClock;
    bool link = false;
    phy_speed_t speed;
    phy_duplex_t duplex;
    uint32_t txnumber = 0;
    status_t status;
    enet_data_error_stats_t eErrStatic;

    /* Hardware Initialization. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    /* Disable SYSMPU. */
    SYSMPU_Enable(SYSMPU, false);

//    PRINTF("\r\n ENET example start.\r\n");

    /* prepare the buffer configuration. */
    enet_buffer_config_t buffConfig[] = {{
        ENET_RXBD_NUM,
        ENET_TXBD_NUM,
        SDK_SIZEALIGN(ENET_RXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT),
        SDK_SIZEALIGN(ENET_TXBUFF_SIZE, APP_ENET_BUFF_ALIGNMENT),
        &g_rxBuffDescrip[0],
        &g_txBuffDescrip[0],
        &g_rxDataBuff[0][0],
        &g_txDataBuff[0][0],
    }};

    /* Get default configuration. */
    /*
     * config.miiMode = kENET_RmiiMode;
     * config.miiSpeed = kENET_MiiSpeed100M;
     * config.miiDuplex = kENET_MiiFullDuplex;
     * config.rxMaxFrameLen = ENET_FRAME_MAX_FRAMELEN;
     */
    ENET_GetDefaultConfig(&config);
    // ETM_Fuzz
//    etm_tc_len = 0;
//    char temp[] = {0x22, 0x00, 0x00, 0x00, 0x6d, 0x78, 0x00, 0x00, 0x6d, 0x78, 0x00, 0x00, 0x98, 0x91, 0x00, 0x00, 0x3c, 0x00, 0x80, 0x08, 0x30, 0x08, 0x00, 0x20, 0x3c, 0x00, 0x80, 0x08, 0x20, 0x0e, 0x00, 0x20, 0x3c, 0x00, 0x80, 0x08, 0x10, 0x14, 0x00, 0x20, 0x3c, 0x00, 0x80, 0x28, 0x00, 0x1a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0xf0, 0x1f, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0xe0, 0x25, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0xd0, 0x2b, 0x00, 0x20, 0x00, 0x00, 0x00, 0x24};
//    etm_tc_len = sizeof(temp);
//    memcpy(etm_tc, temp, etm_tc_len);
    __asm("bkpt 0xEF\n\t");
    /* Set SMI to get PHY link status. */
    sysClock = CORE_CLK_FREQ;
    status   = PHY_Init(EXAMPLE_ENET, EXAMPLE_PHY, sysClock);
    while (status != kStatus_Success)
    {
    	__asm("bkpt 0xFF\n\t");
//        PRINTF("\r\nPHY Auto-negotiation failed. Please check the cable connection and link partner setting.\r\n");
//        status = PHY_Init(EXAMPLE_ENET, EXAMPLE_PHY, sysClock);
    }

    PHY_GetLinkStatus(EXAMPLE_ENET, EXAMPLE_PHY, &link);
    if (link)
    {
        /* Get the actual PHY link speed. */
        PHY_GetLinkSpeedDuplex(EXAMPLE_ENET, EXAMPLE_PHY, &speed, &duplex);
        /* Change the MII speed and duplex for actual link status. */
        config.miiSpeed  = (enet_mii_speed_t)speed;
        config.miiDuplex = (enet_mii_duplex_t)duplex;
    }

    ENET_Init(EXAMPLE_ENET, &g_handle, &config, &buffConfig[0], &g_macAddr[0], sysClock);
    ENET_ActiveRead(EXAMPLE_ENET);

    /* Build broadcast for sending. */
//    ENET_BuildBroadCastFrame();

//    while (1)
//    {
    	// ETMFuzz
    	if(etm_tc_idx >= etm_tc_len){
    		__asm("bkpt 0xFF\n\t");
    	}
    	memcpy(g_handle.rxBdCurrent[0], etm_tc + etm_tc_idx, etm_tc_len - etm_tc_idx);
    	// ETMFuzz
        /* Get the Frame size */
        status = ENET_GetRxFrameSize(&g_handle, &length);
        /* Call ENET_ReadFrame when there is a received frame. */
        if (length != 0)
        {
        	// ETMFuzz
//        	memcpy(etm_tc + etm_tc_len, g_handle.rxBdCurrent[0], length);
//        	etm_tc_len += length;
        	//  ETMFuzz
            /* Received valid frame. Deliver the rx buffer with the size equal to length. */
            uint8_t *data = (uint8_t *)malloc(length);
            status        = ENET_ReadFrame(EXAMPLE_ENET, &g_handle, data, length);
//            if (status == kStatus_Success)
//            {
//                PRINTF(" A frame received. the length %d ", length);
//                PRINTF(" Dest Address %02x:%02x:%02x:%02x:%02x:%02x Src Address %02x:%02x:%02x:%02x:%02x:%02x \r\n",
//                       data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9],
//                       data[10], data[11]);
//            }
            free(data);
        }
        else if (status == kStatus_ENET_RxFrameError)
        {
            /* Update the received buffer when error happened. */
            /* Get the error information of the received g_frame. */
            ENET_GetRxErrBeforeReadFrame(&g_handle, &eErrStatic);
            /* update the receive buffer. */
            ENET_ReadFrame(EXAMPLE_ENET, &g_handle, NULL, 0);
        }

//        if (g_testTxNum < ENET_TRANSMIT_DATA_NUM)
//        {
//            /* Send a multicast frame when the PHY is link up. */
//            if (kStatus_Success == PHY_GetLinkStatus(EXAMPLE_ENET, EXAMPLE_PHY, &link))
//            {
//                if (link)
//                {
//                    g_testTxNum++;
//                    txnumber++;
//                    if (kStatus_Success == ENET_SendFrame(EXAMPLE_ENET, &g_handle, &g_frame[0], ENET_DATA_LENGTH))
//                    {
//                        PRINTF("The %d frame transmitted success!\r\n", txnumber);
//                    }
//                    else
//                    {
//                        PRINTF(" \r\nTransmit frame failed!\r\n");
//                    }
//                }
//            }
//        }
//    }
    __asm("bkpt 0xFF\n\t");
}
