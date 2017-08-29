/*
 * MSP430 USB Firmware Upgrade Example
 *
 * An example software to field firmware upgrade a MSP430 USB based device
 *
 * Copyright (C) {2010} Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
#include <stdio.h> 
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "BSL_IO_USB.h"
#include "BSL430_dll.h"
#include "BSL_Definitions.h"
#include "HidDevice.h"

__declspec(dllexport) BYTE BSL_RegisterUSBDeviceNotification(HWND hWnd)
{
  return USB_RegisterForDeviceNotification(hWnd);
}

__declspec(dllexport) BYTE BSL_UnRegisterUSBDeviceNotification(void)
{
  return USB_UnRegisterDeviceNotification();
}

__declspec(dllexport) DWORD BSL_NumOfUSBDevices(unsigned short VID, unsigned short PID)
{
  return USB_numOfDevices(VID,PID);
}

__declspec(dllexport) BYTE BSL_initialize_BSL( void )
{
	return USB_initialize_BSL();
}

__declspec(dllexport) BYTE BSL_close_BSL(void)
{
	return USB_close_BSL();
}


__declspec(dllexport) void BSL_USB_SendData(unsigned char* buf, unsigned int num_bytes )
{
	USB_sendData(buf, num_bytes );
}

__declspec(dllexport) int BSL_USB_ReceiveData(unsigned char* buf)
{
	return USB_receiveData(buf);
}

__declspec(dllexport) BYTE BSL_FlushBuffer(void)
{
	return USB_FlushBuffer();
}


#ifdef __cplusplus
}
#endif