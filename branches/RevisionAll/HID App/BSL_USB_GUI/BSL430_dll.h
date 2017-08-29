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
#ifdef __cplusplus
extern "C" {
#endif

#include "BSL_Definitions.h"
#include <windows.h>

#define FAMILY_5438  1
#define FAMILY_ROM   2
#define FAMILY_FLASH 3

#define COM_UART 1
#define COM_USB  2

// BSL operation was successful
#define BSL_SUCCESS           0x00
// BSL Acknowledged
#define BSL_ACK               0x00
// BSL failed
#define BSL_FAILED            0x01
// BSL could not find any devices
#define BSL_DEVICE_NOT_FOUND  0x01
// BSL failed to register USB HID with Windows Handle
#define BSL_USB_HANDLE_ERROR  0x06

// CRC Error Codes
// CRC passes
#define CRC_SUCCESS           0x00
// CRC failed
#define CRC_FAILED            -1

/* #defines for TI I/O File Operation */
// Successful in opening file
#define OPERATION_SUCCESSFUL  0
// File is at EOF 
#define TXT_EOF               -1
// Error found during opening
#define ERROR_OPENING_FILE    -2
// No data being read yet
#define NO_DATA_READ          -3



/* =========================================================================
*Function:    BSL_close_BSL
*Valid Mode:  USB
*Description: This command closes the USB BSL port.
*Parameters: 
*             none
*Returns:
*             unsigned int      BSL_SUCCESS          - Success
*                               BSL_DEVICE_NOT_FOUND - Device not found
 * ========================================================================= */
extern unsigned char BSL_close_BSL( void );

/* =========================================================================
*Function:    BSL_initialize_BSL
*Valid Mode:  ROM, 5438, Flash
*Description: This command invokes the BSL. For UART, it initializes based on
*             COM port and the device into BSL mode. For USB, it
*             initializes the HID based on a VID or PID. If no VID and PID
*             supplied, a default TI VID and PID is used
*Parameters: 
*             com               UART - BSL_initialize_BSL("COMx");
*                               USB  - BSL_initialize_BSL("");  // Uses default TI VID and PID
*                                      BSL_initialize_BSL("VID=0x2056,PID=0x201,DEVICEINDEX=0"); // Uses explicit VID and PID
*Returns:
*             unsigned int      BSL_SUCCESS          - Success
*                               BSL_DEVICE_NOT_FOUND - Device not found
 * ========================================================================= */
extern unsigned char BSL_initialize_BSL( void);

/* =========================================================================
*Function:    BSL_RegisterUSBDeviceNotification
*Valid Mode:  USB
*Description: Registers USB BSL devices for automatic event notification when a
*             device is removed or inserted.
*Parameters: 
*             HWND hWnd         Current GUI Windows handle
*Returns:
*             unsigned char     BSL_SUCCESS           - Success
*                               BSL_USB_HANDLE_ERROR  - HID device handle error
 * ========================================================================= */
extern unsigned char BSL_RegisterUSBDeviceNotification(HWND hWnd);

/* =========================================================================
*Function:    BSL_UnRegisterUSBDeviceNotification
*Valid Mode:  USB
*Description: Unregisters USB device for Windows notification
*Parameters: 
*             none
*Returns:
*             unsigned char     BSL_SUCCESS           - Success
*                               BSL_USB_HANDLE_ERROR  - HID device handle error
 * ========================================================================= */
extern unsigned char BSL_UnRegisterUSBDeviceNotification(void);

/* =========================================================================
*Function:    BSL_NumOfUSBDevices
*Valid Mode:  USB
*Description: Search for number of USB devices based on a VID and PID
*Parameters: 
*             VID               USB Vendor ID 
*             PID               USB Product ID
*Returns:
*             unsigned long     Number of USB devices
 * ========================================================================= */
extern unsigned long BSL_NumOfUSBDevices(unsigned short VID, unsigned short PID);


/* =========================================================================
*Function:    BSL_USB_SendData
*Description: 
*             
*Parameters: 
*             
*             
*Returns:
*             
*            
 * ========================================================================= */
extern void BSL_USB_SendData(unsigned char* buf, unsigned int num_bytes );


/* =========================================================================
*Function:    BSL_USB_ReceiveData
*Description: 
*             
*Parameters: 
*             
*             
*Returns:
*             
*             
 * ========================================================================= */
extern int BSL_USB_ReceiveData(unsigned char* buf);

/* =========================================================================
*Function:    BSL_USB_ReceiveData
*Description: 
*             
*Parameters: 
*             
*             
*Returns:
*             
*             
 * ========================================================================= */
extern BYTE BSL_FlushBuffer(void);


#ifdef __cplusplus
}
#endif