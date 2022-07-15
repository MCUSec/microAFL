// size of buffer to store raw trace
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h> 

#define TRACE_TO_FILE               1
#if TRACE_TO_FILE
    #define TRACE_LOG_FILE_NAME     "/mnt/ram/tracefile.bin"
    #define RAW_TRACE_FILE          "/mnt/ram/tracefile_0000.bin"    // if more than 2G, the additional files will be generated and name will increment from 0000  
#endif
#define TRACESIZE                   (512*1024*1024)
#define DEBUG_MODE                  0
#define DEBUG_DECODER               0
// #define TIME_EVA 1

#endif