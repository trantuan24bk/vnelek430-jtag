/*==========================================================================*\
|                                                                            |
| LowLevelFunc430.h                                                          |
|                                                                            |
| Low Level function prototypes, macros, and pin-to-signal assignments       |
| regarding to user's hardware                                               |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               FRGR                                                 |
| Version:              1.9                                                  |
| Initial Version:      04-17-02                                             |
| Last Change:          05-24-12                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 04/02 FRGR        Initial version.                                     |
| 1.1 04/02 FRGR        Included SPI mode to speed up shifting function by 2.|
|                       (JTAG control now on Port5)                          |
| 1.2 06/02 ALB2        Formatting changes, added comments. Removed code used|
|                       for debug purposes during development.               |
| 1.3 08/02 ALB2        Initial code release with Lit# SLAA149.              |
| 1.4 09/05 SUN1        Software delays redesigned to use TimerA harware;    |
|                       see MsDelay() routine. Added TA constant.            |
| 1.5 12/05 STO         Added RESET pin definition                           |
| 1.6 08/08 WLUT        Added DrvSignals and RlsSignals macros for clean     |
|                       JTAG init sequence.                                  |
| 1.7 05/09 GC (Elprotronic)  Added support for the new hardware - VNELEK-PROG   |
| 1.8 07/09 FB          Added macros for Spy-Bi-Wire support                 |
| 1.9 05/12 RL          Updated commentaries                                 |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/


//! \file LowLevelFunc430.h
//! \brief Low Level function prototypes, macros, and pin-to-signal assignments regarding to user's hardware
//! \author Florian Berenbrinker
//! \author Gregory Czajkowski (Elprotronic)
//! \date 05/24/2011
//! \version 1.9

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "Config430.h"        // High-level user input
 #include <msp430f5510.h>

/****************************************************************************/
/* DEFINES & CONSTANTS                                                      */
/****************************************************************************/

//! \brief JTAG interface
#define JTAG_IF             1
//! \brief JTAG interface on a device that supports JTAG and SBW
#define SPYBIWIREJTAG_IF    2
//! \brief Spy-Bi-Wire interface
#define SPYBIWIRE_IF        3


#ifndef __DATAFORMATS__
#define __DATAFORMATS__
#define F_BYTE                     8
#define F_WORD                     16
#define F_ADDR                     20
#define F_LONG                     32
#endif

// Constants for runoff status

//! \brief GetDevice returns this if the security fuse is blown
#define STATUS_FUSEBLOWN 2

//! \brief Replicator is idling
#define STATUS_IDLE      3

/****************************************************************************/
/* Macros and Pin-to-Signal assignments which have to be programmed         */
/* by the user. This implementation assumes use of an MSP430F5510 as the    */
/* host controller and the corresponding hardware given in the MSP430       */
/* Programming Via the JTAG Interface User's Guide (SLAU320).               */
/*                                                                          */
/* The following MSP430 example acts as a hint of how to generally          */
/* implement a micro-controller programmer solution for the MSP430 flash-   */
/* based devices.                                                           */
/****************************************************************************/

//! \brief Status LED registers
#define LED_OUT         P1OUT  
#define LED_DIR         P1DIR

#define LED_GREEN       BIT1
#define LED_RED         BIT0		// Change to P1.3


//! \brief Output 3.3 VCC to Target
#define TVCC_EN_OUT		P1OUT
#define TVCC_EN_DIR     P1DIR
#define TVCC_EN_BIT    	BIT5

//! \brief Check VCC from Target
#define TVCC_RD_IN		P4IN
#define TVCC_RD_DIR		P4DIR
#define TVCC_RD_BIT		BIT7

#define TARGET_HAS_3V	3
#define TARGET_HAS_0V	0

//! \brief Switch on red LED
#define LED_red_on()        ( LED_OUT |= LED_RED )
//! \brief Switch off red LED
#define LED_red_off()       ( LED_OUT &= ~LED_RED )
//! \brief Switch on green LED
#define LED_green_on()      ( LED_OUT |= LED_GREEN )
//! \brief Switch off green LED
#define LED_green_off()     ( LED_OUT &= ~LED_GREEN )
//! \brief Switch off all LEDs
#define All_LEDs_off()      ( LED_OUT &= ~(LED_GREEN | LED_RED ))

#define toggleGreenLED()	(LED_OUT ^= LED_GREEN)

//! \brief JTAG in/out registers
#define JTAGOUT         P4OUT
#define JTAGIN          P4IN
#define JTAGDIR         P4DIR
#define JTAGSEL         P4SEL


//! \brief P4.0 JTAG TDO output pin
#define TDO             BIT0
//! \brief P4.1 JTAG TDI input pin
#define TDI             BIT1
//! \brief P4.2 JTAG TMS input pin
#define TMS             BIT2
//! \brief P4.3 JTAG TCK input pin
#define TCK             BIT3
//! \brief P4.4 Hardware RESET input pin
#define RST             BIT4
//! \brief P4.5 JTAG Test input pin
#define TEST            BIT5

//! \brief TDI (former XOUT) receives TCLK
#define TCLK            TDI

// Constants for Setting up Timer A
//! \brief CCR0 delay for 1ms with a 3MHz TA clock
#define ONEMS           2990		// ~3000

/****************************************************************************/
/* Macros for processing the JTAG port                                      */
/****************************************************************************/

//! \brief JTAG macro: clear/set TMS signal
#define ClrTMS()    ((JTAGOUT) &= (~TMS))
#define SetTMS()    ((JTAGOUT) |= (TMS))

//! \brief JTAG macro: clear/set TDI signal
#define ClrTDI()    ((JTAGOUT) &= (~TDI))
#define SetTDI()    ((JTAGOUT) |= (TDI))

//! \brief JTAG macro: clear/set TCK signal
#define ClrTCK()    ((JTAGOUT) &= (~TCK))
#define SetTCK()    ((JTAGOUT) |= (TCK))

//! \brief JTAG macro: return current TCLK signal (on TDI pin)
#define StoreTCLK() ((JTAGOUT  &   TCLK))

//! \brief JTAG macro: restore TCLK signal on TDI pin (based on input: x)
#define RestoreTCLK(x)  (x == 0 ? (JTAGOUT &= ~TCLK) : (JTAGOUT |= TCLK))

//! \brief JTAG macro: return TDO value (result 0 or TDO (0x40))
#define ScanTDO()   ((JTAGIN   &   TDO))

//! \brief JTAG macro: set RST signal
#define SetRST()    ((JTAGOUT) |= (RST))
//! \brief JTAG macro: clear RST signal
#define ClrRST()    ((JTAGOUT) &= (~RST))
//! \brief JTAG macro: release RST pin
#define ReleaseRST() ( RST_dir( 0 ))
//! \brief JTAG macro: set TST signal
#define SetTST()    ((JTAGOUT) |= (TEST))
//! \brief JTAG macro: clear TST signal
#define ClrTST()    ((JTAGOUT) &= (~TEST))

//! \brief clear the TCLK signal
#define ClrTCLK()  ((JTAGOUT) &= ~(TCLK))
//! \brief set the TCLK signal
#define SetTCLK()  ((JTAGOUT) |=  (TCLK))


/*----------------------------------------------------------------------------
   Definition of global variables
*/
extern byte TCLK_saved;      // holds the last value of TCLK before entering a JTAG sequence

/****************************************************************************/
/* TYPEDEFS                                                                 */
/****************************************************************************/

#ifndef __BYTEWORD__
#define __BYTEWORD__
typedef unsigned int word;
typedef unsigned char byte;
#endif

/****************************************************************************/
/* FUNCTION PROTOTYPES                                                      */
/****************************************************************************/

void    MsDelay(word milliseconds);      // millisecond delay loop, uses Timer_A
void    usDelay(word microeconds);       // microsecond delay loop, uses nops

#define usDelay5()		usDelay(4)
#define usDelay50()		usDelay(50)
#define usDelay100()	usDelay(100)
#define usDelay800()	usDelay(795)

void    InitController(void);
void    InitTarget(void);
void    ReleaseTarget(void);

word    Shift(word Format, word Data);   // used for IR- as well as DR-shift
void    TDOisInput(void);
void    TCLKstrobes(word Amount);
void    ShowStatus(word Status, word Index);
void    DrvSignals( void );
void    RlsSignals( void );
void    SetTargetVcc (void);
void    ClearTargetVcc (void);			// *new
byte    GetTargetVcc( void);

void    TDI_dir( word dir );
void    TDOI_dir( word dir );
void    TEST_dir( word dir );
void    TMS_dir( word dir );
void    RST_dir( word dir );
void    TCK_dir( word dir );

void    ConfigureIO_JTAG( void );
void    IO_3state( void );

