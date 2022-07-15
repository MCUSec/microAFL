#ifndef __INIT_H__
#define __INIT_H__
#include "JLINK_API.h"
/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  U32 HostIF;                 // Host interface used to connect to J-Link. 1 = USB, 2 = IP
  U32 TargetIF;               // See JLINKARM_Const.h "Interfaces" for valid values
  U32 SerialNo;               // Serial number of J-Link we want to connect to via USB
  U32 Speed;                  // Target interface speed in kHz
  const char* sHost;          // Points to the IPAddr / nickname of the J-Link we want to connect to.
  const char* sSettingsFile;  // Points to J-Link settings file to store connection settings
  const char* sDevice;        // Target device J-Link is connected to
} INIT_PARAS;
int _InitConfig();
int _InitDebugSession(INIT_PARAS* pParas);
#endif
