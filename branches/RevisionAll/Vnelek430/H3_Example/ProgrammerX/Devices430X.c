/*==========================================================================*\
|                                                                            |
| Devices430X.c                                                              |
|                                                                            |
| The file contains functions to distinguish MSP430 devices concerning       |
| FLASH programming.                                                         |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               STO                                                  |
| Version:              1.3                                                  |
| Initial Version:      02-09-06                                             |
| Last Change:          06-27-12                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 02/06 STO         Initial version.                                     |
| 1.1 04/07 WLUT        Enhanced with adress ranges for all actual silica    |
| 1.2 01/08 WLUT        Updated device feature list with new devices         |
| 1.3 06/12 RL          Updated commentaries                                 |
|----------------------------------------------------------------------------|
| Designed 2005 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file Devices430X.c
//! \brief The file contains functions to distinguish MSP430 devices
//! concerning FLASH programming.
//! \author Florian Berenbrinker
//! \date 06/27/2012
//! \version 1.3

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "Devices430X.h"

/****************************************************************************/
/* VARIABLES                                                                */
/****************************************************************************/

//! \brief This variable holds the device index used to determine family 
//! & part number.
//! \details The ID of device is at memory location 0x0FF0h on all MSP430 
//! flash devices. In this application, the device ID is used to determine 
//! the JTAG pin configuration (dedicated vs. shared) of the target device.
static word DeviceIdx = 0;

//! \brief Declaration of a struct type that holds all necessary device 
//! information 
struct tsDeviceFeatures {
    word Id;
    bool TestPin;
    bool CpuX;
    bool DataQuick;
    bool FastFlash;
    bool EnhVerify;
    bool JTAG;
    bool SpyBiWire;
    word RamStart;
    word RamEnd;
    word MainStart;
};

//! \brief This struct holds the device information for all relevant MSP430 
//! devices with the original or extended CPU architecture
static const struct tsDeviceFeatures sDeviceFeatures[] =
{
//                      TestPin      DataQuick      EnhVerify     SpyBiWire        RamEnd
//                Id       |    CpuX     |  FastFlash  |    JTAG     |    RamStart    |   MainStart
//                 |       |      |      |      |      |     |       |        |       |       |
/* F11x(1)(A)*/ { 0xF112, TRUE,  FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xF000 }, // MSP430F1121A
/* F11x2 */     { 0x1132, TRUE,  FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }, // MSP430F1132
/* F12x(A) */   { 0xF123, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }, // MSP430F123
/* F12x2 */     { 0x1232, TRUE,  FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }, // MSP430F1232
/* F13x
   F14x  */     { 0xF149, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x09FF, 0x1100 }, // MSP430F149
/* F15x
   F16x  */     { 0xF169, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x09FF, 0x1100 }, // MSP430F169
/* F161x */     { 0xF16C, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x1100, 0x24FF, 0x8000 }, // MSP430F1610
/* F20xx */     { 0xF201, TRUE,  FALSE, TRUE , TRUE,  FALSE, TRUE , TRUE  , 0x0200, 0x027F, 0xF800 }, // MSP430F2013
/* F21x1
   F21x2 */     { 0xF213, TRUE,  FALSE, TRUE , TRUE,  FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }, // MSP430F2131
/* F22x2
   F22x4 */     { 0xF227, TRUE,  FALSE, TRUE , TRUE,  TRUE,  TRUE , TRUE  , 0x0200, 0x05FF, 0x8000 }, // MSP430F2274
/* F23x0 */     { 0xF237, TRUE,  FALSE, TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x0200, 0x09FF, 0x8000 }, // MSP430F2370
/* F23x
   F24x
   F24x1
   F2410 */     { 0xF249, FALSE, FALSE, TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x0200, 0x09FF, 0x1100 }, // MSP430F249
/* F241x
   F261x */     { 0xF26F, FALSE, TRUE,  TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x1100, 0x20FF, 0x2100 }, // MSP430F2619
/* F41x */      { 0xF413, FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }, // MSP430F413
/* F42x(x) */   { 0xF427, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }, // MSP430FW427
/* F43x 80p */  { 0xF437, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x05FF, 0xA000 }, // MSP430F437
/* FG43x */     { 0xF439, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x09FF, 0x1100 }, // MSP430FG439
/* F44x
   F43x 100p */ { 0xF449, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x09FF, 0x1100 }, // MSP430F449
/* FG461x */    { 0xF46F, FALSE, TRUE,  TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x1100, 0x20FF, 0x2100 }, // MSP430FG4619
/* GENERIC */   { 0xFFFF, FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xE000 }
};

/****************************************************************************/
/* FUNCTIONS                                                                */
/****************************************************************************/

//! \brief This function accepts a Device ID and 
//! extracts the corresponding device information from the sDeviceFeatures 
//! struct
//! \param[in] wDeviceId Device ID (determined at runtime)
void SetDevice (word wDeviceId)
{
    for(DeviceIdx = 0; DeviceIdx < (sizeof(sDeviceFeatures)/sizeof(*sDeviceFeatures)); DeviceIdx++)
    {
        if(sDeviceFeatures[DeviceIdx].Id == wDeviceId)
        {
              break;
        }
    }
}

//! \brief Function to check if current device has a test pin
//! \return bool True = feature available, False = feature not available
bool DeviceHas_TestPin(void)
{
    return (sDeviceFeatures[DeviceIdx].TestPin);
}

//! \brief Function to check if current device has the extended CPUX
//! \return bool True = feature available, False = feature not available
bool DeviceHas_CpuX(void)
{
    return (sDeviceFeatures[DeviceIdx].CpuX);
}

//! \brief Function to check if current device supports DataQuick
//! \return bool True = feature available, False = feature not available
bool DeviceHas_DataQuick(void)
{
    return (sDeviceFeatures[DeviceIdx].DataQuick);
}

//! \brief Function to check if current device supports FastFlash
//! \return bool True = feature available, False = feature not available
bool DeviceHas_FastFlash(void)
{
    return (sDeviceFeatures[DeviceIdx].FastFlash);
}

//! \brief Function to check if current device supports EnhVerify
//! \return bool True = feature available, False = feature not available
bool DeviceHas_EnhVerify(void)
{
    return (sDeviceFeatures[DeviceIdx].EnhVerify);
}

//! \brief Function to check if current device supports JTAG
//! \return bool True = feature available, False = feature not available
bool DeviceHas_JTAG(void)
{
    return (sDeviceFeatures[DeviceIdx].JTAG);
}

//! \brief Function to check if current device supports SpyBiWire
//! \return bool True = feature available, False = feature not available
bool DeviceHas_SpyBiWire(void)
{
    return (sDeviceFeatures[DeviceIdx].SpyBiWire);
}

//! \brief This function returns the start address of the device's RAM
//! \return word Start address of the target device's RAM memory
word Device_RamStart(void)
{
    return (sDeviceFeatures[DeviceIdx].RamStart);
}

//! \brief This function returns the end address of the device's RAM
//! \return word End address of the target device's RAM memory
word Device_RamEnd(void)
{
    return (sDeviceFeatures[DeviceIdx].RamEnd);
}

//! \brief This function returns the start address of the device's main
//! memory
//! \return word Start address of the target device's main memory
word Device_MainStart(void)
{
    return (sDeviceFeatures[DeviceIdx].MainStart);
}

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
