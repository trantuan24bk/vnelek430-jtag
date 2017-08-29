/*==========================================================================*\
|                                                                            |
| Replicator430Xv2.c                                                         |
|                                                                            |
| JTAG Replicator for the MSP430Xv2 Flash-based family (5xx/6xx)             |
| and FRAM-based devices.                                                    |
|                                                                            |
| Key features:                                                              |
| � Supports JTAG communication to all 5xx/6xx MSP430 Flash & FRAM devices   |
| � Max. code size of target program: 57kB                                   |
| � Programming speed: ~60kB/10sec (~6kB/sec)                                |
| � Fast Verification and Erase Check: ~60kB/350ms                           |
| � Supports Device Access with user set JTAG password (FRAM only)           |
| � Supports deactivation of the Memory Protection Unit (FRAM only)          |
| � Supports Programming of JTAG Access Protection Fuse                      |
|                                                                            |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 4.11B [Kickstart]             |
|             and:      Code Composer Essentials v3                          |
|----------------------------------------------------------------------------|
| Author:               WLUT                                                 |
| Version:              1.6                                                  |
| Initial Version:      04/25/07                                             |
| Last Change:          03/14/13                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 08/08 WLUT        Initial version.                                     |
| 1.1 06/09 FB          add Loop for 'Go'Button P1.6                         |
| 1.2 06/09 FB          updated Constants for flash erasing modes &          |
|                       added section with "erase the all Flash memory banks |
|                       in main memory and verify them.                      |
| 1.3 10/10 FB          update code examples for F6638 family, F5509 family  |
|                       and F5132 Family                                     |
| 1.4 04/12 RL          updated commentaries                                 |
| 1.5 11/12 RL          Fixed erase main memory flow                         |
| 1.6 03/13 RL/MD       Added InfoA/BSL unlock                               |
|----------------------------------------------------------------------------|
| Designed 2008 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file Replicator430Xv2.c
//! \brief JTAG Replicator for the MSP430Xv2 Flash-based family (5xx/6xx) and FRAM-based devices.
//! \author Robert Lessmeier
//! \date 03/14/2013
//! \version 1.6

/****************************************************************************/
/* Main section of Replicator program: User can modify/insert code as needed*/
/****************************************************************************/
/* Main function:
   Upon execution completion, LED blinks for F5xx/F6xx device target socket
   boards when used as the target system (P1.0 drives LED).

   Note: All High Level JTAG Functions are applied here.

   A number of lines have been commented out which are not strictly required
   to successfully program a flash or fram-based MSP430 device using the 
   Replicator concept. Please note that these lines have been commented out in 
   order to provide reference examples for calling such functions.
   Uncommented lines below represent the minimum requirements to successfully
   program an MSP430 device using the Replicator.
   
   The basic routine consists of the following steps:
   
   1.  | Initialize the host MSP430 on the Replicator board
   2.  | Connect to the target device (provide JTAG password if needed)
   3.  | Control the target peripherals directly
   4.  | Perform a write + read + verify in the target RAM
   5a. | Flash devices: Operations in the device's main memory
   5b. | FRAM devices: Operations in the device's main memory
   6.  | Program the JTAG lock key
   7.  | Release the device from JTAG control and wait for the user to press button "S1"
*/

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "JTAGfunc430Xv2.h"      // JTAG functions
#include "Config430Xv2.h"        // High level user configuration
#include "LowLevelFunc430Xv2.h"  // Low level functions
#include "Target_Code.h"         // holds declarations and/or data of target program code

/****************************************************************************/
/* VARIABLES                                                                */
/****************************************************************************/

//! \brief This variable holds the start address of the main memory       
unsigned long mainStartAdress = MAIN_START_ADDRESS;
//! \brief This variable holds the length of the main memory (in words)
unsigned long mainLength = MAIN_LENGTH;

/****************************************************************************/
/* FUNCTIONS                                                                */
/****************************************************************************/

/*===============================================================================================
  Name         :  initProgrammer
  Description  :  Initialize programmer.
                  This function setup input/output pins of host and display GREEN LED.
  Argument(s)  :  None.
  Return value :  None.
================================================================================================*/
int initProgrammer(void)
{
/*------------------------------------------------------------------------------------------------------*/
/*  1. | Initialize host MSP430 (on Replicator board) & target board                                    */
/*------------------------------------------------------------------------------------------------------*/

    InitController();                     // Initialize the host MSP430F5437
    
    ShowStatus(STATUS_SUCCESS, 0);         // Switch both LEDs on to indicate operation.
    
    InitTarget();                         // Initialize target board    
	
	return 0;
}


/*===============================================================================================
  Name         :  eraseTarget
  Description  :  
  Argument(s)  :  None.
  Return value :  None.
================================================================================================*/
int eraseTarget(unsigned char fram_memory)
{
	unsigned char buffer[2];
	word WriteData[WordBufferSize];
	word ReadData[WordBufferSize];

	// Initialize host MSP430 (on Replicator board) & target board
	initProgrammer();
	
	if (GetDevice() != STATUS_SUCCESS)         	// Set DeviceId
    {
        ReleaseDevice(V_RESET);
	    ShowStatus(STATUS_ERROR, 1);

	    // Send status back to computer
	    usbBuffer[0] = STATUS_DETECT_FAILED;
	    hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

	    return STATUS_ERROR;
    }

	if(!fram_memory)
	{    
		/*---------------------------------------------------------------------------------*/
		/*  Flash devices: Operations in the device's main memory                    	   */
		/*---------------------------------------------------------------------------------*/
		// Check Flash memory access		
		// Program Flash in main memory
		EraseFLASH(ERASE_SEGMENT, mainStartAdress);
		
		// Erase all Flash memory banks in main memory.
		EraseFLASH(ERASE_MAIN, mainStartAdress); 

		// Check, if main memory is erased completely
		if (!EraseCheck(mainStartAdress, mainLength))   
		{
			ReleaseDevice(V_RESET);
		    ShowStatus(STATUS_ERROR, 1);

		    // Send status back to computer
		    usbBuffer[0] = STATUS_ERASE_FAILED;		    
		    hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

		    return STATUS_ERROR;
		}
	}
	else
	{
		/*---------------------------------------------------------------------------------*/
		/*  FRAM devices: Operations in the device's main memory (disable MPU if necessary)*/
		/*---------------------------------------------------------------------------------*/    
		
		// The Memory Protection Unit (MPU) allows the user to set up individual access rights for up to 
		// three user defined memory segments. For detailed information see the Memory Protection Unit 
		// User's Guide (SLAU272).
		// Uncomment the following section if the MPU is active and should be disabled 
		/*{
			// Disable Memory Protection Unit
			if (DisableMpu() != STATUS_SUCCESS)
			{
				ShowStatus(STATUS_ERROR, 1);
			}
		}*/
			
		{    
			// Erase main FRAM memory.
			EraseFRAM(mainStartAdress, mainLength);  

			// Check if main memory is completely erased.
			if (!EraseCheck(mainStartAdress, mainLength/2))   
			{ 
				ReleaseDevice(V_RESET);
			    ShowStatus(STATUS_ERROR, 1);

			    // Send status back to computer
			    usbBuffer[0] = STATUS_ERASE_FAILED;		    
			    hidSendDataInBackground(usbBuffer, START_INDEX, HID0_INTFNUM, 1);

			    return STATUS_ERROR;
			}
		}
		
		// The following section shows how to erase Info-Segments on FR57xx Devices.
		// Note: FR59xx devices have different InfoMemory boundaries!
		{    
			// Comment-in the following code to unlock Info memory segment A
		   
			/*{
				UnlockInfoA_430Xv2();
		  
				// Erase Info A memory
				EraseFRAM(0x1800,0x0040); 
				  
				// Check Info A memory erasure
				if (!EraseCheck(0x1800, 0x0040))       
				{
					ShowStatus(STATUS_ERROR, 2);
				}
		   }*/     
			 
			// // Info B memory erase
			// EraseFRAM(0x1880,0x0040); 
			  
			// // Check Info B memory erasure     
			// if (!EraseCheck(0x1880, 0x0040))      
			// {
			// 	ShowStatus(STATUS_ERROR, 2);
			// }
		}
	}

	// Release the target device from JTAG control
    ReleaseDevice(V_RESET);
    ShowStatus(STATUS_SUCCESS, 0);
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
	WriteFLASH(0x8000, 34, (word*)fdata1);
	WriteFLASH(0xFFE0,  1, (word*)fdata2);
	WriteFLASH(0xFFFE,  1, (word*)fdata3);
	
	if (VerifyMem(0x8000, 34, (word*)fdata1) == STATUS_ERROR)
	{
		ReleaseDevice(V_RESET);
		ShowStatus(STATUS_ERROR, 0);
		return(STATUS_ERROR);
	}

	// Release the target device from JTAG control    
    ReleaseDevice(V_RESET);         // Perform Reset, release CPU from JTAG control
                                    // Target board LED should start blinking
    ShowStatus(STATUS_SUCCESS, 0);       // OK: green LED is ON
	return(STATUS_SUCCESS);
}
	
//! \brief The basic Replicator routine
//! \details This function is executed once at startup and can be restarted by pressing button S1 on the REP430F board.
void runProgram(void)
{

//! \brief Array to store data for a memory write
word WriteData[WordBufferSize];
//! \brief Array to store data for a memory read
word ReadData[WordBufferSize];

/*---------------------------------------------------------------------------------*/
/*  1. | Initialize host MSP430 (on Replicator board) & target board                                    */
/*---------------------------------------------------------------------------------*/

    InitController();                     // Initialize the host MSP430F5437
    
    ShowStatus(STATUS_SUCCESS, 0);         // Switch both LEDs on to indicate operation.
    
    InitTarget();                         // Initialize target board    
    
/*---------------------------------------------------------------------------------*/
/*  2. | Connect to the target device (provide JTAG password for FRAM devices if needed)                */
/*---------------------------------------------------------------------------------*/

    // Uncomment the following section if the JTAG password is set and should be removed.
    /*{   
        // enter password & length here:
        unsigned short Password[] = { 0x1111, 0x2222 };
        unsigned long  PasswordLength = 0x02; // password length in words  
        
        //Unlock device with user password
        if(UnlockDevice(Password, PasswordLength) != STATUS_SUCCESS)   
        {
            ShowStatus(STATUS_ERROR, 1);  // stop here password was wrong
        } 
    }*/
    
    if (GetDevice() != STATUS_SUCCESS)         // Set DeviceId
    {
        ShowStatus(STATUS_ERROR, 1);      // Stop here if invalid JTAG ID or
    }                                     // time-out. (error: red LED is ON)

/*---------------------------------------------------------------------------------*/
/*  3. | Control the target peripherals directly                                                        */
/*---------------------------------------------------------------------------------*/
    
    // Remove the following comments to toggle Pin 1.0 (i.e flash the LED for MSP430 target socket boards)
    /*{ 
        word k;
        
        WriteMem(F_BYTE, 0x204, 0x01);         // P1.0 for F1xx,2xx devices
        
        for(k = 0; k < 3; k++)
        {
            WriteMem(F_BYTE, 0x202, 0x01);
            MsDelay(500);                      // LED on for 0.5s
            WriteMem(F_BYTE, 0x202, 0x00);
            MsDelay(500);                      // LED off for 0.5s
        }
        
        WriteMem(F_BYTE, 0x202, 0x01);
    }*/
    
/*---------------------------------------------------------------------------------*/
/*  4. | Perform a write + read + verify in the target RAM                                              */
/*---------------------------------------------------------------------------------*/   

    {
        word i,j;
  
        // write dummy data to target RAM    
        for (i = 0, j = 0; j < WordBufferSize; j++, i+=2)
        {
            ReadData[j]  = 0;
            WriteData[j] = j;
            WriteMem(F_WORD, RAM_START_ADDRESS + i, j);
        }      
        // read data from target RAM and verify    
        for (i = 0, j = 0; j < WordBufferSize; j++, i+=2)
        {
            ReadData[j] = ReadMem(F_WORD, RAM_START_ADDRESS + i);
            if(ReadData[j] != WriteData[j])
            {
                ShowStatus(STATUS_ERROR, 2);
            }
        }
        // verify content with PSA
        if(VerifyMem(RAM_START_ADDRESS, WordBufferSize, WriteData) != STATUS_SUCCESS)
        {
            ShowStatus(STATUS_ERROR, 3);
        }
    }

/*---------------------------------------------------------------------------------*/
/*  5a. | Flash devices: Operations in the device's main memory                                         */
/*---------------------------------------------------------------------------------*/
#ifndef  FRAM_MEMORY
    
    // Check Flash memory access
    {
        // Program Flash in main memory
        EraseFLASH(ERASE_SEGMENT, mainStartAdress);
        WriteFLASH(mainStartAdress, WordBufferSize, WriteData);
        if(VerifyMem(mainStartAdress, WordBufferSize, WriteData) != STATUS_SUCCESS)
        {
            ShowStatus(STATUS_ERROR, 4);
        }
    }

    // This will erase all Flash memory banks in main memory.
    // Additionally an erase check is performed
    {
        // Erase all Flash memory banks in main memory.
        EraseFLASH(ERASE_MAIN, mainStartAdress);  
        // Check, if main memory is erased completely
        if (!EraseCheck(mainStartAdress, mainLength))   
        {
            ShowStatus(STATUS_ERROR, 2);
        }       
    }

    // This will only erase one flash memory bank (64KB) in main memory.
    // I.e. the second memory bank of an MSP430F5438 can be erased by calling:
    {   
        //EraseFLASH(ERASE_BANK, 0x10100);
    }  

    // The following section shows how to erase Info-Segments on 5xx Devices selectively
    {
        // Comment-in the following code to unlock Info memory segment A
      
        /*{
            UnlockInfoA_430Xv2();
      
            // Info A memory erase
            EraseFLASH(ERASE_SEGMENT, 0x1980);     
        
            // Check Info A memory erasure
            if (!EraseCheck(0x1980, 0x0040))       
            {
                ShowStatus(STATUS_ERROR, 2);
            }
        }*/   
        
        // Info B memory erase
        EraseFLASH(ERASE_SEGMENT, 0x1900); 
        
        // Check Info B memory erasure     
        if (!EraseCheck(0x1900, 0x0040))      
        {
            ShowStatus(STATUS_ERROR, 2);
        }
        // Info C erase
        EraseFLASH(ERASE_SEGMENT, 0x1880);
        // Check Info C memory erasure
        if (!EraseCheck(0x1880, 0x0040))      
        {
            ShowStatus(STATUS_ERROR, 2);
        }
        // Info D erase
        EraseFLASH(ERASE_SEGMENT, 0x1800);    
        // Check Info D memory erasure 
        if (!EraseCheck(0x1800, 0x0040))      
        {
            ShowStatus(STATUS_ERROR, 2);
        }    
    }

    // The following code unlocks the BSL memory on 5xx/6xx devices with flash memory 
	// for reading/writing/erasure.
	// WARNING: The code in the BSL memory area should only be erased if another BSL is 
	//          programmed immediately afterwards. Otherwise, the device might not start 
	//          up properly anymore.  
    /*{
        UnlockBsl_430Xv2Flash();
    }*/
       
    // Program target code    
    if (!WriteFLASHallSections(&eprom[0], &eprom_address[0], &eprom_length_of_sections[0], eprom_sections))
    {
        ShowStatus(STATUS_ERROR, 10);
    }

/*---------------------------------------------------------------------------------*/
/*  5b. | FRAM devices: Operations in the device's main memory (disable MPU if necessary)               */
/*---------------------------------------------------------------------------------*/    
#else // Device has FRAM memory      

    // The Memory Protection Unit (MPU) allows the user to set up individual access rights for up to 
    // three user defined memory segments. For detailed information see the Memory Protection Unit 
    // User's Guide (SLAU272).
    // Uncomment the following section if the MPU is active and should be disabled 
    /*{
        // Disable Memory Protection Unit
        if (DisableMpu() != STATUS_SUCCESS)
        {
            ShowStatus(STATUS_ERROR, 1);
        }
    }*/
        
    {    
        // Erase main FRAM memory.
        EraseFRAM(mainStartAdress, mainLength);  
        // Check if main memory is completely erased.
        if (!EraseCheck(mainStartAdress, mainLength/2))   
        { 
            ShowStatus(STATUS_ERROR, 2);
        }
    }
    
    // The following section shows how to erase Info-Segments on FR57xx Devices.
    // Note: FR59xx devices have different InfoMemory boundaries!
    {    
        // Comment-in the following code to unlock Info memory segment A
       
        /*{
            UnlockInfoA_430Xv2();
      
            // Erase Info A memory
            EraseFRAM(0x1800,0x0040); 
              
            // Check Info A memory erasure
            if (!EraseCheck(0x1800, 0x0040))       
            {
                ShowStatus(STATUS_ERROR, 2);
            }
       }*/     
	     
        // Info B memory erase
        EraseFRAM(0x1880,0x0040); 
          
        // Check Info B memory erasure     
        if (!EraseCheck(0x1880, 0x0040))      
        {
            ShowStatus(STATUS_ERROR, 2);
        }
    }
    
    // Program blinking LED target code    
    if (!WriteFRAMallSections_430Xv2(&eprom[0], &eprom_address[0], &eprom_length_of_sections[0], eprom_sections))
    {
        ShowStatus(STATUS_ERROR, 10);
    }  

#endif  
/*---------------------------------------------------------------------------------*/
/*  6. | Program the JTAG lock key                                                                      */
/*---------------------------------------------------------------------------------*/
    
    // Remove following comments to enable Lock Key programming routine.
    // This makes the MSP430 device permanently inaccessible via JTAG
    
    /*if (!ProgramLockKey())        // ***Action is permanent***
    {
        ShowStatus(STATUS_ERROR, 15);
    }*/

/*---------------------------------------------------------------------------------*/
/*  7. | Release the target device from JTAG control and wait for the user to press button "S1"         */
/*---------------------------------------------------------------------------------*/

    ReleaseDevice(V_RESET);         // Perform Reset, release CPU from JTAG control
    ShowStatus(STATUS_SUCCESS, 0);       // OK: green LED is ON    
}

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
