/*==========================================================================*\
|                                                                            |
| Config430Xv2.h                                                             |
|                                                                            |
| Replicator configuration file for MSP430Xv2 Flash-based family (5xx/6xx)   |
| and FRAM-based devices.                                                    |
|                                                                            |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator Xv2                                |
| Developed using:      IAR Embedded Workbench 4.11B [Kickstart]             |
|             and:      Code Composer Essentials v3                          |
|----------------------------------------------------------------------------|
| Author:               RL                                                   |
| Version:              1.0                                                  |
| Initial Version:      04-30-12                                             |
| Last Change:          10-24-12                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 04/12 RL          Initial version.                                     |
| 1.1 10/12 RL          Added #define for start address of target RAM        |
|----------------------------------------------------------------------------|
| Designed 2012 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file Config430Xv2.h
//! \brief Configurations for the MSP430 Replicator Xv2.
//! \author Robert Lessmeier
//! \date 10/24/2012
//! \version 1.1

/****************************************************************************/
/* QUICK START OPTIONS                                                      */
/****************************************************************************/

//! \brief Select the interface to be used to communicate with the device
//! \details Options: 
//! <li> JTAG_IF          - MCU has 4-wire JTAG ONLY (F1xx, old F4xx)
//! <li> SPYBIWIRE_IF     - 2-wire Spy-Bi-Wire (F2xx, F4xx with SBW, F5xx, F6xx)
//! <li> SPYBIWIREJTAG_IF - 4-wire JTAG in MCU with Spy-Bi-Wire (F2xx, F4xx 
//! with SBW, F5xx, F6xx)

//! Select ONLY ONE interface, comment-out the remaining option
#define INTERFACE  JTAG_IF
//#define INTERFACE  SPYBIWIRE_IF
// #define INTERFACE  SPYBIWIREJTAG_IF

////! \brief Comment this in if your device has FRAM instead of flash memory
//#define FRAM_MEMORY         // device has FRAM memory


//! \brief Set start address of the main memory
#define MAIN_START_ADDRESS 0xF000       

//! \brief Set length of main memory
//! \details Note that the memory length is counted in 16-bit WORDS!
#define MAIN_LENGTH 0x800

//! \brief Set the start address of the device RAM
#define RAM_START_ADDRESS 0x1C00

// This table lists the possible start adresses for the device's RAM
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                 DEFAULT  ->  RAM_START_ADDRESS  0x1C00                   */
/*                   F52xx  ->  RAM_START_ADDRESS  0x2400                   */
/*--------------------------------------------------------------------------*/

// The following table lists the main start addresses and memory length for 
// most 5xx/6xx devices. If your device is not listed, please look up these 
// values in the device data sheet.
/*--------------------------------------------------------------------------*/
/*    Examples:                                                             */
/*                                                                          */
/* F5529 F5528 F5519  ->  MAIN_START_ADDRESS  0x4400  MAIN_LENGTH  0x10000  */
/* F5527 F5526 F5517  ->  MAIN_START_ADDRESS  0x4400  MAIN_LENGTH  0xC000   */
/* F5525 F5524 F5515  ->  MAIN_START_ADDRESS  0x4400  MAIN_LENGTH  0x8000   */
/* F5522 F5521 F5513  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x4000   */
/*                                                                          */  
/* F6137 F6127 F5137  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x4000   */   
/*                                                                          */   
/* F5438A F5438       ->  MAIN_START_ADDRESS  0x5C00  MAIN_LENGTH  0x20000  */
/* F5436A F5436       ->  MAIN_START_ADDRESS  0x5C00  MAIN_LENGTH  0x18000  */ 
/* F5419A F5419       ->  MAIN_START_ADDRESS  0x5C00  MAIN_LENGTH  0x10000  */ 
/*                                                                          */
/* F5510              ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x4000   */ 
/*                                                                          */
/* F5132              ->  MAIN_START_ADDRESS  0xE000  MAIN_LENGTH  0x1000   */                
/* F5152              ->  MAIN_START_ADDRESS  0xC000  MAIN_LENGTH  0x2000   */ 
/* F5172              ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x4000   */ 
/*                                                                          */
/* F5636 F5633 F5630  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x10000  */                
/* F5637 F5634 F5631  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x18000  */ 
/* F5638 F5635 F5632  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x20000  */   
/*                                                                          */
/* F6636 F6633 F6630  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x10000  */                
/* F6637 F6634 F6631  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x18000  */ 
/* F6638 F6635 F6632  ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x20000  */
/*                                                                          */
/* F67x9(1) F67x8(1)  ->  MAIN_START_ADDRESS  0xC000  MAIN_LENGTH  0x40000  */ 
/* F67x7(1) F67x6(1)  ->  MAIN_START_ADDRESS  0xC000  MAIN_LENGTH  0x20000  */
/* F67x5(1)           ->  MAIN_START_ADDRESS  0xC000  MAIN_LENGTH  0x10000  */
/*                                                                          */
/* FR5739             ->  MAIN_START_ADDRESS  0xC200  MAIN_LENGTH  0x1F00   */
/*                                                                          */
/* CC430F6147         ->  MAIN_START_ADDRESS  0x8000  MAIN_LENGTH  0x4000   */
/*--------------------------------------------------------------------------*/

/****************************************************************************/
/* OTHER DEFINES                                                            */
/****************************************************************************/

//! \brief Select the main clock frequency
//! \details Comment it out for MCLK=12MHz, if the Voltage supplied to the 
//! REP430F is low (below 2.5V).
//! That can apply when the REP430F is supplied from the target device, not 
//! from the external power supply.

//! \brief Use Jtag Mailbox to write FRAM
#define MailBoxWriteFram
//! \brief Activate support for Low Power Mode x.5
#define LPM5_ACTIVATED  1
//! \brief Allow use of the "magic pattern"
#define ACTIVATE_MAGIC_PATTERN  1
//! \brief For devices with JTAG bug 17 - see device errata sheet (slaz046) 
#define DEVICE_HAS_JTAG17  1  
//! \brief Buffer size for read and write operations in words
#define WordBufferSize  50
//! \brief Maximum number of tries for the determination of the core
//! identification info
#define MAX_ENTRY_TRY  4

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


/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
