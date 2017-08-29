/*==========================================================================*\
|                                                                            |
| Replicator430X.c                                                           |
|                                                                            |
| JTAG Replicator for the MSP430X Flash-based family devices                 |
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
|             and:      Code Composer Essentials 2.0                         |
|----------------------------------------------------------------------------|
| Author:               FRGR                                                 |
| Version:              2.2                                                  |
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
| 2.1 05/12 RL          Updated commentaries                                 |
| 2.2 03/13 RL/MD       Added InfoA unlock                                   |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file Replicator430X.c
//! \brief JTAG Replicator for the MSP430X Flash-based family.
//! \author Robert Lessmeier
//! \date 03/14/2013
//! \version 2.2

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
/* INCLUDES                                                                 */
/****************************************************************************/

#include "JTAGfunc430X.h"      // JTAG functions
#include "LowLevelFunc430X.h"  // low level functions
#include "Devices430X.h"       // holds Device specific information
#include "Target_Code.h"       // holds declarations and/or data of target program code

/*===============================================================================================
  Name         :  initProgrammer
  Description  :  Initialize programmer.
                  This function setup input/output pins of host and display GREEN LED.
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
  Name         :  eraseTarget
  Description  :  
  Argument(s)  :  None.
  Return value :  None.
================================================================================================*/
int eraseTarget(void)
{
	unsigned char buffer[2];
	
	// Initialize host MSP430 (on Replicator board) & target board
	initProgrammer();
	
	// Connect to the target device    
    if (GetDevice() != STATUS_SUCCESS)         	// Set DeviceId
    {
		ReleaseDevice(V_RESET);
        ShowStatus(STATUS_ERROR, 1);      		// stop here if invalid JTAG ID or		
		return STATUS_ERROR;
    }

	// Mass-Erase Flash (all types)
	EraseFLASH(ERASE_MASS, 0xFE00);

    if (!EraseCheck(0xFE00, 0x0100))      // Check part of main memory erasure (Fxx2..9)
    {
		ReleaseDevice(V_RESET);
        ShowStatus(STATUS_ERROR, 4);

        buffer[0] = STATUS_ERASE_FAILED;
        cdcSendDataInBackground((BYTE*)buffer, 2, CDC0_INTFNUM, 20);
		__delay_cycles(1000);
        return STATUS_ERROR;
    }

    buffer[0] = STATUS_ERASE_SUCCESS;
    cdcSendDataInBackground((BYTE*)buffer, 2, CDC0_INTFNUM, 20);
	__delay_cycles(1000);

	
	// Release the target device from JTAG control    
    ReleaseDevice(V_RESET);         // Perform Reset, release CPU from JTAG control
                                    // Target board LED should start blinking
    ShowStatus(STATUS_SUCCESS, 0);       // OK: green LED is ON
	return STATUS_SUCCESS;
}


//! \brief The basic Replicator routine
//! \details This function is executed once at startup
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

//! \brief The basic Replicator routine
//! \details This function is executed once at startup and can be restarted 
//! by pressing button "S1" on the REP430F board.
int runProgramm(void)
{
    //! \brief Data pointer
    word p;
    //! \brief Buffer, used for memory read with ReadMemQuick()
    word ReadArray[0x40];

/*------------------------------------------------------------------------------------------------------*/
/*  1. | Initialize host MSP430 (on Replicator board) & target board                                    */
/*------------------------------------------------------------------------------------------------------*/    
    
    InitController();                     // Initialize the host MSP430F5437
    
    ShowStatus(STATUS_SUCCESS, 0);         // Switch both LEDs on to indicate operation.

    InitTarget();                         // Initialize target board

/*------------------------------------------------------------------------------------------------------*/
/*  2. | Connect to the target device                                                                   */
/*------------------------------------------------------------------------------------------------------*/    
    
    if (GetDevice() != STATUS_SUCCESS)         // Set DeviceId
    {
        ShowStatus(STATUS_ERROR, 1);      // stop here if invalid JTAG ID or
    }                                     // time-out. (error: red LED is ON)
    
/*------------------------------------------------------------------------------------------------------*/
/*  3. | Control the target peripherals directly                                                        */
/*------------------------------------------------------------------------------------------------------*/

    // Remove the following comments to toggle Pin 1.0 (i.e flash the LED for MSP430 target socket boards)
    /*{
        word k;
        
        WriteMem(F_BYTE, 0x21, 0x01);         // P1.0 for F1xx,2xx devices
        WriteMem(F_BYTE, 0x31, 0x02);         // P5.1 for F4xx devices
    
        for(k = 0; k < 3; k++)
        {
            WriteMem(F_BYTE, 0x22, 0x01);
            WriteMem(F_BYTE, 0x32, 0x02);
            MsDelay(500);                     // LED on for 0.5s
            WriteMem(F_BYTE, 0x21, 0x00);
            WriteMem(F_BYTE, 0x31, 0x00);
            MsDelay(500);                     // LED off for 0.5s    
        }
    }*/

/*------------------------------------------------------------------------------------------------------*/
/*  4. | Perform a write + read + verify in the target RAM                                              */
/*------------------------------------------------------------------------------------------------------*/

    // Communication test: Write 2 RAM bytes
    WriteMem(F_BYTE, 0x0200, 0x34);
    WriteMem(F_BYTE, 0x0201, 0x12);
    // Read back word
    if (ReadMem(F_WORD, 0x0200) != 0x1234)
    {
        ShowStatus(STATUS_ERROR, 5);
    }


    // Write RAM word 
    WriteMem(F_WORD, 0x0202, 0x5678);
    // Read back 2 bytes
    if (ReadMem(F_BYTE, 0x0202) != 0x78)
    {
        ShowStatus(STATUS_ERROR, 6);
    }
    if (ReadMem(F_BYTE, 0x0203) != 0x56)
    {
        ShowStatus(STATUS_ERROR, 7);
    }

    // Is Read/WriteQuick possible for current device?
    if (DeviceHas_DataQuick())
    {
        // Write RAM block
        WriteMemQuick (0x0200, 0x0010, (word*)&eprom[0]);
        // Verify(PSA) RAM of target
        if (!VerifyMem(0x0200, 0x0010, (word*)&eprom[0]))
        {
            ShowStatus(STATUS_ERROR, 8);
        }
        // Read RAM block
        ReadMemQuick  (0x0200, 0x0010, &ReadArray[0]);
        // Verify(word-for-word) RAM of target
        for (p = 0; p < 0x0010; p++)
        {
            if (ReadArray[p] != eprom[p])
            {
                ShowStatus(STATUS_ERROR, 9);
            }
        }
    }

/*------------------------------------------------------------------------------------------------------*/
/*  5. | Operations in the device's main memory                                                         */
/*------------------------------------------------------------------------------------------------------*/
    
    // The following section is not required and included only for reference as to the
    // manner in which a single flash memory segment can be erased. The more common
    // "mass erase" is used to prepare the target device for replicator programming.
    /*{
        // Segment 0 erase Flash (all types)
        EraseFLASH(ERASE_SGMT, 0xFE00);
        // Check segment 0 memory erasure
        if (!EraseCheck(0xFE00, 0x0100))
        {
            ShowStatus(STATUS_ERROR, 2);
        }
    }*/  

    // Perform a mass erase  
    if (DeviceHas_CpuX())
    {
        EraseFLASH(ERASE_GLOB, 0xFE00);     // Global-Erase Flash
    }                                       // (for all devices with CPU-X)
    else
    {
        EraseFLASH(ERASE_MASS, 0xFE00);     // Mass-Erase Flash (all types)
        // NOTE: the INFO memory in F2xx device will be not erased,
        // if the memory is locked. For more info See EraseFLASH() and
        // UnlockInfoA_430x() in JTAGfunc430.c
    }

    // Check main memory erasure
    if (!EraseCheck(0xF800, 0x0400))
    {
        ShowStatus(STATUS_ERROR, 3);
    }  

    // For all 1xx & 4xx & 2xx devices, where ALL Flash has been erased
    EraseFLASH(ERASE_SGMT, 0xFE00);
    // Check part of main memory erasure (Fxx2..9)
    if (!EraseCheck(0xfe00, 0x0100))
    {
        ShowStatus(STATUS_ERROR, 4);
    }

    // For all 2xx devices, where Info A was not erased (Info A keeps calibration data)
    /*
    // Check info memory erasure (Fxx2..9)
    if (!EraseCheck(0x1000, 0x0080))
    {  
        ShowStatus(STATUS_ERROR, 4);
    }
    */

    // Following section shows how to erase Info-Segments on 2xx Devices selectively
    {
        // Comment-in the following code to unlock Info memory segment A
        // WARNING: Info A may include calibration data!
        /*{
            // Info A unlock
            UnlockInfoA();

            // Info A erase
            EraseFLASH(ERASE_SGMT, 0x10C0);
            // Check Info A memory erasure (2xx)
            if (!EraseCheck(0x10C0, 0x0020))
            {
                ShowStatus(STATUS_ERROR, 2);
            }
        }*/
      
        // Info B erase (2xx)
        EraseFLASH(ERASE_SGMT, 0x1080);
        // Check Info B memory erasure (2xx)
        if (!EraseCheck(0x1080, 0x0020))
        {
            ShowStatus(STATUS_ERROR, 2);
        }
        
        // Info C erase (2xx)
        EraseFLASH(ERASE_SGMT, 0x1040);
        // Check Info C memory erasure (2xx)
        if (!EraseCheck(0x1040, 0x0020))
        {
            ShowStatus(STATUS_ERROR, 2);
        }
        
        // Info D erase (2xx)
        EraseFLASH(ERASE_SGMT, 0x1000);
        // Check Info D memory erasure (2xx)
        if (!EraseCheck(0x1000, 0x0020))
        {
            ShowStatus(STATUS_ERROR, 2);
        }
    }

    // Program target code
    if (!WriteFLASHallSections(&eprom[0], &eprom_address[0], &eprom_length_of_sections[0], eprom_sections))
    {
        ShowStatus(STATUS_ERROR, 10);
    }
    
/*------------------------------------------------------------------------------------------------------*/
/*  6. | Blow the JTAG access protection fuse                                                           */
/*------------------------------------------------------------------------------------------------------*/
     
    // Remove following comments to enable the JTAG fuse blow routine.
    // This makes the MSP430 device permanently inaccessible via JTAG

    /*if (!BlowFuse())              // ***Action is permanent***
    {
        ShowStatus(STATUS_ERROR, 15);
    }*/

/*------------------------------------------------------------------------------------------------------*/
/*  7. | Release the target device from JTAG control and wait for the user to press button "S1"         */
/*------------------------------------------------------------------------------------------------------*/
    
    ReleaseDevice(V_RESET);         // Perform Reset, release CPU from JTAG control
                                    // Target board LED should start blinking
    ShowStatus(STATUS_SUCCESS, 0);       // OK: green LED is ON
	
	return STATUS_SUCCESS;
}


/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
