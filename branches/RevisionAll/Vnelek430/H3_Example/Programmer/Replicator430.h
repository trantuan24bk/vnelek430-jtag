/*==========================================================================*\
|                                                                            |
| Replicator430.c                                                            |
|                                                                            |
| JTAG Replicator for the MSP430 Flash-based family devices                  |
|                                                                            |
| Key features:                                                              |
| � Supports JTAG communication to all MSP430 Flash Devices                  |
| � Max. code size of target program: 57kB                                   |
| � Programming speed: ~60kB/10sec (~6kB/sec)                                |
| � Fast Verification and Erase Check: ~60kB/350ms                           |
| � Supports Programming of JTAG Access Protection Fuse                      |
|                                                                            |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               FRGR                                                 |
| Version:              2.0                                                  |
| Initial Version:      04-17-02                                             |
| Last Change:          03-14-13                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 04/02 FRGR        Initial version.                                     |
| 1.1 04/02 FRGR        Included SPI mode to speed up shifting function by 2.|
| 1.2 06/02 ALB2        Formatting changes, added comments.                  |
| 1.3 06/02 ALB2        DEVICE ID info added.                                |
| 1.4 08/02 ALB2        Initial code release with Lit# SLAA149.              |
| 1.5 01/03 ALB2        Unnecessary lines have been commented out(see below).|
| 1.6 09/05 JDI         converted to IAR EW 3.30. F2xxx support added        |
|           SUN1        Software delays redesigned to use TimerA harware;    |
|                       see MsDelay() routine.                               |
|           ALB2        Added 2xx Info memory B, C, & D erase & check        |
| 1.7 12/05 STO         Adapted for 2xx devices with SpyBiWire               |
| 1.8 02/06 STO         Minor cosmetic changes                               |
| 1.9 05/06 STO         Minor cosmetic changes                               |
| 2.0 04/07 WLUT        Minor changes at declarations and calls              |
|                       according to use srec_cat.exe                        |
| 2.1 07/09 FB          Added loop for polling P1.6 / added support for      |
|                       replicator                                           |
| 2.2 07/12 RL          Updated commentaries                                 |
| 2.3 03/13 RL/MD       Added InfoA unlock                                   |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file Replicator430.c
//! \brief JTAG Replicator for the MSP430 Flash-based family.
//! \author Robert Lessmeier
//! \date 03/14/2013
//! \version 2.3

/****************************************************************************/
/* Main section of Replicator program: User can modify/insert code as needed*/
/****************************************************************************/
/* Main function:
	 Upon execution completion, LED blinks for F1xx & F4xx device target socket
	 boards when used as the target system (P1.0 and P5.1 drive LEDs).

	 Note: All High Level JTAG Functions are applied here.

	 A number of lines have been commented out which are not strictly required
	 to successfully program a flash-based MSP430 device using the Replicator
	 concept. Please note that these lines have been commented out in order
	 to provide reference examples for calling such functions.
	 Uncommented lines below represent the minimum requirements to successfully
	 program an MSP430 flash device using the Replicator.
	 
		The basic routine consists of the following steps:
	 
	 1.  | Initialize the host MSP430 on the Replicator board
	 2.  | Connect to the target device
	 3.  | Control the target peripherals directly (optional)
	 4.  | Perform a write + read + verify in the target RAM
	 5.  | Operations in the device's main memory
	 6.  | Blow JTAG access fuse (optional)
	 7.  | Release the device from JTAG control and wait for the user to press button "S1"
*/

/****************************************************************************/
/* Includes                                                                 */
/****************************************************************************/

#include "JTAGfunc430.h"           // JTAG functions
#include "LowLevelFunc430.h"       // low level functions
#include "Devices430.h"            // holds Device specific information
#include "Target_Code.h"           // holds declarations and/or data of target program code

unsigned char firstWrite = TRUE;


/*===============================================================================================
	Name         :  usbReportStatus
	Description  :  This function send status code to computer application
					using USB HID communication.
	Argument(s)  :  None.
	Return value :  None.
================================================================================================*/
void usbReportStatus(byte status, byte error)
{
	// Send status back to computer
	usbBuffer[0] = status;
	hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

	if(has_error == TRUE)
	{
		// Release the target device from JTAG control    
		ReleaseDevice(V_RESET);
		ShowStatus(STATUS_ERROR, 0);
	}	
}


/*===============================================================================================
	Name         :  initProgrammer
	Description  :  Initialize programmer.
									This function is executed once at startup and before 'startProgrammer'.
	Argument(s)  :  None.
	Return value :  None.
================================================================================================*/
int initProgrammer(void)
{
	// Initialize host MSP430 (on Replicator board) & target board
	InitController();                     		// Initialize the host MSP430F5510
		
	ShowStatus(STATUS_SUCCESS, 0);         	  	// Switch both LEDs on to indicate operation.
		
	InitTarget();                         		// Initialize target board
	
	return STATUS_SUCCESS;
}

/*===============================================================================================
	Name         :  detectTarget
	Description  :  
									
	Argument(s)  :  None.
	Return value :  None.
================================================================================================*/
int detectTarget(void)
{
	word DeviceId;
	byte SubId;
	
	// Initialize host MSP430 (on Replicator board) & target board
	initProgrammer();
	
	if(detectTargetL(&DeviceId, &SubId) != STATUS_SUCCESS)
	{
		// Send error status to computer
		usbReportStatus(STATUS_DETECT_FAILED, TRUE);

		return STATUS_ERROR;
	}
	else
	{
		usbBuffer[1] = DeviceId>>8;
		usbBuffer[2] = DeviceId;
		usbBuffer[3] = SubId;
		
		// Send success status to computer
		usbReportStatus(STATUS_DETECT_SUCCESS, FALSE);
	}
	
	// Release the target device from JTAG control    
	ReleaseDevice(V_RESET);
	ShowStatus(STATUS_SUCCESS, 0);
	
	return STATUS_SUCCESS;
}

/*===============================================================================================
	Name         :  eraseTarget
	Description  :  
									
	Argument(s)  :  None.
	Return value :  None.
================================================================================================*/
int eraseTarget(void)
{
	// Initialize host MSP430 (on Replicator board) & target board
	initProgrammer();
	
	// Connect to the target device    
	if (GetDevice() != STATUS_SUCCESS)
	{
		ReleaseDevice(V_RESET);
		ShowStatus(STATUS_ERROR, 1);

		// Send status back to computer
		usbBuffer[0] = STATUS_DETECT_FAILED;
		hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

		return STATUS_ERROR;
	}

	// Mass-Erase Flash (all types)
	EraseFLASH(ERASE_MASS, 0xFE00);

	// Check part of main memory erasure (Fxx2..9)
		if (!EraseCheck(0xfe00, 0x0100))
		{
		ReleaseDevice(V_RESET);
				ShowStatus(STATUS_ERROR, 4);

		// Send status back to computer
				usbBuffer[0] = STATUS_ERASE_FAILED;
		hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

				return STATUS_ERROR;
		}

		// Send status back to computer
	usbBuffer[0] = STATUS_ERASE_SUCCESS;        
	hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);
	
	// Release the target device from JTAG control    
		ReleaseDevice(V_RESET);
		ShowStatus(STATUS_SUCCESS, 0);
	
	return STATUS_SUCCESS;
}

/*===============================================================================================
	Name         :  writeToTarget
	Description  :  
									
	Argument(s)  :  None.
	Return value :  None.
================================================================================================*/
int writeToTarget(void)
{
	// Initialize host MSP430 (on Replicator board) & target board
	initProgrammer();
	
	// Connect to the target device    
		if (GetDevice() != STATUS_SUCCESS)         // Set DeviceId
		{
		ReleaseDevice(V_RESET);
				ShowStatus(STATUS_ERROR, 1);      // stop here if invalid JTAG ID or		
		return STATUS_ERROR;
		}                                     // time-out. (error: red LED is ON)

	// Write code to FLASH
	WriteFLASH(0xC000, 35, (word*)fdata1);
	WriteFLASH(0xFFF8,  1, (word*)fdata2);
	WriteFLASH(0xFFFE,  1, (word*)fdata3);
	
	if (VerifyMem(0xC000, 35, (word*)fdata1) == STATUS_ERROR)
	{
		ReleaseDevice(V_RESET);
		ShowStatus(STATUS_ERROR, 0);
		return(STATUS_ERROR);
	}
		
	// Release the target device from JTAG control    
		ReleaseDevice(V_RESET);
		ShowStatus(STATUS_SUCCESS, 0);
	
	return(STATUS_SUCCESS);
}


/*===============================================================================================
	Name         :  writeProgramToTarget
	Description  :  
	Argument(s)  :  None.
	Return value :  None.
================================================================================================*/
int writeProgramToTarget(byte* buffer)
{
		unsigned char status, command;
		unsigned int index, i;
		word len, address, crc;
		word codeData[32];
		
	command = buffer[0];								// Read command.	
		len 	= (buffer[1]<<8) | buffer[2];				// Number of bytes in code segment   
		address = (buffer[3]<<8) | buffer[4];				// Get address
		crc 	= (buffer[5]<<8) | buffer[6];				// Get CRC

		// Check CRC. 
		// If error, send re-send request to computer...

		// If len is odd number. 
		// Then, add a zero-byte after data-frame
		if(len%2)
		{        
				buffer[len + START_INDEX] = 0x00;
		len = len + 1;
		}

		// Swap 2-byte and store it in a word
	index = 0x00;
		for(i = START_INDEX; i < (len+START_INDEX); i +=2)
		{
				codeData[index++] = (buffer[i+1]<<8) | (buffer[i]);
		}

		// Length of word = length of byte/2
		len = len>>1;

	if(firstWrite == TRUE)
	{
		firstWrite = FALSE;
		
		// Initialize host MSP430
		initProgrammer();
		
		// Connect to the target device    
		if (GetDevice() != STATUS_SUCCESS)
		{
			// Send status back to computer
			usbBuffer[0] = STATUS_DETECT_FAILED;        
			hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);
	
			ReleaseDevice(V_RESET);
			ShowStatus(STATUS_ERROR, 1);
			return STATUS_ERROR;
		}
	}
	
		// Try to write data to FLASH for 2 times
		i = 2;
		while(i)
		{
				// Write code into target FLASH
				WriteFLASH(address, len , (word*)codeData);

				// Verify memory
				if(VerifyMem(address, len, (word*)codeData) == STATUS_SUCCESS)
						break;

				// Decrease counter
				i = i - 1;
		}

		if(i != 0x00)	/* Write success */
		{
				if(command != CMD_WRITE_END)
				{            
						// IT'S STILL HAS DATA AFTER THIS SEQUENCE...
						// Send a new data request.
			
						// Send status back to computer
			usbBuffer[0] = STATUS_WRITE_SUCCESS;        
			hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

						// Toggle green LED to indicate that programmer is in progress.
						toggleGreenLED();
			
						// End of code sequence.
						return STATUS_SUCCESS;
				}
				else
				{
						// THIS IS END OF DATA.			
			firstWrite = TRUE;
			
						// Send an end of data status.
			usbBuffer[0] = STATUS_WRITE_SUCCESS;        
			hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

						// Perform Reset, release CPU from JTAG control
						ReleaseDevice(V_RESET);
						ShowStatus(STATUS_SUCCESS, 0);

						// End of code sequence.
						return STATUS_SUCCESS;
				}
		}
		else	/* Write failed */
		{
				// Send a failure message to computer
		usbBuffer[0] = STATUS_WRITE_FAILED;        
		hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

				// Perform Reset, release CPU from JTAG control
				ReleaseDevice(V_RESET);
				ShowStatus(STATUS_ERROR, 0);

				return STATUS_ERROR;
		}
}

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
