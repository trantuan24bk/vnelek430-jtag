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

#ifndef _CONFIG430_H_
#define _CONFIG430_H_

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
#define CMD_READ				0x11
#define CMD_READ_END			0x12
#define CMD_WRITE				0x13
#define CMD_WRITE_FIRST			0x1A
#define CMD_WRITE_END			0x14
#define CMD_VERIFY				0x15
#define CMD_VERIFY_END			0x16
#define CMD_DETECT				0x17
#define CMD_ERASE				0x18
#define CMD_HWTEST				0x19
#define CMD_POWERON				0x1A

#define STATUS_SUCCESS			0x01				// Returned status code
#define STATUS_ERROR			0x00

#define STATUS_CRC_FAILED		0x20
#define STATUS_CRC_SUCCESS		0x21

#define STATUS_WRITE_FAILED		0x22
#define STATUS_WRITE_SUCCESS	0x23

#define STATUS_VERIFY_FAILED	0x24
#define STATUS_VERIFY_SUCCESS	0x25

#define STATUS_POWERON_FAILED	0x26
#define STATUS_POWERON_SUCCESS  0x27

#define STATUS_ERASE_FAILED		0x28
#define STATUS_ERASE_SUCCESS	0x29

#define STATUS_HWTEST_FAILED	0x2A
#define STATUS_HWTEST_SUCCESS	0x2B

#define STATUS_DETECT_FAILED	0x2C
#define STATUS_DETECT_SUCCESS	0x2D

#define STATUS_NOT_RESPONDING	0x2E

#define GET_DEVICE_RAM_END			0x60			// Retrieve device's infomation
#define GET_DEVICE_RAM_START		0x61
#define GET_DEVICE_INFO_END			0x62
#define GET_DEVICE_INFO_START		0x63
#define GET_DEVICE_FLASH_END		0x64
#define GET_DEVICE_FLASH_START		0x65

#define GET_DEVICE_HAS_CPUX			0x66
#define GET_DEVICE_HAS_DATAQUICK	0x67
#define GET_DEVICE_ENH_VERIFY		0x68
#define GET_DEVICE_FAST_FLASH		0x69
#define GET_DEVICE_JTAG				0x6A
#define GET_DEVICE_SPYBIWIRE		0x6B
#define GET_DEVICE_HAS_TESTPIN		0x6C

#define DEVICE_NOT_MATCH			0xFFFF

/****************************************************************************/
/* FUNCTION PROTOTYPES                                                      */
/****************************************************************************/

#endif
/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
