/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
unsigned int etm_tc_len __attribute__((section(".non_init")));
unsigned char etm_tc[2000] __attribute__((section(".non_init")));
unsigned int etm_tc_idx = 0;
unsigned int etm_exit = 0;
int main(void)
{
    char ch;

    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
//    etm_tc_len = 0;
    __asm("bkpt 0xEF\n\t");
//    int i = 1000000;
//    int sum = 0;
////    etm_tc_len = 5;
////    etm_tc[0] = 'H';
////    etm_tc[1] = 'e';
////    etm_tc[2] = 'l';
////    etm_tc[3] = 'l';
////    etm_tc[4] = 'o';
//    while(i--){
//    	if(etm_tc_len >=1 && etm_tc[0] == 'H'){
//    		if(etm_tc_len >=2 && etm_tc[1] == 'e'){
//    			if(etm_tc_len >=3 && etm_tc[2] == 'l'){
//    				if(etm_tc_len >=4 && etm_tc[3] == 'l'){
//    					if(etm_tc_len >=5 && etm_tc[4] == 'o'){
//    						sum++;
//    						sum+=4;
//    						sum++;
//    					}else{
//    						sum--;
//    						sum-=4;
//    						sum--;
//    					}
//    				}else{
//    					sum--;
//						sum+=3;
//						sum--;
//    				}
//    			}else{
//					sum--;
//					sum+=2;
//					sum--;
//    			}
//    		}else{
//				sum--;
//				sum+=1;
//				sum--;
//    		}
//    	}else{
//			sum--;
//			sum+=5;
//			sum--;
//    	}
//    }
    /* Fibonacci */
    unsigned int a = 0, b = 1, temp = 0;
    unsigned int end;
    if(etm_tc_len >= sizeof(unsigned int))
    	memcpy(&end, etm_tc, sizeof(end));
    else
    	end = 10000;
    end = end % 10001;
    while(1){
    	temp = a + b;
    	if(temp >= end)
    		break;
    	a = b;
    	b = temp;
    }
    __asm("bkpt 0xFF\n\t");

}
