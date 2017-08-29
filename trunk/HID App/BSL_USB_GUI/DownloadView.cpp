/*
 * MSP430 USB Firmware Upgrade Example
 *
 * An example software to field firmware upgrade a MSP430 USB based device
 *
 * Copyright (C) {2010} Texas Instruments Incorporated - http://www.ti.com/ 
 * 
*/
#include "StdAfx.h"
#include "DownloadView.h"
#include <stdio.h>
#include <string.h>


/* =========================================================================
*Function:    RegisterUSBDevice
*Description: This function registers a HID USB device to Windows.
*Parameters: 
*             HWND hWnd
*
*Returns:
*             BYTE              BSL_SUCCESS           - Success
*                               BSL_USB_HANDLE_ERROR  - HID device handle error
*             
 * ========================================================================= */
BYTE BSL_USB_GUI::DownloadView::RegisterUSBDevice(HWND hWnd)
{
  return BSL_RegisterUSBDeviceNotification(hWnd);
}

/* =========================================================================
*Function:    UnRegisterUSBDevice
*Description: This function unregisters a HID USB device to Windows.
*Parameters: 
*             none
*
*Returns:
*             BYTE              BSL_SUCCESS           - Success
*                               BSL_USB_HANDLE_ERROR  - HID device handle error
*             
 * ========================================================================= */
BYTE BSL_USB_GUI::DownloadView::UnRegisterUSBDevice()
{
  return BSL_UnRegisterUSBDeviceNotification();
}



/* =========================================================================
*Function:    StringToChar
*Description: This function parsses String^ to character array
*Parameters: 
*             String^ str               String value to be parssed 
*             BYTE * character          pointer to character array to be constructed
*
*Returns:
*             none
*                                       
 * ========================================================================= */
void BSL_USB_GUI::DownloadView::StringToChar(String^ str, BYTE * character)
{
  for(int i = 0 ; i < str->Length ; i++)
  {
    character[i] = (BYTE) str[i];
  }
}


