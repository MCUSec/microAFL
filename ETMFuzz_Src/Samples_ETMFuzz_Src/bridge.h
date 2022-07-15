// #include <stdio.h>
// #include <string.h>
#ifndef __BRIDGE_H__
#define __BRIDGE_H__
#include "JLINK_API.h"
#include "device.h"

#define DEBUG 1

#define TARGET_INTERFACE JLINKARM_TIF_SWD
#define SERIAL_NUMBER 0
#define SPEED 4000
#define JTRACE_IP "192.168.0.3"


// end reason including BKPT params
#define END_OF_TESTCASE 0
#define CRASH 1
#define TIMEOUT 2

// #define RAWTRACE // STRACE if not define RAWTRACE

void AFL_JLinkInit(char* target_bin);
// void AFL_WriteTestCase(U8* testcase, U32 size);
int AFL_GetBitmap(U8* bitmap, U32 timeout, U8* testcase, U32 size);
#endif