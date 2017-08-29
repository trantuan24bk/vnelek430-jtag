/*==========================================================================*\
|                                                                            |
| Devices430X.h                                                              |
|                                                                            |
| Device Function Prototypes and Definitions for FLASH programming.          |
|                                                                            |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               LUT                                                  |
| Version:              1.2                                                  |
| Initial Version:      02-08-06                                             |
| Last Change:          06-18-12                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 02/06 LUT         Initial version.                                     |
| 1.1 04/07 WLUT        Enhanced with adress ranges for all actual silica    |
| 1.2 06/12 RL          Updated commentaries                                 |
|----------------------------------------------------------------------------|
| Designed 2005 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file Devices430X.h
//! \brief Device Function Prototypes and Definitions for FLASH programming.
//! \author Robert Lessmeier
//! \date 06/18/2012
//! \version 1.2

/****************************************************************************/
/* Defines                                                                  */
/****************************************************************************/

#ifndef _DEVICES_H_
#define _DEVICES_H_

#ifndef __BYTEWORD__
#define __BYTEWORD__
typedef unsigned int  word;
typedef unsigned char byte;
#endif

typedef unsigned char  bool;
#define TRUE          1
#define FALSE         0

// Constants for flash erasing modes
//! \brief Constant for flash erase: main & info of ALL      mem arrays
#define ERASE_GLOB                 0xA50E
//! \brief Constant for flash erase: main        of ALL      mem arrays
#define ERASE_ALLMAIN              0xA50C
//! \brief Constant for flash erase: main & info of SELECTED mem arrays
#define ERASE_MASS                 0xA506
//! \brief Constant for flash erase: main        of SELECTED mem arrays
#define ERASE_MAIN                 0xA504
//! \brief Constant for flash erase: SELECTED segment
#define ERASE_SGMT                 0xA502

/****************************************************************************/
/* Function prototypes                                                      */
/****************************************************************************/

void SetDevice(word wDeviceId);
bool DeviceHas_TestPin(void);
bool DeviceHas_CpuX(void);
bool DeviceHas_DataQuick(void);
bool DeviceHas_FastFlash(void);
bool DeviceHas_EnhVerify(void);
bool DeviceHas_JTAG(void);
bool DeviceHas_SpyBiWire(void);
word Device_RamStart(void);
word Device_RamEnd(void);
word Device_MainStart(void);

#endif
