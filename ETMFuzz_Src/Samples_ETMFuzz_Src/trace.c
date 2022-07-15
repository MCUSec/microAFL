#include <unistd.h>
#include <stdio.h>
#include "JLINK_API.h"

#include "device.h"
#include "trace.h"
#include "common.h"

static void AFL_ERROR_OUT(const char* sError) {
	printf("\nERROR: %s\n", sError);
    // printf("press any key to exit.\n");
    // system("pause");
    // exit(-1);
}

void STM32F407VE_OneTimeTraceStart(){
	U32 RCC_AHB1ENR_Addr;
	U32 GPIOE_MODER_Addr;
	U32 GPIOE_PUPDR_Addr;
	U32 GPIOE_OSPEEDR_Addr;
	U32 GPIOE_AFRL_Addr;
	U32 DBGMCU_CR_Addr;
	U32 iTCLK;
	U32 iTD0;
	U32 iTD1;
	U32 iTD2;
	U32 iTD3;
	U32 EdgeTCLK;
	U32 EdgeTD;
	U32 v;
	U32 PortWidth;

	RCC_AHB1ENR_Addr   = 0x40023830;
	GPIOE_MODER_Addr   = 0x40021000;
	GPIOE_PUPDR_Addr   = 0x4002100C;
	GPIOE_OSPEEDR_Addr = 0x40021008;
	GPIOE_AFRL_Addr    = 0x40021020;
	DBGMCU_CR_Addr     = 0xE0042004;
	iTCLK = 2;
	iTD0  = 3;
	iTD1  = 4;
	iTD2  = 5;
	iTD3  = 6;
	PortWidth = 4; //JLINK_TRACE_PortWidth;

	EdgeTCLK = 3;
	EdgeTD   = 3;

	JLINKARM_ReadMemU32(RCC_AHB1ENR_Addr, 1, &v, NULL);

	JLINKARM_WriteU32(RCC_AHB1ENR_Addr, v | 1 << 4); // Enable clock for GPIOE

	JLINKARM_ReadMemU32(GPIOE_MODER_Addr, 1, &v, NULL);
	v  &= ~(3 << (2 * iTCLK));               // Mask Mode register
	v  |= (2 << (2 * iTCLK));                // Set alt function mode
	JLINKARM_WriteU32(GPIOE_MODER_Addr, v);
	JLINKARM_ReadMemU32(GPIOE_PUPDR_Addr, 1, &v, NULL);
	v  &= ~(3 << (2 * iTCLK));               // Mask PUP register
	v  |= (1 << (2 * iTCLK));            // Set PUP register (Pullup)
	
	JLINKARM_WriteU32(GPIOE_PUPDR_Addr, v);
	JLINKARM_ReadMemU32(GPIOE_OSPEEDR_Addr, 1, &v, NULL);
	v  &= ~(3 << (2 * iTCLK));               // Mask OSPEED register
	v  |= (EdgeTCLK << (2 * iTCLK));                // Set OSPEED register (very high speed)
	JLINKARM_WriteU32(GPIOE_OSPEEDR_Addr, v);
	JLINKARM_ReadMemU32(GPIOE_AFRL_Addr, 1, &v, NULL);
	v  &= ~(15 << (4 * iTCLK));              // Select alt func 0
	JLINKARM_WriteU32(GPIOE_AFRL_Addr, v);

	JLINKARM_ReadMemU32(GPIOE_MODER_Addr, 1, &v, NULL);
	v  &= ~(3 << (2 * iTD0));               // Mask Mode register
	v  |= (2 << (2 * iTD0));                // Set alt function mode
	JLINKARM_WriteU32(GPIOE_MODER_Addr, v);
	JLINKARM_ReadMemU32(GPIOE_PUPDR_Addr, 1, &v, NULL);
	v  &= ~(3 << (2 * iTD0));               // Mask PUP register
	v  |= (1 << (2 * iTD0));                // Set PUP register (Pullup)
	JLINKARM_WriteU32(GPIOE_PUPDR_Addr, v);
	JLINKARM_ReadMemU32(GPIOE_OSPEEDR_Addr, 1, &v, NULL);

	v  &= ~(3 << (2 * iTD0));               // Mask OSPEED register
	v  |= (EdgeTD << (2 * iTD0));                // Set OSPEED register (very high speed)
	JLINKARM_WriteU32(GPIOE_OSPEEDR_Addr, v);
	JLINKARM_ReadMemU32(GPIOE_AFRL_Addr, 1, &v, NULL);
	v  &= ~(15 << (4 * iTD0));              // Select alt func 0
	JLINKARM_WriteU32(GPIOE_AFRL_Addr, v);

	if (PortWidth > 1) {
		JLINKARM_ReadMemU32(GPIOE_MODER_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD1));               // Mask Mode register
		v  |= (2 << (2 * iTD1));                // Set alt function mode
		JLINKARM_WriteU32(GPIOE_MODER_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_PUPDR_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD1));               // Mask PUP register
		v  |= (1 << (2 * iTD1));                // Set PUP register (Pullup)
		JLINKARM_WriteU32(GPIOE_PUPDR_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_OSPEEDR_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD1));               // Mask OSPEED register
		v  |= (EdgeTD << (2 * iTD1));                // Set OSPEED register (very high speed)
		JLINKARM_WriteU32(GPIOE_OSPEEDR_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_AFRL_Addr, 1, &v, NULL);
		v  &= ~(15 << (4 * iTD1));              // Select alt func 0
		JLINKARM_WriteU32(GPIOE_AFRL_Addr, v);
	}

	if (PortWidth > 2) {
		JLINKARM_ReadMemU32(GPIOE_MODER_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD2));               // Mask Mode register
		v  |= (2 << (2 * iTD2));                // Set alt function mode
		JLINKARM_WriteU32(GPIOE_MODER_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_PUPDR_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD2));               // Mask PUP register
		v  |= (1 << (2 * iTD2));                // Set PUP register (Pullup)
		JLINKARM_WriteU32(GPIOE_PUPDR_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_OSPEEDR_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD2));               // Mask OSPEED register
		v  |= (EdgeTD << (2 * iTD2));                // Set OSPEED register (very high speed)
		JLINKARM_WriteU32(GPIOE_OSPEEDR_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_AFRL_Addr, 1, &v, NULL);
		v  &= ~(15 << (4 * iTD2));              // Select alt func 0
		JLINKARM_WriteU32(GPIOE_AFRL_Addr, v);

		JLINKARM_ReadMemU32(GPIOE_MODER_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD3));               // Mask Mode register
		v  |= (2 << (2 * iTD3));                // Set alt function mode
		JLINKARM_WriteU32(GPIOE_MODER_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_PUPDR_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD3));               // Mask PUP register
		v  |= (1 << (2 * iTD3));                // Set PUP register (Pullup)
		JLINKARM_WriteU32(GPIOE_PUPDR_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_OSPEEDR_Addr, 1, &v, NULL);
		v  &= ~(3 << (2 * iTD3));               // Mask OSPEED register
		v  |= (EdgeTD << (2 * iTD3));                // Set OSPEED register (very high speed)
		JLINKARM_WriteU32(GPIOE_OSPEEDR_Addr, v);
		JLINKARM_ReadMemU32(GPIOE_AFRL_Addr, 1, &v, NULL);
		v  &= ~(15 << (4 * iTD3));              // Select alt func 0
		JLINKARM_WriteU32(GPIOE_AFRL_Addr, v);

		JLINKARM_ReadMemU32(DBGMCU_CR_Addr, 1, &v, NULL);               // Debug MCU sets how many Pins to use for trace (STM32 specific)
		v  &= ~(7 << 5);               // Mask Register
		v  |= (7 << 5);                // Enable Trace and Select configured Tracepins for Trace
		JLINKARM_WriteU32(DBGMCU_CR_Addr, v);
	}
	if (PortWidth < 3){
		JLINKARM_ReadMemU32(DBGMCU_CR_Addr, 1, &v, NULL);              // Debug MCU sets how many Pins to use for trace (STM32 specific)
		v  &= ~(7 << 5);               // Mask Register
		v  |= (1 << 5);                // Enable Trace
		v  |= ((PortWidth) << 6);                // Enable Trace and Select configured Tracepins for Trace
		JLINKARM_WriteU32(DBGMCU_CR_Addr, v);
	}
}

void MK64FN1M0XXX12_OneTimeTraceStart(){
	U32 TracePortWidth;
	U32 SIM_SOPT2;
	U32 SIM_SCGC5;
	U32 Trace_CLKOUT;
	U32 Trace_D0;
	U32 Trace_D1;
	U32 Trace_D2;
	U32 Trace_D3;
	U32 EdgeTCLK;
	U32 EdgeTD;
	U32 v;
	TracePortWidth = 4; //JLINK_TRACE_PortWidth;

	SIM_SCGC5    = 0x40048038;
	SIM_SOPT2    = 0x40048004;
	Trace_CLKOUT = 0x4004D000 + (4 * 0);
	Trace_D0     = 0x4004D000 + (4 * 4);
	Trace_D1     = 0x4004D000 + (4 * 3);
	Trace_D2     = 0x4004D000 + (4 * 2);
	Trace_D3     = 0x4004D000 + (4 * 1);

	EdgeTCLK = 1;
	EdgeTD   = 1;

	JLINKARM_ReadMemU32(SIM_SCGC5, 1, &v, NULL);
	v |= (1 << 13);                    // PORTE: 1 == Enable clock for PORTE registers
	JLINKARM_WriteU32(SIM_SCGC5, v);
	JLINKARM_ReadMemU32(SIM_SOPT2, 1, &v, NULL);
	v &= ~(1 << (1 * 12));
	v |=  (1 << (1 * 12));  
	JLINKARM_WriteU32(SIM_SOPT2, v); // System Options Register 2 (SIM_SOPT2): TRACECLKSEL (Debug trace clock select) : 1 == Core/system clock
	JLINKARM_ReadMemU32(Trace_CLKOUT, 1, &v, NULL);
	v &= ~(5 << 8);
	v |=  (5 << 8);
	v |=  (EdgeTCLK << 6);
	JLINKARM_WriteU32(Trace_CLKOUT, v); // Select ALT Mode 5 of Pin E0 (Trace_CLKOUT) and enable High drive strength
	JLINKARM_ReadMemU32(Trace_D0, 1, &v, NULL);
	v &= ~(5 << 8);
	v |=  (5 << 8);
	v |=  (EdgeTD << 6);
	JLINKARM_WriteU32(Trace_D0, v); // Select ALT Mode 5 of Pin E4 (Trace_D0) and enable High drive strength
	if (TracePortWidth > 1) {
		JLINKARM_ReadMemU32(Trace_D1, 1, &v, NULL);
		v &= ~(5 << 8);
		v |=  (5 << 8);
		v |= (EdgeTD << 6);
		JLINKARM_WriteU32(Trace_D1, v); // Select ALT Mode 5 of Pin E3 (Trace_D1) and enable High drive strength
	}
	if (TracePortWidth > 2) {
		JLINKARM_ReadMemU32(Trace_D2, 1, &v, NULL);
		v &= ~(5 << 8);
		v |=  (5 << 8);
		v |= (EdgeTD << 6);
		JLINKARM_WriteU32(Trace_D2, v); // Select ALT Mode 5 of Pin E2 (Trace_D2) and enable High drive strength
		JLINKARM_ReadMemU32(Trace_D3, 1, &v, NULL);
		v &= ~(5 << 8);
		v |=  (5 << 8);
		v |= (EdgeTD << 6);
		JLINKARM_WriteU32(Trace_D3, v); // Select ALT Mode 5 of Pin E1 (Trace_D3) and enable High drive strength
	}
} 

void M2351KIAAE_OneTimeTraceStart(){
	U32 SYS_GPE_MFPH;
	U32 PE_MODE;
	U32 PORT_PUSH_OUTPUT_MODE;
	U32 MODE_MASK;
	U32 FUNCTION_MASK;   
	U32 iTCLK;
	U32 iTD0;
	U32 iTD1;
	U32 iTD2;
	U32 iTD3;
	U32 v;
	U32 PortWidth;
	//
	//JLINK_ExecCommand("TraceSampleAdjust TD=2000"); 
	//
	// Set Trace Portwidth(Optional): Default 4 Pin Trace, other possibilities: 1, 2, 4
	//
	//JLINK_TRACE_PortWidth = 4;
	//
	//   PE12 => TCLK
	//   PE11 => TD0
	//   PE10 => TD1
	//   PE9  => TD2
	//   PE8  => TD3
	//   alternate mode => MFP 14
	//
	// Init register addresses and variables
	//
	SYS_GPE_MFPH = 0x40000054;
	PE_MODE      = 0x10000100;
	iTCLK = 12;
	iTD0  = 11;
	iTD1  = 10;
	iTD2  = 9;
	iTD3  = 8;
	PortWidth = 4; //JLINK_TRACE_PortWidth;
	PORT_PUSH_OUTPUT_MODE = 1;
	MODE_MASK             = 0x3;
	FUNCTION_MASK         = 0xF;
	// Init pins
	// TCLK init
	JLINKARM_ReadMemU32(SYS_GPE_MFPH, 1, &v, NULL);
	v  &= ~(FUNCTION_MASK << (16));                         // Mask alt func register
	v  |= (14 << (16));                                     // Set alt function mode
	JLINKARM_WriteU32(SYS_GPE_MFPH, v);
	JLINKARM_ReadMemU32(PE_MODE, 1, &v, NULL);
	v  &= ~(MODE_MASK << (2 * iTCLK));                      // Mask output mode
	v  |= (PORT_PUSH_OUTPUT_MODE  << (2 * iTCLK));          // Set output mode to output push_pull
	JLINKARM_WriteU32(PE_MODE, v);
	//
	// TD0 init
	//
	JLINKARM_ReadMemU32(SYS_GPE_MFPH, 1, &v, NULL);
	v  &= ~(FUNCTION_MASK << (12));                         // Mask alt func register
	v  |= (14 << (12));                                     // Set alt function mode
	JLINKARM_WriteU32(SYS_GPE_MFPH, v);                                                                          
	JLINKARM_ReadMemU32(PE_MODE, 1, &v, NULL);                       
	v  &= ~(MODE_MASK << (2 * iTD0));                       // Mask output mode
	v  |= (PORT_PUSH_OUTPUT_MODE << (2 * iTD0));            // Set output mode to output push_pull
	JLINKARM_WriteU32(PE_MODE, v);
	//
	// TD1 init
	//
	if (PortWidth > 1) {
		JLINKARM_ReadMemU32(SYS_GPE_MFPH, 1, &v, NULL);
		v  &= ~(FUNCTION_MASK << (8));                          // Mask alt func register
		v  |= (14 << (8));                                      // Set alt function mode
		JLINKARM_WriteU32(SYS_GPE_MFPH, v);
		JLINKARM_ReadMemU32(PE_MODE, 1, &v, NULL);
		v  &= ~(MODE_MASK << (2 * iTD1));                       // Mask output mode
		v  |= (PORT_PUSH_OUTPUT_MODE  << (2 * iTD1));           // Set output mode to output push_pull
		JLINKARM_WriteU32(PE_MODE, v);
	}
  // TD2 & TD3 init
	if (PortWidth > 2) {
		// TD2 init
		JLINKARM_ReadMemU32(SYS_GPE_MFPH, 1, &v, NULL);
		v  &= ~(FUNCTION_MASK << (4));                      // Mask alt func register
		v  |= (14 << (4));                                  // Set alt function mode
		JLINKARM_WriteU32(SYS_GPE_MFPH, v);                                                               
		JLINKARM_ReadMemU32(PE_MODE, 1, &v, NULL);
		v  &= ~(MODE_MASK << (2 * iTD2));                   // Mask output mode
		v  |= (PORT_PUSH_OUTPUT_MODE << (2 * iTD2));        // Set output mode to output push_pull
		JLINKARM_WriteU32(PE_MODE, v);
		// TD3 init
		JLINKARM_ReadMemU32(SYS_GPE_MFPH, 1, &v, NULL);
		v  &= ~(FUNCTION_MASK);                             // Mask alt func register
		v  |= (14);                                         // Set alt function mode
		JLINKARM_WriteU32(SYS_GPE_MFPH, v);                                                                 
		JLINKARM_ReadMemU32(PE_MODE, 1, &v, NULL);         
		v  &= ~(MODE_MASK << (2 * iTD3));                   // Mask output mode
		v  |= (PORT_PUSH_OUTPUT_MODE << (2 * iTD3));        // Set output mode to output push_pull
		JLINKARM_WriteU32(PE_MODE, v);
	}
}

void OneTimeTraceStart(){
	#if (DEVICE == 0)
    	#if (BOARD == 0)
			STM32F407VE_OneTimeTraceStart();
		#elif (BOARD == 1)
			MK64FN1M0XXX12_OneTimeTraceStart();
		#endif
	#elif (DEVICE == 1)
		M2351KIAAE_OneTimeTraceStart();
	#endif
}

// void ConfigTraceRegs(void){
// 	// JLINKARM_TIF_Select(JLINKARM_TIF_SWD);
// 	// JLINKARM_SetSpeed(4000);
// 	// JLINKARM_ClrRESET();
// 	// _Sleep(5);
// 	// JLINKARM_WriteU32(0xE000EDFC, 0x01000000);    // DEMC_R : set TRCENA : otherwise, TPIU registers are not accessible
// 	// JLINKARM_WriteU32(0xE00400F0, 0x00000000);    // TPIU/SPP : select SYNC PORT Mode
// 	// JLINKARM_WriteU32(0xE0040004, 0x00000008);    // TPIU/CPS : select PORT SIZE=4
// 	// JLINKARM_WriteU32(0xE0041FB0, 0xC5ACCE55);    // ETM Unlock Lock Access register
// 	// JLINKARM_WriteU32(0xE0041000, 0x00003d00);    // Set Programming bit (b10)
// 	// JLINKARM_WriteU32(0xE00411E0, 0x00000100);    // Outputs a sync every 256 cycles
// 	// JLINKARM_WriteU32(0xE0041008, 0x0000006F);    // ETM Trigger Event Register, start trace unconditionally
// 	// JLINKARM_WriteU32(0xE004101C, 0x00000000);    // ETM TraceEnable Control2 Register // this register not exist in cortex-m4
// 	// JLINKARM_WriteU32(0xE0041020, 0x0000006F);    // ETM TraceEnable Event Register
// 	// JLINKARM_WriteU32(0xE0041024, 0x01000000);    // ETM TraceEnable Control1 Register
// 	// JLINKARM_WriteU32(0xE00411F0, 0x00000001);    // ETM Trace Start/Stop
// 	// JLINKARM_WriteU32(0xE00411F4, 0x00000001);    // ETM EmbeddedICE Behavior Register
// 	// JLINKARM_WriteU32(0xE0041000, 0x00003c00);    // ETM Control Register: Port mode 001: 2:1; Cycle acurate; ETMEN, branch output; Portsize 4 bit; Access: No data tracing
// 	// JLINKARM_WriteU32(0xE0041000, 0x00003800);    // ETM Control Register: Port mode 001: 2:1; Cycle acurate; ETMEN, branch output; Portsize 4 bit; Access: No data tracing
// 	// JLINKARM_WriteU32(0xE0041200, TRACE_ID);      // CoreSight Trace ID register
// 	// JLINKARM_WriteU32(0xE0042004, 0x000000E0);    // DBG_MCU_CR : assign SYNC TRACE I/Os for SIZE=4
// 	// _Sleep(5);
// 	// JLINKARM_SetRESET();
// 	// JLINKARM_Halt();
// 	JLINKARM_WriteU32(DEMCR, 0x01000000);
// 	JLINKARM_WriteU32(TPIU_SPPR, 0x00000000);
// 	JLINKARM_WriteU32(TPIU_CSPSR, 0x00000008);
// 	JLINKARM_WriteU32(ETMLAR, UNLOCK_VALUE);
// 	JLINKARM_WriteU32(ETMCR, 0x00003d00);
// 	JLINKARM_WriteU32(ETMSYNCFR, 0x00000100);
// 	JLINKARM_WriteU32(ETMTRIGGER, 0x0000006F);
// 	// lack 0xE004101C
// 	JLINKARM_WriteU32(ETMTEEVR, 0x0000006F);
// 	JLINKARM_WriteU32(ETMTECR1, 0x01000000);
// 	JLINKARM_WriteU32(ETMTESSEICR, 0x00000001);
// 	// lack 0xE00411F4
// 	JLINKARM_WriteU32(ETMCR, 0x00003c00);
// 	JLINKARM_WriteU32(ETMCR, 0x00003800);
// 	JLINKARM_WriteU32(ETMTRACEIDR, ETMTRACEIDR_ID);
// 	// lack 0xE0042004
// }

void ConfigTraceRegs(void) {
	U32 tmp_reg = 0x0;
	// ETM CONFIG
	// unlock 
	JLINKARM_WriteU32(ETMLAR, UNLOCK_VALUE);
	// JLINKARM_ReadMemU32(0xE0041FB4, 1, &tmp_reg, NULL); // make sure write successfully

	// Enable power
	JLINKARM_ReadMemU32(ETMCR, 1, &tmp_reg, NULL);
	if (tmp_reg & 1) {
		tmp_reg &= ~1;
		JLINKARM_WriteU32(ETMCR, tmp_reg);
		// JLINKARM_ReadMemU32(ETMCR, 1, &tmp_reg, NULL);
	}

	// Set programming bit [10]
	if ((tmp_reg & 0x400) == 0) {
		tmp_reg |= 0x400;
		JLINKARM_WriteU32(ETMCR, tmp_reg);
		// tmp_reg = 0;
		// JLINKARM_ReadMemU32(ETMCR, 1, &tmp_reg, NULL);
	}
	// ETM branch output[8]
	// ETM output enable [11]
	tmp_reg |= 0x900;
	JLINKARM_WriteU32(ETMCR, tmp_reg);
	// JLINKARM_ReadMemU32(ETMCR, 1, &tmp_reg, NULL);

	// Enable trace
	JLINKARM_WriteU32(ETMTEEVR, ETMTEEVR_TRUE);
	// tmp_reg = 0;
	// JLINKARM_ReadMemU32(ETMTEEVR, 1, &tmp_reg, NULL);

	// config trace id
	JLINKARM_WriteU32(ETMTRACEIDR, ETMTRACEIDR_ID);
	// JLINKARM_ReadMemU32(ETMTRACEIDR, 1, &tmp_reg, NULL);

	// Clear programming bit [10]
	tmp_reg &= 0xFFFFFBFF;
	JLINKARM_WriteU32(ETMCR, tmp_reg);

	// TPIU CONFIG
	// Global enable for all DWT and ITM features 
	// M4 doesn't need this, but demo from segger of M3 use this
	JLINKARM_ReadMemU32(DEMCR, 1, &tmp_reg, NULL);
	if((tmp_reg & 0x1000000) == 0){ //TRCENA, bit[24]
		tmp_reg |= 0x1000000;
		JLINKARM_WriteU32(DEMCR, tmp_reg);
	}
	// config parallel port size
	JLINKARM_WriteU32(TPIU_CSPSR, TPIU_CSPSR_PORT_SIZE);
	// JLINKARM_ReadMemU32(TPIU_CSPSR, 1, &tmp_reg, NULL);
	// 
	JLINKARM_WriteU32(TPIU_SPPR, TPIU_SPPR_PIN_PROTOCOL);
	// JLINKARM_ReadMemU32(TPIU_SPPR, 1, &tmp_reg, NULL);
}
void STRACE_Config(){
	U32 tmp_reg = 0x0;
	JLINKARM_WriteU32(ETMLAR, UNLOCK_VALUE);
	JLINKARM_ReadMemU32(ETMCR, 1, &tmp_reg, NULL);
	if ((tmp_reg & 0x400) == 0) {
		tmp_reg |= 0x400;
		JLINKARM_WriteU32(ETMCR, tmp_reg);
	}
	tmp_reg |= 0x100;
	JLINKARM_WriteU32(ETMCR, tmp_reg);
	JLINKARM_WriteU32(ETMTRACEIDR, ETMTRACEIDR_ID);
	tmp_reg &= 0xFFFFFBFF;
	JLINKARM_WriteU32(ETMCR, tmp_reg);
}
int StartTrace(){
	// if(JLINK_STRACE_Start()<0){
	if(JLINKARM_RAWTRACE_Control(JLINKARM_RAWTRACE_CMD_START, NULL) < 0){
		AFL_ERROR_OUT("Fail to start trace.");
		return -1;
	}
	// STRACE_Config();
	usleep(1);
	return 0;
}

int StopTrace(){
	// if(JLINK_STRACE_Stop() < 0){
	if(JLINKARM_RAWTRACE_Control(JLINKARM_RAWTRACE_CMD_STOP, NULL) < 0){
		AFL_ERROR_OUT("Fail to stop trace.");
		return -1;
	}
	usleep(1);
	return 0;
}