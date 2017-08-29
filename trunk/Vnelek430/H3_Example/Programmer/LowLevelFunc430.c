/*==========================================================================*\
|                                                                            |
| LowLevelFunc430.c                                                          |
|                                                                            |
| Low Level Functions regarding user's hardware                              |
|----------------------------------------------------------------------------|
| Project:              MSP430 Replicator                                    |
| Developed using:      IAR Embedded Workbench 3.40B [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               FRGR                                                 |
| Version:              1.6                                                  |
| Initial Version:      04-17-02                                             |
| Last Change:          05-24-12                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 04/02 FRGR        Initial version.                                     |
| 1.1 04/02 FRGR        Included SPI mode to speed up shifting function by 2.|
| 1.2 06/02 ALB2        Formatting changes, added comments.                  |
| 1.3 08/02 ALB2        Initial code release with Lit# SLAA149.              |
| 1.4 09/05 SUN1        Software delays redesigned to use TimerA harware;    |
|                       see MsDelay() routine. Added TA setup                |
| 1.5 12/05 STO         Adapted for 2xx devices with SpyBiWire using 4JTAG   |
| 1.6 08/08 WLUT        Cleaned up InitTarget() for JTAG init sequence.      |
| 1.7 05/09 GC (Elprotronic)  Added support for the new hardware - VNELEK-PROG   |
| 1.8 07/09 FB          Added support for Spy-Bi-Wire and function           |
|                        configure_IO_SBW( void )                            |
| 1.9 05/12 RL          Updated commentaries                                 |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/


//! \file LowLevelFunc430.c
//! \brief Low Level Functions regarding user's Hardware
//! \author Wolfgang Lutsch
//! \date 09/16/2009
//! \version 1.9

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "LowLevelFunc430.h"
#include "JTAGfunc430.h"

/****************************************************************************/
/* GLOBAL VARIABLES                                                         */
/****************************************************************************/

//! \brief Holds the value of TDO-bit
byte tdo_bit;

//! \brief Holds the last value of TCLK before entering a JTAG sequence
byte TCLK_saved = TDI;

/****************************************************************************/
/* FUNCTIONS                                                                */
/****************************************************************************/


//----------------------------------------------------------------------------
//! \brief Shift a value into TDI (MSB first) and simultaneously shift out a 
//! value from TDO (MSB first).
//! \param[in] Format (number of bits shifted, 8 (F_BYTE), 16 (F_WORD), 
//! 20 (F_ADDR) or 32 (F_LONG))
//! \param[in] Data (data to be shifted into TDI)
//! \return unsigned long (scanned TDO value)
word Shift(word Format, word Data)
{
    word tclk = StoreTCLK();        // Store TCLK state;
    word TDOword = 0x0000;          // Initialize shifted-in word
    word MSB = 0x0000;

    // Shift via port pins, no coding necessary
    volatile word i;

   (Format == F_WORD) ? (MSB = 0x8000) : (MSB = 0x80);

   for (i = Format; i > 0; i--)
   {
       ((Data & MSB) == 0) ? ClrTDI() : SetTDI();
        
        Data <<= 1;
        if (i == 1)                 // Last bit requires TMS=1
            SetTMS();
        
        ClrTCK();
        SetTCK();

        TDOword <<= 1;              // TDO could be any port pin
        if (ScanTDO() != 0)
            TDOword++;
    }

    // common exit
    RestoreTCLK(tclk);              // restore TCLK state
    
    // JTAG FSM = Exit-DR
    ClrTCK();
    SetTCK();

    // JTAG FSM = Update-DR
    ClrTMS();
    ClrTCK();
    SetTCK();

    // JTAG FSM = Run-Test/Idle
    return(TDOword);
}

//----------------------------------------------------------------------------
//! \brief Initialization of the Controller Board
void InitController(void)
{
    // Setup timer_A for hardware delay
    TA0CTL = TACLR;
    TA0CTL = ID_3+TASSEL_2;			// Timer_A source: SMCLK/8 = 3MHz
    TA0CCR0 = ONEMS;                // Load CCR0 with delay... (1ms delay)

    // Set LED ports direction
    LED_DIR |= LED_GREEN + LED_RED;

    // Turn on GREEN LED at the startup
    LED_OUT |= LED_GREEN;
    
    // Turn off RED LED at the startup
    LED_OUT &= ~LED_RED;

    SetTargetVcc();
}


//----------------------------------------------------------------------------
//! \brief Set target Vcc (supplied from Programmer)
void SetTargetVcc (void)
{
	if(GetTargetVcc() == TARGET_HAS_0V)
	{
		// Vcc control pin is output
	    TVCC_EN_DIR |= TVCC_EN_BIT;

	    // Enable Vcc on JTAG_VCC
	    TVCC_EN_OUT |= TVCC_EN_BIT;

	    MsDelay(50);
	}
}

//----------------------------------------------------------------------------
//! \brief Clear target Vcc (supplied from Programmer)
void ClearTargetVcc (void)
{
	// Vcc control pin is output
    TVCC_EN_DIR |= TVCC_EN_BIT;

    // Disable Vcc on JTAG_VCC
    TVCC_EN_OUT &= ~TVCC_EN_BIT;

    MsDelay(50);
}

//----------------------------------------------------------------------------
//! \brief Determine target VCC
//! \return word (target VCC)
byte GetTargetVcc(void)
{
	// Check pin is input
    TVCC_RD_DIR &= ~ TVCC_RD_BIT;

    if(TVCC_RD_IN & TVCC_RD_BIT)
    {
		return TARGET_HAS_3V;
    }
	else
	{
		return TARGET_HAS_0V;
	}
}

//----------------------------------------------------------------------------
//! \brief Set the direction for the TDI pin
//! \param[in] dir (0 = IN - from target to VNELEK-PROG, !0 = OUT)
void TDI_dir(word dir)
{    
    if( dir == 0 )      	// Direction IN
        JTAGDIR &= ~TDI;
    else
        JTAGDIR |= TDI;		// Direction OUT
}

//----------------------------------------------------------------------------
//! \brief Set the direction for the TDO pin
//! \param dir (0 = IN - from target to VNELEK-PROG, !0 = OUT)
void TDOI_dir(word dir)
{
	if( dir == 0 )      	// Direction IN
        JTAGDIR &= ~TDO;
    else
        JTAGDIR |= TDO;		// Direction OUT
}
//----------------------------------------------------------------------------
//! \brief Set the direction for the TEST pin
//! \param dir (0 = IN - from target to VNELEK-PROG, !0 = OUT)
void TEST_dir(word dir)
{
    if( dir == 0 )             // Direction IN - from target to REP430
    {
        JTAGDIR &= ~TEST;      // Switch MSP port to input first - to avoid two outputs on the line
    }
    else
    {
        JTAGDIR |= TEST;       // Switch MSP port to output
    }
}
//----------------------------------------------------------------------------
//! \brief Set the direction for the TMS pin
//! \param dir (0 = IN - from target to VNELEK-PROG, !0 = OUT)
void TMS_dir(word dir)
{
    if( dir == 0 )             // Direction IN - from target to REP430
    {
        JTAGDIR &= ~TMS;       // Switch MSP port to input first - to avoid two outputs on the line
    }
    else
    {
        JTAGDIR |= TMS;       // Switch MSP port to output
    }
}
//----------------------------------------------------------------------------
//! \brief Set the direction for the RST pin
//! \param dir (0 = IN - from target to VNELEK-PROG, !0 = OUT)
void RST_dir(word dir)
{
    if( dir == 0 )            // Direction IN - from target to REP430
    {
        JTAGDIR &= ~RST;      // Switch MSP port to input first - to avoid two outputs on the line
    }
    else
    {
        JTAGDIR |= RST;       // Switch MSP port to output
    }
}
//----------------------------------------------------------------------------
//! \brief Set the direction for the TCK pin
//! \param dir (0 = IN - from target to VNELEK-PROG, !0 = OUT)
void TCK_dir(word dir)
{
    if( dir == 0 )
        JTAGDIR &= ~TCK;
    else
        JTAGDIR |= TCK; 
}

//----------------------------------------------------------------------------
//! \brief Set JTAG pins to output direction - from VNELEK-PROG to target
void ConfigureIO_JTAG(void)
{
    TDI_dir( 1 );
    TDOI_dir( 0 );
    TEST_dir( 1 );
    TMS_dir( 1 );
    RST_dir( 1 );
    TCK_dir( 1 );
}


//----------------------------------------------------------------------------
//! \brief Set all JTAG pins to input direction - from target to VNELEK-PROG
void IO_3state(void)
{
    TDI_dir( 0 );
    TDOI_dir( 0 );
    TEST_dir( 0 );
    TMS_dir( 0 );
    RST_dir( 0 );
    TCK_dir( 0 );
}

//----------------------------------------------------------------------------
//! \brief This function switches TDO to Input (used for fuse blowing)
void TDOisInput(void)
{
    TDI_dir( 0 );       // Release TDI pin on target
    TDOI_dir( 1 );      // Switch TDI --> TDO
}

//----------------------------------------------------------------------------
//! \brief Set up I/O pins for JTAG communication
void DrvSignals(void)
{
	// Pins all I/Os
    JTAGSEL  = 0x00;

    JTAGOUT  = (TEST | TDI | TMS | TCK | TCLK | RST);

    // Config pins direction
    ConfigureIO_JTAG();
}

//----------------------------------------------------------------------------
//! \brief Release I/O pins
void RlsSignals(void)
{
    IO_3state();
}

//----------------------------------------------------------------------------
//! \brief Initialization of the Target Board (switch voltages on, preset JTAG pins)
//! \details For devices with normal 4 wires JTAG  (JTAG4SBW = 0)
//! For devices with Spy-Bi-Wire to work in 4 wires JTAG (JTAG4SBW = 1)
void InitTarget(void)
{
    DrvSignals();
    SetTargetVcc();
}

//----------------------------------------------------------------------------
//! \brief Release Target Board (switch voltages off, JTAG pins are HI-Z)
void ReleaseTarget(void)
{
    RlsSignals();
    SetTargetVcc();
}

//----------------------------------------------------------------------------
//! \brief Delay function (resolution is 1 ms)
//! \param[in] milliseconds (number of ms, max number is 0xFFFF)
void MsDelay(word milliseconds)
{
   word i;
   for(i = milliseconds; i > 0; i--)
   {
        TA0CCTL0 &= ~CCIFG;             // Clear the interrupt flag
        TA0CTL |= TACLR + MC_1 +		// Clear timer, UP to CCR0
        		  ID_3 + TASSEL_2;		// SMCLK, /8
        TA0CCR0 = ONEMS;                // Load CCR0 with delay... (1ms delay)

        while ((TA0CCTL0 & CCIFG)==0);  // Wait until the Timer elapses
        TA0CTL = 0x00;    				// Stop Timer
   }
}

//----------------------------------------------------------------------------
//! \brief Delay function (resolution is ~1 us)
//! \param[in] microseconds (number of ms, max number is 0xFFFF)
void usDelay(word microseconds)
{
    do
    {
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
    }
    while (--microseconds > 0);
}

//----------------------------------------------------------------------------
//! \brief This function generates Amount strobes with the Flash Timing
//! Generator
//! \details Frequency fFTG = 257..476kHz (t = 3.9..2.1us).
//! Used freq. in procedure - 400 kHz
//! User knows target frequency, instruction cycles, C implementation.
//! No. of MCKL cycles - 18MHz/400 kHz = 45 or 12MHz/400 kHz = 30
//! \param[in] Amount (number of strobes to be generated)
void TCLKstrobes(word Amount)
{
    volatile word i;

    // This implementation has 45 (MCLK = 24MHz)
    // or 30 (MCLK 24MHz) body cycles! -> 400kHz
    // DO NOT MODIFY IT !

    for (i = Amount; i > 0; i--)
    {
        JTAGOUT |=  TCLK;       // Set TCLK
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP();

        JTAGOUT &= ~TCLK;       // Reset TCLK
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP(); _NOP();
        _NOP();
    }
}


//----------------------------------------------------------------------------
//! \brief This function controls the status LEDs depending on the status
//! argument. It stops program in error case.
//! \param[in] status (4 stati - can be extended to 8 - possible for 3 LEDs - 
//! Yellow,Green,Red)
//! \param[in] index (additional number for detailed diagnostics or watch 
//! variable during debugging phase)
void ShowStatus(word status, word index)
{
    All_LEDs_off();

    switch(status)
    {
        case STATUS_ERROR:
            LED_red_on();               // Switch red LED on
            break;
        case STATUS_SUCCESS:                 // Switch green LED on
            LED_green_on();
            break;
		default:
			break;
    }
}


/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
