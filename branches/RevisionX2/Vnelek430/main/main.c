/* ==============================================================================
 *  File name:
 *      		main.c
 *	Description:
 * 		
 *
 *  Created on: 
 * 				October 10, 2013
 *  Author: 
 * 				TRAN KHAC TUAN
 *	Address:
 * 		Vnelektronics Co.Ltd
 * 		776 Ton Duc Thang, Lien Chieu, Danang
 *		website: www.vnelek.vn
 *
 * 	Complier: 
 * 			IAR v5.3
 * ==============================================================================*/
 
#include <msp430f5510.h>
#include <intrinsics.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "USB_API/USB_Common/device.h"					// USB's headers
#include "USB_API/USB_Common/types.h"
#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/usb.h"
#include "F5xx_F6xx_Core_Lib/HAL_UCS.h"
#include "F5xx_F6xx_Core_Lib/HAL_PMM.h"
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_App/usbConstructs.h"

#define MAX_USB_RECEIVE		490

#ifndef MCLK_24MHZ
#define MCLK_24MHZ			1
#endif

volatile BYTE bCDCDataReceived_event = FALSE;			// USB receiver flag   
unsigned char hexBuffer[MAX_USB_RECEIVE];   			// USB receive buffer

WORD count 			= 0x00;								// USB transfer status
BYTE ReceiveError 	= 0x00;
BYTE SendError 		= 0x00;	

#include "main.h"										// Main header
#include "Config430Xv2.h"
#include "LowLevelFunc430Xv2.h"
#include "Replicator430Xv2.h"

/*===============================================================================================
  Name         :  main program
  Description  :  
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
void main( void )
{
    WDTCTL = WDTPW + WDTHOLD;						// Stop watchdog timer

    SetVCore(3);									// Initialize VCore
    Init_Clock();               					// Init clocks
    USB_init();               						// Init USB
	
    USB_setEnabledEvents(kUSB_VbusOnEvent + kUSB_VbusOffEvent + 
						kUSB_dataReceivedEvent + kUSB_receiveCompletedEvent + 
						kUSB_UsbSuspendEvent + kUSB_UsbResumeEvent);
    
    if (USB_connectionInfo() & kUSB_vbusPresent)
		USB_handleVbusOnEvent();

    __enable_interrupt();							// Enable global interrupt
    
    while(TRUE)
    {
        switch(USB_connectionState())
        {
			case ST_ENUM_ACTIVE:
				if (bCDCDataReceived_event == TRUE)
				{
					// Clear usb received flag
					bCDCDataReceived_event = FALSE;

					// Read usb data
					count = cdcReceiveDataInBuffer((BYTE*)hexBuffer, MAX_USB_RECEIVE, CDC0_INTFNUM);
					// cdcSendDataInBackground((BYTE*)hexBuffer, count, CDC0_INTFNUM, 20);
					
					runProgram();
					
					// Excute command
					switch(hexBuffer[0])
					{
						// case 0x81:	eraseTarget(); break;
						// case 0x82:	writeToTarget(); break;
						// case 0x83:	detectDevice(); break;
						
						// case CMD_DETECT: detectDevice(); break;						
						// case CMD_ERASE:	eraseTarget(); break;						
						// case CMD_WRITE:	
						// case CMD_WRITE_END:	
								// writeProgramToTarget(&hexBuffer[0]);
								// break;
						default: 	break;
					}
				}
				break;
			default:
				break;
        }

		/* Check USB error */
        if (ReceiveError || SendError)
        {
			USB_reset();
            USB_connect();
        }
    }
}

/*===============================================================================================
  Name         :  UNMI_ISR
  Description  :  
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
	switch (__even_in_range(SYSUNIV, SYSUNIV_SYSBUSIV))
	{
		case SYSUNIV_NONE:
		  __no_operation();
		  break;		  
		case SYSUNIV_NMIIFG:
		  __no_operation();
		  break;
		case SYSUNIV_OFIFG:
		  UCSCTL7 &= ~(DCOFFG+0+0+XT2OFFG); // Clear OSC flaut Flags fault flags
		  SFRIFG1 &= ~OFIFG;                // Clear OFIFG fault flag
		  break;
		case SYSUNIV_ACCVIFG:
		  __no_operation();
		  break;
		case SYSUNIV_SYSBUSIV:
		  SYSBERRIV = 0;            		// Clear bus error flag
		  USB_disable();            		// Disable
	}
}
