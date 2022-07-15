/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_uart.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */
#define DEMO_UART UART1
#define DEMO_UART_CLKSRC kCLOCK_CoreSysClk
#define DEMO_UART_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

//uint8_t txbuff[]   = "Uart polling example\r\nBoard will send back received characters\r\n";
uint8_t rxbuff[20] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
unsigned int etm_tc_len __attribute__((section(".non_init")));
unsigned char etm_tc[2000] __attribute__((section(".non_init")));
int main(void)
{
    uint8_t ch;
    uart_config_t config;

    BOARD_InitPins();
    BOARD_BootClockRUN();

    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kUART_ParityDisabled;
     * config.stopBitCount = kUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 1;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    UART_Init(DEMO_UART, &config, DEMO_UART_CLK_FREQ);
    __asm("bkpt 0xEF\n\t");
//    UART_WriteBlocking(DEMO_UART, txbuff, sizeof(txbuff) - 1);
    UART_WriteBlocking(DEMO_UART, etm_tc, etm_tc_len);
    __asm("bkpt 0xFF\n\t");
//    while (1)
//    {
//        UART_ReadBlocking(DEMO_UART, &ch, 1);
//        UART_WriteBlocking(DEMO_UART, &ch, 1);
//    }
}
