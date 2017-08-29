/*==========================================================================*\
|                                                                            |
| Config430.h                                                                |
|                                                                            |
| Replicator configuration file for MSP430 Flash-based family devices.       |
|                                                                            |
|                                                                            |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 4.11B [Kickstart]             |
|             and:      Code Composer Essentials v3                          |
|----------------------------------------------------------------------------|
| Author:               RL                                                   |
| Version:              1.0                                                  |
| Initial Version:      06-18-12                                             |
| Last Change:          06-18-12                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 06/12 RL          Initial version.                                     |
|----------------------------------------------------------------------------|
| Designed 2012 by Texas Instruments Germany                                 |
\*==========================================================================*/


//! \file Config430.h
//! \brief Configurations for the MSP430 Replicator.
//! \author Robert Lessmeier
//! \date 06/18/2012
//! \version 1.0

/****************************************************************************/
/* QUICK START OPTIONS                                                      */
/****************************************************************************/

//! \brief Select the interface to be used to communicate with the device
//! \details Options: 
//! JTAG_IF          - MCU has 4-wire JTAG ONLY (F1xx, old F4xx)
//! SPYBIWIRE_IF     - 2-wire Spy-Bi-Wire (F2xx, F4xx with SBW, F5xx, F6xx)
//! SPYBIWIREJTAG_IF - 4-wire JTAG in MCU with Spy-Bi-Wire (F2xx, F4xx, with SBW: F5xx, F6xx)
//! Select ONLY ONE interface, comment-out remaining options 

#define INTERFACE  JTAG_IF
//#define INTERFACE  SPYBIWIRE_IF
//#define INTERFACE  SPYBIWIREJTAG_IF


/****************************************************************************/
/* OTHER DEFINES                                                            */
/****************************************************************************/

//! \brief Buffer size in words for read and write operations
#define WordBufferSize  50

//! \brief Maximum number of tries for the determination of the core
//! identification info
#define MAX_ENTRY_TRY  7

/****************************************************************************/
/* TYPEDEFS                                                                 */
/****************************************************************************/

#ifndef __BYTEWORD__
#define __BYTEWORD__
typedef unsigned int word;
typedef unsigned char byte;
#endif

#ifndef BIT0
#define BIT0                (0x0001u)
#define BIT1                (0x0002u)
#define BIT2                (0x0004u)
#define BIT3                (0x0008u)
#define BIT4                (0x0010u)
#define BIT5                (0x0020u)
#define BIT6                (0x0040u)
#define BIT7                (0x0080u)
#define BIT8                (0x0100u)
#define BIT9                (0x0200u)
#define BITA                (0x0400u)
#define BITB                (0x0800u)
#define BITC                (0x1000u)
#define BITD                (0x2000u)
#define BITE                (0x4000u)
#define BITF                (0x8000u)
#endif


/****************************************************************************/
/* Define command between PC and host                                       */
/****************************************************************************/

#define START_INDEX			7		// Index of code in data-frame

 // Define command
#define CMD_READ     	 	0x11
#define CMD_READ_END      	0x12
#define CMD_WRITE    	 	0x13
#define CMD_WRITE_FIRST	 	0x1A
#define CMD_WRITE_END 	 	0x14
#define CMD_VERIFY   	 	0x15
#define CMD_VERIFY_END    	0x16
#define CMD_DETECT   	 	0x17
#define CMD_ERASE    	 	0x18
#define CMD_HWTEST   	 	0x19
#define CMD_POWERON  	 	0x20

// Status code
#define STATUS_SUCCESS         	0x01
#define STATUS_ERROR        	0x00

#define STATUS_CRC_FAILED      	0x33
#define STATUS_CRC_SUCCESS     	0x34

#define STATUS_WRITE_FAILED    	0x35
#define STATUS_WRITE_SUCCESS   	0x36

#define STATUS_VERIFY_FAILED   	0x37
#define STATUS_VERIFY_SUCCESS	0x38

#define STATUS_POWERON_FAILED   0x39
#define STATUS_POWERON_SUCCESS  0x40

#define STATUS_ERASE_FAILED  	0x41
#define STATUS_ERASE_SUCCESS 	0x42

#define STATUS_HWTEST_FAILED   	0x43
#define STATUS_HWTEST_SUCCESS  	0x44

#define STATUS_DETECT_FAILED   	0x45
#define STATUS_DETECT_SUCCESS 	0x46

/****************************************************************************/
/* FUNCTION PROTOTYPES                                                      */
/****************************************************************************/

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
