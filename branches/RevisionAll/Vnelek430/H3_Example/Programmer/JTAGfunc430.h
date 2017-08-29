/*==========================================================================*\
|                                                                            |
| JTAGfunc430.h                                                              |
|                                                                            |
| JTAG Function Prototypes and Definitions                                   |
|----------------------------------------------------------------------------|
| Project:              JTAG Functions                                       |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               STO                                                  |
| Version:              1.5                                                  |
| Initial Version:      04-17-02                                             |
| Last Change:          03-14-13                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 04/02 FRGR        Initial version.                                     |
| 1.1 06/02 ALB2        Formatting changes, added comments.                  |
| 1.2 08/02 ALB2        Initial code release with Lit# SLAA149.              |
| 1.3 01/06 STO         Minor cosmetic changes                               |
| 1.4 04/07 WLUT        WriteFLASHallSections changed due to function spec   |
| 1.5 03/13 RL/MD       Added unlock function for Info A                     |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/


//! \file JTAGfunc430.h
//! \brief JTAG Function Prototypes and Definitions
//! \author Berenbrinker Florian
//! \date 03/14/2013
//! \version 1.5

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "Config430.h"        // High-level user input

/****************************************************************************/
/* Global types                                                             */
/****************************************************************************/

#ifndef __BYTEWORD__
#define __BYTEWORD__
typedef unsigned int  word;
typedef unsigned char byte;
#endif

/****************************************************************************/
/* Define section for constants                                             */
/****************************************************************************/

//----------------------------------------------------------------------------
// Constants for the JTAG instruction register (IR) require LSB first.
// The MSB has been interchanged with LSB due to use of the same shifting
// function as used for the JTAG data register (DR) which requires MSB 
// first.
//----------------------------------------------------------------------------

// Instructions for the JTAG control signal register
//! \brief Set the JTAG control signal register
#define IR_CNTRL_SIG_16BIT         0xC8   // original value: 0x13
//! \brief Read out the JTAG control signal register
#define IR_CNTRL_SIG_CAPTURE       0x28   // original value: 0x14
//! \brief Release the CPU from JTAG control
#define IR_CNTRL_SIG_RELEASE       0xA8   // original value: 0x15

// Instructions for the JTAG fuse
//! \brief Prepare for JTAG fuse blow
#define IR_PREPARE_BLOW            0x44   // original value: 0x22
//! \brief Perform JTAG fuse blow
#define IR_EX_BLOW                 0x24   // original value: 0x24

// Instructions for the JTAG data register
//! \brief Set the MSP430 MDB to a specific 16-bit value with the next 
//! 16-bit data access 
#define IR_DATA_16BIT              0x82   // original value: 0x41
//! \brief Set the MSP430 MDB to a specific 16-bit value (RAM only)
#define IR_DATA_QUICK              0xC2   // original value: 0x43

// Instructions for the JTAG PSA mode
//! \brief Switch JTAG data register to PSA mode
#define IR_DATA_PSA                0x22   // original value: 0x44
//! \brief Shift out the PSA pattern generated by IR_DATA_PSA
#define IR_SHIFT_OUT_PSA           0x62   // original value: 0x46

// Instructions for the JTAG address register
//! \brief Set the MSP430 MAB to a specific 16-bit value
//! \details Use the 20-bit macro for 430X and 430Xv2 architectures
#define IR_ADDR_16BIT              0xC1   // original value: 0x83
//! \brief Read out the MAB data on the next 16/20-bit data access
#define IR_ADDR_CAPTURE            0x21   // original value: 0x84
//! \brief Set the MSP430 MDB with a specific 16-bit value and write
//! it to the memory address which is currently on the MAB
#define IR_DATA_TO_ADDR            0xA1   // original value: 0x85
//! \brief Bypass instruction - TDI input is shifted to TDO as an output
#define IR_BYPASS                  0xFF   // original value: 0xFF

// JTAG identification values for Flash-based MSP430 devices
//! \brief JTAG identification value for MSP430 architecture devices
#define JTAG_ID                    0x89

// Constants for data formats, dedicated addresses
#define F_BYTE                     8
#define F_WORD                     16
#define V_RESET                    0xFFFE


/****************************************************************************/
/* Function prototypes                                                      */
/****************************************************************************/

// Low level JTAG functions
word DR_Shift16(word Data);
word IR_Shift(byte Instruction);
void ResetTAP(void);
word ExecutePOR(void);
word SetInstrFetch(void);
void SetPC(word Addr);
void HaltCPU(void);
void ReleaseCPU(void);
word VerifyPSA(word StartAddr, word Length, word *DataArray);

// High level JTAG functions
word GetDevice(void);

word detectTargetL(word* DeviceId, byte* SubId);		// new

void ReleaseDevice(word Addr);
void WriteMem(word Format, word Addr, word Data);
void WriteMemQuick(word StartAddr, word Length, word *DataArray);
void WriteFLASH(word StartAddr, word Length, word *DataArray);
word WriteFLASHallSections(const unsigned int *data, const unsigned long *address, 
						   const unsigned long *length_of_sections, const unsigned long sections);

word ReadMem(word Format, word Addr);
void ReadMemQuick(word StartAddr, word Length, word *DataArray);
void EraseFLASH(word EraseMode, word EraseAddr);
word EraseCheck(word StartAddr, word Length);
word VerifyMem(word StartAddr, word Length, word *DataArray);
word BlowFuse(void);
word IsFuseBlown(void);
void UnlockInfoA(void);

/****************************************************************************/
/* VARIABLES                                                                */
/****************************************************************************/

//! \brief Holds the device's JTAGID
static byte JtagId = 0;

//! \brief Holds the Flash InfoA Lock/Unlock Key, default = locked
static unsigned short SegmentInfoAKey = 0xA500;

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
