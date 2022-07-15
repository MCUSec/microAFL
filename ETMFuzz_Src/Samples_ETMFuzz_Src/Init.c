#include <stdio.h>
#include <string.h>
#include "Init.h"
#include "device.h"
#include "common.h"

INIT_PARAS _Paras;

/********************************************************************* 
* 
*       _LogOut()
* 
* Function description 
*   Print log output to console.
*/
static void _LogOut(const char* sLog) {
  printf("Log: %s\n", sLog);
}

/********************************************************************* 
* 
*       _ErrorOut()
* 
* Function description 
*   Print error output to console.
*/
static void _ErrorOut(const char* sError) {
  printf("\nERROR: %s\n", sError);
}

/********************************************************************* 
* 
*       _InitConfig()
* 
* Function description 
*   Set up the configuration used for the startup sequence.
*   For interactive setup refer to the startup sample.
*/
int _InitConfig() {

  _Paras.sDevice = DEVICE_NAME;
  _Paras.TargetIF = JLINKARM_TIF_SWD;
  _Paras.Speed = JLINK_SPEED;
  _Paras.HostIF = HOST_INTERFACE; //JLINKARM_HOSTIF_IP; //JLINKARM_HOSTIF_USB;
  _Paras.SerialNo = JLINK_SERIAL_NO;
  _Paras.sSettingsFile = "Settings.jlink";
  // _Paras.sHost = JLINK_IPADDR;
  return 0;
}

static void _WarnOutHandler(const char* sError) { 
  return;
}

static void _cbErrorOutHandler(const char* sError) {
  printf("\n\n\n\n");
  printf(sError);
  return;
}

static void _cbLogOutHandler(const char* sLog){
  return;
}

/*********************************************************************
*
*       _InitDebugSession()
*
*  Function description
*    Initializes the debug session by connecting to a J-Link,
*    setting up the J-Link DLL and J-Link and finally connecting to the target system
*
*  Return value
*      0 O.K.
*    < 0 Error
*/
int _InitDebugSession(INIT_PARAS* pParas) {
  const char* sError;
  U8 acIn[0x400];
  U8 acOut[0x400];
  int r;
  //
  // Select and configure host interface (USB is default)
  //
  if (pParas->HostIF == JLINKARM_HOSTIF_IP) {  // Host interface == IP?
    //
    // If sHost is NULL, J-Link selection dialog will pop-up
    // on JLINKARM_Open() / JLINKARM_OpenEx(), showing all
    // emulators that have been found in the network    
    // Passing 0 as port selects the default port (19020).
    //
    // r = JLINKARM_EMU_SelectIP(pParas->sHost, sizeof(JLINK_IPADDR), 0);
    JLINKARM_EMU_SelectIPBySN(pParas->SerialNo);
    printf("JLINKARM_EMU_SelectIP\n");
  //   if (r == 1) {
  //     return -1;  // Error occurred during configuration of IP interface
  //  }
  } else {        // Connect via USB
    //
    // Was a specific serial number set we shall to connect to?
    //
    if (pParas->SerialNo) {
      r = JLINKARM_EMU_SelectByUSBSN(pParas->SerialNo);
      if (r < 0) {
        return -1;    // Error: Specific serial number not found on USB
     }
    }
  }
  //
  // Open communication with J-Link
  //
  // sError = JLINKARM_OpenEx(_LogOut, _ErrorOut);
  // if (sError) {                   // Error occurred while connecting to J-Link?
  //   _ErrorOut(sError);
  //   return -1;
  // }
  JLINKARM_SetWarnOutHandler(_WarnOutHandler);
  JLINKARM_Close();
  sError = JLINKARM_OpenEx(_cbLogOutHandler, _cbErrorOutHandler);
  if(sError){
    _ErrorOut("Fail to open JLink.");
  }
  // if(JLINKARM_IsOpen()==0){
    // if(JLINKARM_Open()){
    //   _ErrorOut("Fail to open JLink.");
    // }
  // }

  //
  // Select settings file
  // Used by the control panel to store its settings and can be used by the user to
  // enable features like flash breakpoints in external CFI flash, if not selectable
  // by the debugger. There should be different settings files for different debug
  // configurations, for example different settings files for LEDSample_DebugFlash
  // and LEDSample_DebugRAM. If this file does not exist, the DLL will create one
  // with default settings. If the file is already present, it will be used to load
  // the control panel settings
  //
  if (pParas->sSettingsFile) {
    strcpy(acIn, "ProjectFile = ");
    strcat(acIn, pParas->sSettingsFile);
    JLINKARM_ExecCommand(acIn, acOut, sizeof(acOut));
    if (acOut[0]) {
      _ErrorOut(acOut);
      return -1;
    }
  }
  //
  // Select device or core
  //
  if (pParas->sDevice) {
    strcpy(acIn, "device = ");
    strcat(acIn, pParas->sDevice);
    JLINKARM_ExecCommand(acIn, &acOut[0], sizeof(acOut));
    if (acOut[0]) {
      _ErrorOut(acOut);
      return -1;
    }
  }

  strcpy(acIn, "scriptfile = ");
  strcat(acIn, SCRIPTEFILE);
  JLINKARM_ExecCommand(acIn, &acOut[0], sizeof(acOut));
  if(acOut[0]) {
    _ErrorOut(acOut);
  }

  #if (DEVICE==1)
    JLINKARM_ExecCommand("CORESIGHT_SetTPIUBaseAddr = 0xE0040000;", &acOut[0], sizeof(acOut));
    JLINKARM_ExecCommand("CORESIGHT_SetETMBaseAddr  = 0xE0041000", &acOut[0], sizeof(acOut));  
  #endif

  
	#ifdef TRACE_DELAY_TIME
	  strcpy(acIn, "TraceSampleAdjust TD = ");
    strcat(acIn, TRACE_DELAY_TIME);
		JLINKARM_ExecCommand(acIn, &acOut[0], sizeof(acOut));
		if(acOut[0]){
			_ErrorOut(acOut);
		}
	#endif

  //
  // Select and configure target interface
  // If not called, J-Link will use the interface which was configured before. If
  // J-Link is power-cycled, JTAG is the default target interface.
  // It is recommended to always select the interface at debug startup.
  //
  JLINKARM_TIF_Select(pParas->TargetIF);
  //
  // Select target interface speed which
  // should be used by J-Link for target communication
  //
  JLINKARM_SetSpeed(pParas->Speed);
  //
  // Connect to target CPU
  //
  r = JLINKARM_Connect();
  if (r) {
    _ErrorOut("Could not connect to target.");
    return -1;
  }
#if TRACE_TO_FILE
  strcpy(acIn, "TraceFile = ");
  strcat(acIn, TRACE_LOG_FILE_NAME);
  JLINKARM_ExecCommand(acIn, &acOut[0], sizeof(acOut));
  if(acOut[0]){
    _ErrorOut(acOut);
  }
#endif

  return 0;
}
