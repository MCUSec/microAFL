/*************************************************************************************************************************************
The following prototypes, types, defines have been taken from the SEGGER J-Link SDK with SEGGER's prior written consent.
*************************************************************************************************************************************/
#ifndef JLINK_API_H            //  Avoid multiple inclusion
#define JLINK_API_H

#define   U8    unsigned char
#define   U16   unsigned short
#if (defined (__SES_ARM) || defined (__ICCARM__))
  #define   U32   unsigned long
#else
  #define   U32   unsigned int
#endif
#define   I8    signed char
#define   I16   signed short
#if (defined (__SES_ARM) || defined (__ICCARM__))
  #define   I32   signed long
#else
  #define   I32   signed int
#endif

#ifdef _MSC_VER
  #define U64   unsigned __int64
  #define I64   signed __int64
  #if _MSC_VER <= 1200
    #define U64_C(x) x##UI64
    #define I64_C(x) x##I64
  #else
    #define U64_C(x) x##ULL
    #define I64_C(x) x##LL
  #endif
#else
  #define U64   unsigned long long
  #define I64   signed long long
  #define U64_C(x) x##ULL
  #define I64_C(x) x##LL
#endif

#if (defined(_WIN64) || defined(__LP64__))  // 64-bit symbols used by Visual Studio and GCC, maybe others as well.
  #define PTR_ADDR  U64
#else
  #define PTR_ADDR  U32
#endif

#define STDCALL
typedef void            JLINKARM_LOG                    (const char* sErr);
#define JLINKARM_TIF_SWD               1
#define JLINKARM_HOSTIF_USB   (1 << 0)
#define JLINKARM_HOSTIF_IP    (1 << 1)
#define JLINKARM_RAWTRACE_CMD_START                 0
#define JLINKARM_RAWTRACE_CMD_STOP                  1
//
// ARM_REG
//
typedef enum {
  ARM_REG_R0,                         // Index  0
} ARM_REG;


#define JLINKARM_CM4_REG_R15  15

#if defined(__cplusplus)
  extern "C" {                // Make sure we have C-declarations in C++ programs
#endif
//
// DLL functions are exported through def file, so no __declspec(dllexport) is necessary
// Applications usually link against JLink-lib or JLinkARM.lib which dynamically loads DLL functions via GetProcAddress(), so no need for __declspec(dllimport)
//
#ifdef _WIN32
  #define JLINK_API
  #define JLINKARMDLL_API
  #define JLINKARMDLL_INIT
#else
  #define JLINK_API         __attribute__((visibility("default")))       
  #define JLINKARMDLL_API   __attribute__((visibility("default")))
  #define JLINKARMDLL_INIT  __attribute__((constructor))    // The Linux way of calling an initialization function when a DLL is loaded into memory
#endif


/*********************************************************************
*
*       API functions
*
**********************************************************************
*/
JLINK_API       int                 STDCALL JLINK_DownloadFile                (const char* sFileName, U32 Addr);
JLINKARMDLL_API void                        JLINKARM_Close                    (void);
JLINKARMDLL_API int                         JLINKARM_Connect                  (void);
JLINKARMDLL_API int                         JLINKARM_ExecCommand              (const char* pIn, char* pOut, int BufferSize);

JLINKARMDLL_API void                        JLINKARM_Go                       (void);
JLINKARMDLL_API char                        JLINKARM_Halt                     (void);
JLINKARMDLL_API char                        JLINKARM_IsOpen                   (void);
JLINKARMDLL_API const char*                 JLINKARM_OpenEx                   (JLINKARM_LOG* pfLog, JLINKARM_LOG* pfErrorOut);

JLINKARMDLL_API int                         JLINKARM_ReadMemU8                (U32 Addr, U32 NumItems, U8*  pData, U8* pStatus);
JLINKARMDLL_API int                         JLINKARM_ReadMemU32               (U32 Addr, U32 NumItems, U32* pData, U8* pStatus);
JLINKARMDLL_API U32                         JLINKARM_ReadReg                  (ARM_REG RegIndex);
JLINKARMDLL_API int                         JLINKARM_Reset                    (void);

JLINKARMDLL_API void                        JLINKARM_SetSpeed                 (U32 Speed);
JLINKARMDLL_API void                        JLINKARM_SetWarnOutHandler        (JLINKARM_LOG * pfWarnOut);
JLINKARMDLL_API char                        JLINKARM_Step                     (void);
JLINKARMDLL_API int                         JLINKARM_WaitForHalt              (int TimeOut);

JLINKARMDLL_API int                         JLINKARM_WriteU8                  (U32 Addr, U8  Data);
JLINKARMDLL_API int                         JLINKARM_WriteU16                 (U32 Addr, U16 Data);
JLINKARMDLL_API int                         JLINKARM_WriteU32                 (U32 Addr, U32 Data);

JLINKARMDLL_API int                         JLINKARM_EMU_SelectByUSBSN        (U32 SerialNo);
JLINKARMDLL_API int                         JLINKARM_EMU_SelectIP             (char* pIPAddr, int BufferSize, U16* pPort);
JLINKARMDLL_API void                        JLINKARM_EMU_SelectIPBySN         (U32 SerialNo);
JLINKARMDLL_API int                         JLINKARM_RAWTRACE_Control         (U32 Cmd, void* pData);
JLINKARMDLL_API int                         JLINKARM_RAWTRACE_Read            (U8* pData, U32 NumBytes);
JLINKARMDLL_API int                         JLINKARM_TIF_Select               (int Interface);

JLINKARMDLL_API int                         JLINKARM_WriteMem                 (U32 Addr, U32 Count, const void* pData);
JLINKARMDLL_API void                        JLINKARM_WriteBits                (void);                           // Used by IAR V4.31A - Do NOT remove

#if defined(__cplusplus)
}     /* Make sure we have C-declarations in C++ programs */
#endif

#endif