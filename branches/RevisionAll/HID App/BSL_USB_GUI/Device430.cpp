
#include "Device430.h"


//! \brief This function accepts a Device ID and 
//! extracts the corresponding device information from the sDeviceCPU89 
//! struct
//! \param[in] wDeviceId Device ID (determined at runtime)
WORD SetDevice (WORD wDeviceId, BYTE wSubId)
{
	WORD DeviceIdx;
	WORD IdLastMatch = DEVICE_NOT_MATCH;

    for(DeviceIdx = 0; DeviceIdx < (sizeof(sDeviceCPU89)/sizeof(*sDeviceCPU89)); DeviceIdx++)
    {
        if(sDeviceCPU89[DeviceIdx].Id == wDeviceId)
        {
			IdLastMatch = DeviceIdx;
        }

		if((sDeviceCPU89[DeviceIdx].Id == wDeviceId) && (sDeviceCPU89[DeviceIdx].SubId == wSubId))
		{
			return DeviceIdx;
		}
    }
	
	return IdLastMatch;
}

//! \brief Function to check if current device has a test pin
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_TestPin(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].TestPin);
}

//! \brief Function to check if current device has the extended CPUX
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_CpuX(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].CpuX);
}

//! \brief Function to check if current device supports DataQuick
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_DataQuick(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].DataQuick);
}

//! \brief Function to check if current device supports FastFlash
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_FastFlash(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].FastFlash);
}

//! \brief Function to check if current device supports EnhVerify
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_EnhVerify(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].EnhVerify);
}

//! \brief Function to check if current device supports JTAG
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_JTAG(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].JTAG);
}

//! \brief Function to check if current device supports SpyBiWire
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_SpyBiWire(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].SpyBiWire);
}

//! \brief This function returns the start address of the device's RAM
//! \return WORD Start address of the target device's RAM memory
WORD Device_RamStart(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].RamStart);
}

//! \brief This function returns the end address of the device's RAM
//! \return WORD End address of the target device's RAM memory
WORD Device_RamEnd(WORD DeviceIdx)
{
    return (sDeviceCPU89[DeviceIdx].RamEnd);
}

//! \brief This function returns the start address of the device's FLASH
//! \return WORD Start address of the target device's FLASH memory
WORD Device_FlashStart(WORD DeviceIdx)
{
	return (sDeviceCPU89[DeviceIdx].FlashStart);
}

//! \brief This function returns the end address of the device's FLASH
//! \return WORD End address of the target device's FLASH memory
WORD Device_FlashEnd(WORD DeviceIdx)
{
	return (sDeviceCPU89[DeviceIdx].FlashEnd);
}

//! \brief This function returns the start address of the device's INFO
//! \return WORD Start address of the target device's INFO memory
WORD Device_InfoStart(WORD DeviceIdx)
{
	return (sDeviceCPU89[DeviceIdx].InfoStart);
}

//! \brief This function returns the end address of the device's INFO
//! \return WORD End address of the target device's INFO memory
WORD Device_InfoEnd(WORD DeviceIdx)
{
	return (sDeviceCPU89[DeviceIdx].InfoEnd);
}
