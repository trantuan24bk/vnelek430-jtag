/*==========================================================================*\
|                                                                            |
| LowLevelFunc430Xv2.h                                                       |
|                                                                            |
| Low Level function prototypes, macros, and pin-to-signal assignments       |
| regarding to user's hardware                                               |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               FRGR                                                 |
| Version:              2.0                                                  |
| Initial Version:      04-17-02                                             |
| Last Change:          06-29-12                                             |
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
| 1.6 03/08 WLUT        Removed reference to JTAGfunc430X.h file             |
|                       Added data format definitions                        |
| 1.7 08/08 WLUT        Added macros for RST and TEST pin handling           |
|                       Added macros for Spy-Bi-Wire capability.             |
| 1.8 10/08 WLUT        Fixed macro DrvSignals to drive correct signals      |
|                       for Spy-Bi-Wire.                                     |
| 1.9 05/09 GC (Elprotronic)  Added support for the new hardware - REP430F   |
| 2.0 10/11 FB          Added support for JSBW functions                     |
| 2.1 06/12 RL          Updated commentaries                                 |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file LowLevelFunc430Xv2.h
//! \brief Low Level function prototypes, macros, and pin-to-signal assignments regarding to user's hardware
//! \author Florian Berenbrinker
//! \author Gregory Czajkowski (Elprotronic)
//! \date 06/29/2012
//! \version 2.1

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "Config430Xv2.h"        // High-level user input
#include <msp430f5510.h>

/****************************************************************************/
/* DEFINES & CONSTANTS                                                      */
/****************************************************************************/

//! \brief JTAG interface on a device that supports JTAG and SBW
#define SPYBIWIREJTAG_IF    1
//! \brief Spy-Bi-Wire interface
#define SPYBIWIRE_IF        2


#define RSTLOW_SBW   0
#define RSTLOW_JTAG  1
#define RSTHIGH_SBW  2
#define RSTHIGH_JTAG 3

#ifndef __DATAFORMATS__
#define __DATAFORMATS__
#define F_BYTE                     8
#define F_WORD                     16
#define F_ADDR                     20
#define F_LONG                     32
#endif


//! \brief GetDevice returns this if the security fuse is blown
#define STATUS_FUSEBLOWN 2

//! \brief Replicator is idling
#define STATUS_IDLE      3

/****************************************************************************/
/* Pin-to-Signal Assignments                                                */
/****************************************************************************/

//! \brief LED output register
#define LED_OUT         P1OUT  
//! \brief LED direction register     
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

//! \brief JTAG output register
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

//! \brief JTAG macro: clear TMS signal
#define ClrTMS()    ((JTAGOUT) &= (~TMS))
#define SetTMS()    ((JTAGOUT) |= (TMS))

//! \brief JTAG macro: clear TDI signal
#define ClrTDI()    ((JTAGOUT) &= (~TDI))
#define SetTDI()    ((JTAGOUT) |= (TDI))

//! \brief JTAG macro: clear TCK signal
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
unsigned long AllShifts(word Format, unsigned long Data);
void    TDOisInput(void);
void    TCLKstrobes(word Amount);
void    ShowStatus(word Status, word Index);
void    DrvSignals( void );
void    RlsSignals( void );
void    SetTargetVcc (void);
void    ClearTargetVcc (void);			// *new
byte    GetTargetVcc( void);
void    RST_dir( word dir );

void    TDI_dir( word dir );
void    TDOI_dir( word dir );
void    TEST_dir( word dir );
void    TMS_dir( word dir );
void    TCK_dir( word dir );
void    configure_IO_JTAG( void );
void    IO_3state(void);