/*==========================================================================*\
|                                                                            |
| JTAGfunc430Xv2.c                                                           |
|                                                                            |
| JTAG Control Sequences for Erasing / Programming / Fuse Programming        |
|----------------------------------------------------------------------------|
| Project:              JTAG Functions                                       |
| Developed using:      IAR Embedded Workbench 3.40A [Kickstart]             |
|             and:      Code Composer Eessentials v3                         |
|----------------------------------------------------------------------------|
| Author:               WLUT                                                 |
| Version:              1.9                                                  |
| Initial Version:      08-28-08                                             |
| Last Change:          03-14-13                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 08/08 WLUT        Initial version.                                     |
| 1.1 10/08 WLUT        Fixed VerifyPSA_430Xv2(). The previous version was   |
|                       driving StartAddr-2 on the devices' address bus.     |
|                       This can cause security violations in the 5xx system.|
|                       The fixed version of the algorithm is now operating  |
|                       only within the specified memory range which is to   |
|                       be verified.                                         |
|                       Removed 'fuse check' in ResetTap() for SPYBIWIREMODE.|
|                       Fuse check is not required for 5xx.                  |
|                       Added correct Spy-Bi-Wire entry sequence to StartJtag|
| 1.2 07/09 FB          Fix JTAG 17 BUG, Add support for new Replecator      |
|                       Fix Spy Bi wire entry sequence                       |
| 1.3 08/09 FB          Changed VerifyPSA_430Xv2 to clock through PSA during |
|                       TCLK low phase                                       |
| 1.3 10/10 FB          changed function StartJtag(), that the device could  |
|                       be held in reset to shift in the "magic pattern"     |
|                       to stop user code excecution                         |
|                       added new function ConnectJTAG() to enstablish the   |
|                       physical JTAG connection                             |
|                       added new function MagicPattern() to stop user code  |
|                       execution and set the target device into LPM4        |
| 1.4 GC (Elprotronic)  Bug in GetCoreID() --> DeviceIdPointer has been fixed|
| 1.5 08/11 FB          Added FRAM support functions:                        |
|                           ProgramLockKey() - Set JTAG Lock Key             |
|                           UnlockDevice() - Open a device with JTAG password|
|                           WriteFram_430Xv2 - Write FRAM memory             |
|                           EraseFRAM_430Xv2 - Erase FRAM memory             |
|                           DisableMPU430Xv2 - Disable Memory Protection Unit|
| 1.6 10/11 FB/RL       Fixed JTAG mailbox ready check                       |
|                       Added function descriptions                          |
|                       Added FRAM write through JTAG mailbox                |
| 1.7 06/12 RL          Updated commentaries                                 |
| 1.8 10/12 RL          Removed hard-coded RAM addresses - RAM start address |
|                       has to be specified in the config header file        |
| 1.9 03/13 RL/MD       Added unlock functions for Info A & BSL              |
|----------------------------------------------------------------------------|
| Designed 2008 by Texas Instruments Germany                                 |
\*==========================================================================*/

//! \file JTAGfunc430Xv2.c
//! \brief JTAG Control Sequences for Erasing / Programming / Fuse Programming
//! \author Berenbrinker Florian
//! \date 03/14/2013
//! \version 1.9

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "FlashErase.c"
#include "FlashWrite.c"
#include "FramWrite.c"
#include "FramErase.c"
#include "LowLevelFunc430Xv2.h"
#include "JTAGfunc430Xv2.h"


/****************************************************************************/
/* Low level routines for accessing the target device via JTAG:             */
/****************************************************************************/

//----------------------------------------------------------------------------
//! \brief Function for shifting a given 16-bit word into the JTAG data
//! register through TDI.
//! \param[in] word data (16-bit data, MSB first)
//! \return word (value is shifted out via TDO simultaneously)
static word DR_Shift16(word data)
{
    // JTAG FSM state = Run-Test/Idle
    SetTMS();
    ClrTCK();
    SetTCK();

    // JTAG FSM state = Select DR-Scan
    ClrTMS();
    ClrTCK();
    SetTCK();
    // JTAG FSM state = Capture-DR
    ClrTCK();
    SetTCK();

    // JTAG FSM state = Shift-DR, Shift in TDI (16-bit)
    return(AllShifts(F_WORD, data));
    // JTAG FSM state = Run-Test/Idle
}

//----------------------------------------------------------------------------
//! \brief Function for shifting a given 20-bit address word into the
//! JTAG address register through TDI.
//! \param[in] unsigned long address (20-bit address word, MSB first)
//! \return unsigned long TDOvalue (is shifted out via TDO simultaneously)
static unsigned long DR_Shift20(unsigned long address)
{
    // JTAG FSM state = Run-Test/Idle
    SetTMS();
    ClrTCK();
    SetTCK();

    // JTAG FSM state = Select DR-Scan
    ClrTMS();
    ClrTCK();
    SetTCK();
    // JTAG FSM state = Capture-DR
    ClrTCK();
    SetTCK();

    // JTAG FSM state = Shift-DR, Shift in TDI (16-bit)
    return(AllShifts(F_ADDR, address));
    // JTAG FSM state = Run-Test/Idle
}

//----------------------------------------------------------------------------
//! \brief Function for shifting a new instruction into the JTAG instruction
//! register through TDI (MSB first, but with interchanged MSB - LSB, to
//! simply use the same shifting function, Shift(), as used in DR_Shift16).
//! \param[in] byte Instruction (8bit JTAG instruction, MSB first)
//! \return word TDOword (value shifted out from TDO = JTAG ID)
static word IR_Shift(byte instruction)
{
    // JTAG FSM state = Run-Test/Idle
    SetTMS();
    ClrTCK();
    SetTCK();
    // JTAG FSM state = Select DR-Scan
    ClrTCK();
    SetTCK();

    // JTAG FSM state = Select IR-Scan
    ClrTMS();
    ClrTCK();
    SetTCK();
    // JTAG FSM state = Capture-IR
    ClrTCK();
    SetTCK();

    // JTAG FSM state = Shift-IR, Shift in TDI (8-bit)
    return(AllShifts(F_BYTE, instruction));
    // JTAG FSM state = Run-Test/Idle
}

//----------------------------------------------------------------------------
//! \brief Reset target JTAG interface and perform fuse-HW check.
static void ResetTAP(void)
{
    word i;

    // process TDI first to settle fuse current
    SetTDI();
    SetTMS();
    SetTCK();

    // Reset JTAG FSM
    for (i = 6; i > 0; i--)
    {
        ClrTCK();
        SetTCK();
    }
    // JTAG FSM is now in Test-Logic-Reset
    ClrTCK();
    ClrTMS();
    SetTCK();
    ClrTCK();    
    SetTCK();
    ClrTCK();    
    // JTAG FSM is now in Run-Test/IDLE
}

//----------------------------------------------------------------------------
//! \brief Function to execute a Power-On Reset (POR) using JTAG CNTRL SIG 
//! register
//! \return word (STATUS_SUCCESS if target is in Full-Emulation-State afterwards,
//! STATUS_ERROR otherwise)
static word ExecutePOR_430Xv2(void)
{
    word i = 0;

    // provide one clock cycle
    ClrTCLK();
    SetTCLK();

    // prepare access to the JTAG CNTRL SIG register  
    IR_Shift(IR_CNTRL_SIG_16BIT);
    // release CPUSUSP signal and apply POR signal
    DR_Shift16(0x0C01);
    // release POR signal again
    DR_Shift16(0x0401);
  
    // provide 5 clock cycles
    for (i = 0; i < 5; i++)
    {
        ClrTCLK();
        SetTCLK();
    }
    // now set CPUSUSP signal again
    DR_Shift16(0x0501);
    // and provide one more clock
    ClrTCLK();
    SetTCLK();
    // the CPU is now in 'Full-Emulation-State'
  
    // disable Watchdog Timer on target device now by setting the HOLD signal
    // in the WDT_CNTRL register
    WriteMem_430Xv2(F_WORD, 0x015C, 0x5A80);

    // Check if device is in Full-Emulation-State again and return status
    IR_Shift(IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(0) & 0x0301)
    {
      return(STATUS_SUCCESS);
    }
  
    return(STATUS_ERROR);
}

//----------------------------------------------------------------------------
//! \brief Load a given address into the target CPU's program counter (PC).
//! \param[in] unsigned long Addr (destination address)
static void SetPC_430Xv2(unsigned long Addr)
{
    unsigned short Mova;
    unsigned short Pc_l;
  
    Mova  = 0x0080;
    Mova += (unsigned short)((Addr>>8) & 0x00000F00);
    Pc_l  = (unsigned short)((Addr & 0xFFFF));
  
    // Check Full-Emulation-State at the beginning
    IR_Shift(IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(0) & 0x0301)
    {
        // MOVA #imm20, PC
        ClrTCLK();
        // take over bus control during clock LOW phase
        IR_Shift(IR_DATA_16BIT);
        SetTCLK();
        DR_Shift16(Mova);
        // insert on 24.03.2010 Florian
        ClrTCLK();
        // above is just for delay
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x1400);
        IR_Shift(IR_DATA_16BIT);
        ClrTCLK();
        SetTCLK();
        DR_Shift16(Pc_l);
        ClrTCLK();
        SetTCLK();
        DR_Shift16(0x4303);    
        ClrTCLK();
        IR_Shift(IR_ADDR_CAPTURE);    
        DR_Shift20(0x00000);
    }
}

//----------------------------------------------------------------------------
//! \brief Read a 32bit value from the JTAG mailbox.
//! \return unsigned long (32bit value from JTAG mailbox)
static unsigned long i_ReadJmbOut(void)
{
    unsigned short sJMBINCTL;
    unsigned long  lJMBOUT = 0;
    unsigned short sJMBOUT0, sJMBOUT1;
  
    sJMBINCTL = 0;
  
    IR_Shift(IR_JMB_EXCHANGE);// start exchange
    lJMBOUT = DR_Shift16(sJMBINCTL);
  
    if(lJMBOUT & OUT1RDY)// check if new data available 
    {
        sJMBINCTL |= JMB32B + OUTREQ;
        lJMBOUT  = DR_Shift16(sJMBINCTL);
        sJMBOUT0 = (unsigned short)DR_Shift16(0);
        sJMBOUT1 = (unsigned short)DR_Shift16(0);
    
        lJMBOUT = ((unsigned long)sJMBOUT1<<16) + sJMBOUT0;
    }
  
    return lJMBOUT;
}

//----------------------------------------------------------------------------
//! \brief Write a 16bit value into the JTAG mailbox system.
//! The function timeouts if the mailbox is not empty after a certain number
//! of retries.
//! \param[in] word lJMBIN (data to be shifted into mailbox)
static void i_WriteJmbIn(word lJMBIN)
{
    unsigned short sJMBINCTL; // create in control  
    unsigned short sJMBOUTCTL; //create out control
    unsigned long Timeout = 0; // set up timeout
    
    sJMBINCTL = INREQ; 
  
    // // fix for Jtag 17 error
// #if(DEVICE_HAS_JTAG17 == 1)
    // IR_Shift(IR_JMB_EXCHANGE); // set up instrucion to start mailbox exchange
    // do
    // { 
        // sJMBOUTCTL = (unsigned short)DR_Shift16(0); // shift data into mailbox
        // Timeout++;
    // }
    // while(!(sJMBOUTCTL & IN0RDY) && (Timeout < 3000));
  
    // if(IN0RDY & sJMBOUTCTL)
    // {
        // sJMBOUTCTL = (unsigned short)DR_Shift16(sJMBINCTL);
    // }  
// #else // no Jtag 17 error
    // IR_Shift(IR_JMB_EXCHANGE); // set up instrucion to start mailbox exchange
    // do
    // { 
        // sJMBOUTCTL = (unsigned short)DR_Shift16(sJMBINCTL); // shift data into mailbox
        // Timeout++;
    // }
    // while(!(sJMBOUTCTL & IN0RDY) && Timeout < 3000);  
// #endif 

	// No Jtag 17 error
    IR_Shift(IR_JMB_EXCHANGE); // set up instrucion to start mailbox exchange
    do
    { 
        sJMBOUTCTL = (unsigned short)DR_Shift16(sJMBINCTL); // shift data into mailbox
        Timeout++;
    }
    while(!(sJMBOUTCTL & IN0RDY) && Timeout < 3000);  
    
    if(Timeout < 3000)
    {
        DR_Shift16(lJMBIN);
    }
}

//----------------------------------------------------------------------------
//! \brief This function compares the computed PSA (Pseudo Signature Analysis)
//! value to the PSA value shifted out from the target device.
//! It is used for very fast data block write or erasure verification.
//! \param[in] unsigned long StartAddr (Start address of data block to be checked)
//! \param[in] unsigned long Length (Number of words within data block)
//! \param[in] word *DataArray (Pointer to array with the data, 0 for Erase Check)
//! \return word (STATUS_SUCCESS if comparison was successful, STATUS_ERROR otherwise)
word VerifyPSA_430Xv2(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    word TDOword;
    unsigned long i;
    const word POLY = 0x0805;             // Polynom value for PSA calculation
    word PSA_CRC = (word)(StartAddr-2);   // Start value for PSA calculation

    ExecutePOR_430Xv2();
    
    SetPC_430Xv2(StartAddr);
    
    SetTCLK();
    
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x0501);

    IR_Shift(IR_DATA_16BIT);
    DR_Shift16(PSA_CRC);

    IR_Shift(IR_DATA_PSA);   
    
    for (i = 0; i < Length; i++)
    {      
        // Calculate the PSA (Pseudo Signature Analysis) value
        if ((PSA_CRC & 0x8000) == 0x8000)
        {
            PSA_CRC ^= POLY;
            PSA_CRC <<= 1;
            PSA_CRC |= 0x0001;
        }
        else
        {
            PSA_CRC <<= 1;
        }
        // if pointer is 0 then use erase check mask, otherwise data
        &DataArray[0] == 0 ? (PSA_CRC ^= 0xFFFF) : (PSA_CRC ^= DataArray[i]);      
        
        ClrTCLK();
        
        // Clock through the PSA     
      
        ClrTCK();
     
        SetTMS();
        SetTCK();            // Select DR scan
        ClrTCK();
        ClrTMS();
        
        SetTCK();            // Capture DR
        ClrTCK();
       
        SetTCK();            // Shift DR
        ClrTCK();
         
        SetTMS();        
        SetTCK();          // Exit DR  
        ClrTCK();
        
         // Set JTAG FSM back into Run-Test/Idle
        SetTCK();
        ClrTMS();
        ClrTCK();
        SetTCK();          
        
        SetTCLK();      
    }
    
    IR_Shift(IR_SHIFT_OUT_PSA);
    TDOword = DR_Shift16(0x0000);     // Read out the PSA value
     
    ExecutePOR_430Xv2();

    return((TDOword == PSA_CRC) ? STATUS_SUCCESS : STATUS_ERROR);
}

/****************************************************************************/
/* High level routines for accessing the target device via JTAG:            */
/*                                                                          */
/* For the following section, the user is relieved from coding anything.    */
/* To provide better understanding and clearness, some functionality is     */
/* coded generously. (Code and speed optimization enhancements may          */
/* be desired)                                                              */
/****************************************************************************/


//static word JtagId = 0;
//static word CoreId = 0;
//static unsigned long DeviceIdPointer = 0;
//static word DeviceId = 0;

//----------------------------------------------------------------------------
//! \brief Function to set up the JTAG pins
static void ConnectJTAG()
{
    // drive JTAG/TEST signals
    DrvSignals();
    MsDelay(15);             // delay 15ms
}

//----------------------------------------------------------------------------
//! \brief Function to start the JTAG communication
//! \param[in] byte states (reset state)
static void StartJtag(byte states)
{
    // reset TEST logic  
    ClrTST();
    MsDelay(20);             // delay 10ms
    
    if(states == RSTLOW_JTAG)
    {   
        // Clear Reset pin
        ClrRST();
    }
    else
    {   
        // Set Reset pin = 1 
        SetRST();
    }  
    
    SetTST();
    // activate TEST logic
    MsDelay(25); 
    
    // phase 1
    if(states == RSTLOW_JTAG || states == RSTHIGH_JTAG)
    {
        //Set Reset pin =0 
        ClrRST();
    }
    else
    {
        SetRST();
    }  
    usDelay(40); 
    
    // phase 2 -> TEST pin to 0, no change on RST pin
    if(states == RSTLOW_JTAG || states == RSTHIGH_JTAG)
    { 
        // for 4-wire JTAG
        ClrTST();
    }
    
    // phase 3
    if(states == RSTLOW_JTAG)
    {
        SetRST();
    }
    usDelay(1); 
    
    // phase 4 -> TEST pin to 1, no change on RST pin
    if(states == RSTLOW_JTAG || states == RSTHIGH_JTAG)
    { 
        // for 4-wire JTAG
        SetTST();
    }
    usDelay(40); 
    
    // phase 5
    if(states == RSTHIGH_JTAG)
    {
        SetRST();
    }
    MsDelay(5);        
}

//----------------------------------------------------------------------------
//! \brief Function to stop the JTAG communication by releasing the JTAG signals
static void StopJtag (void)
{
    // release JTAG/TEST signals
    RlsSignals();
    MsDelay(15);             // delay 15ms
}

//----------------------------------------------------------------------------
extern void configure_IO_JTAG( void );

//----------------------------------------------------------------------------
//! \brief Function to enable JTAG communication with a target. Use JSBW mode
//!  if device is in LPM5 mode.
//! \return word (JTAG_ID91(0x91) if connection was established successfully, 
//! invalid JTAG ID (0x1) otherwise)
static word magicPattern(void)
{
    word deviceJtagID = 0;
    
    // Enable the JTAG interface to the device.
    ConnectJTAG();     
    // Apply again 4wire/SBW entry Sequence.
    // set ResetPin = 0
	
    StartJtag(RSTLOW_JTAG);
         
    // reset TAP state machine -> Run-Test/Idle
    ResetTAP();  
    // feed JTAG mailbox with magic pattern
    if(((word)IR_Shift(IR_JMB_EXCHANGE)) == JTAG_ID91)
    {
        // check if JTAG mailbox is ready & perform input request
        if((DR_Shift16(0x0001) & 0xFFF0) == 0x1200)
        {
            // and feed in magic pattern
            DR_Shift16(0xA55A);
            // Enable the JTAG interface to the device.
            ConnectJTAG();
                        
            // Apply again 4wire/SBW entry Sequence.                        
            StartJtag(RSTHIGH_JTAG);

            ResetTAP();  // reset TAP state machine -> Run-Test/Idle
            
            deviceJtagID = (word)IR_Shift(IR_CNTRL_SIG_CAPTURE);
            
            if(deviceJtagID == JTAG_ID91)
            {
                // // if Device is in LPM.x5 -> reset IO lock of JTAG pins and Configure it for debug
// #ifdef LPM5_ACTIVATED
                // {
// #ifdef SPYBIWIRE_MODE // 2 wire mode
                    // IR_Shift(IR_TEST_3V_REG);
                    // MsDelay(10);  
                    // DR_Shift16(0x4020);
                    // MsDelay(10);    
// #endif
                // }   
// #endif
                return deviceJtagID;
            }
        }
    }
	// if Device is in LPM.x5 -> reset IO lock of JTAG pins and Configure it for debug
// #ifdef LPM5_ACTIVATED
    // {
// #ifdef SPYBIWIREJTAG_IF // run 2 wire mode of 4 wire selection because 4wire pins are locked by JTAG lock
      
        // ConnectJTAG();
        // StartJtagJSbw(RSTLOW_SBW);        
        // jResetJtagTap();
        // JsbwMagicPattern();
        // jRelease();          
        // MsDelay(60); 
        
        // // reset JTAG lock
        // configure_IO_JTAG();                    
        // StartJtagJSbw(RSTHIGH_SBW);   
        // jResetJtagTap();
        // jsbwJtagUnlock();
        // MsDelay(60); 
        
        // ConnectJTAG();
        // StartJtag(RSTHIGH_JTAG);              
        // ResetTAP();  // reset TAP state machine -> Run-Test/Idle    
        // MsDelay(60); 
        // deviceJtagID = (word)IR_Shift(IR_CNTRL_SIG_CAPTURE);
        // if(deviceJtagID == JTAG_ID91)
        // {
            // return deviceJtagID;
        // }
// #endif
    // } 
// #endif
    return 1;  // return 1 as an invalid JTAG ID
}

//----------------------------------------------------------------------------
//! \brief This function unlocks the Fram memory when a JTAG password is set.
//! \param[in] unsigned short* password (Pointer to array containing the JTAG
//! Password)
//! \param[in] unsigned long passwordLength (length of the password in words)
//! \return word (STATUS_SUCCESS if memory unlock was successful, STATUS_ERROR
//! otherwise)
word UnlockDeviceXv2(unsigned short* password, unsigned long passwordLength)
{    
  /*----------------------------------------------------------------------- */ 
  /*            phase 1 of device entry using a user password               */
  /*------------------------------------------------------------------------*/ 
  
    // Enable the JTAG interface to the device.
    ConnectJTAG();     
    // Apply again 4wire/SBW entry Sequence.
    // set ResetPin =0
         
    StartJtag(RSTLOW_JTAG);

    // reset TAP state machine -> Run-Test/Idle
    ResetTAP();
    // shift in JTAG mailbox exchange request
    if(((word)IR_Shift(IR_JMB_EXCHANGE)) == JTAG_ID91)
    {   
        // check if JTAG mailbox is ready & perform input request
        if((DR_Shift16(0x0011) & 0xFFF0) == 0x1200)
        {
            // shift in A55A to set device into lpm 4
            DR_Shift16(0xA55A);       
            // and feed in perform password exchange request
            DR_Shift16(0x1E1E);          
        }
        else
        {
            return(STATUS_ERROR);
        } 
    }
    else
    {
        return(STATUS_ERROR);    
    }
    StopJtag();
    /*----------------------------------------------------------------------- */ 
    /*            phase 2 of device entry using a user password               */
    /*------------------------------------------------------------------------*/
    // Enable the JTAG interface to the device.
    ConnectJTAG();  
    // Apply again 4wire/SBW entry Sequence.

    StartJtag(RSTHIGH_JTAG);

    // reset TAP state machine -> Run-Test/Idle
    ResetTAP();
    // shift in JTAG mailbox exchange request      
    if( IR_Shift(IR_JMB_EXCHANGE) == JTAG_ID91)
    {
        unsigned short i = 0;
        unsigned short timeOut = 0;
        
        while(i < passwordLength)
        {
            MsDelay(10);
            // check if JTAG mailbox is ready & perform input request
            if((DR_Shift16(0x0001)) && (timeOut < 600))
            {
                // and feed in password beginning with the length
                DR_Shift16(password[i]);  
                MsDelay(1);
                i++;
                timeOut =0;
            }
            else // if JTAG mailbox is not ready try again until timeout is reached
            {
                timeOut++;
                if(timeOut == 600)
                {
                    return(STATUS_ERROR);  
                }
            } 
        }    
    }
    else
    {
         return(STATUS_ERROR);
    }        
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief Function to determine & compare core identification info 
//! \return word (STATUS_SUCCESS if correct JTAG ID was returned, STATUS_ERROR 
//! otherwise)
static word GetCoreID (void)
{
    word i;
    for (i = 0; i < MAX_ENTRY_TRY; i++)
    {
        // initialize JtagId with an invalid value
        JtagId = 0;  
        // release JTAG/TEST signals to savely reset the test logic
        StopJtag();        
        // establish the physical connection to the JTAG interface
        ConnectJTAG();               
        // Apply again 4wire/SBW entry Sequence. 
        // set ResetPin =1    

        StartJtag(RSTHIGH_JTAG);

        // reset TAP state machine -> Run-Test/Idle
        ResetTAP();  
        // shift out JTAG ID
        JtagId = (word)IR_Shift(IR_CNTRL_SIG_CAPTURE);  
         
        // break if a valid JTAG ID is being returned
        if((JtagId == JTAG_ID91) || (JtagId == JTAG_ID99))
        {
            break;
        }
    }
    if(i >= MAX_ENTRY_TRY)
    {         
    // if connected device is MSP4305438 JTAG Mailbox is not usable
#ifdef ACTIVATE_MAGIC_PATTERN
        for(i = 0; i < MAX_ENTRY_TRY; i++)
        {
            // if no JTAG ID is  beeing returnd -> apply magic pattern to stop user cd excecution 
            JtagId = magicPattern();
          
            if((JtagId == 1) || (i >= MAX_ENTRY_TRY))
            {
                // if magic pattern failed and 4 tries passed -> return status error
                return(STATUS_ERROR);
            }
            else
            {
                break; 
            }
        }
        // For MSP430F5438 family mailbox is not functional in reset state.
        // Because of this issue the magicPattern is not usable on MSP430F5438 family devices
#else
    return(STATUS_ERROR);
#endif   
    }
    if((JtagId == JTAG_ID91) || (JtagId == JTAG_ID99))                         //****************************
    {
        return(STATUS_SUCCESS);
    }
    else
    {
        return(STATUS_ERROR);
    } 
}

//----------------------------------------------------------------------------
//! \brief Function to determine & compare core identification info (Xv2)
//! \return word (STATUS_SUCCESS if correct JTAG ID was returned, STATUS_ERROR 
//! otherwise)
static word GetCoreipIdXv2()
{
    IR_Shift(IR_COREIP_ID);
    CoreId = DR_Shift16(0);
    if(CoreId == 0)
    {
        return(STATUS_ERROR);
    }
    IR_Shift(IR_DEVICE_ID);
    DeviceIdPointer = DR_Shift20(0);
    // The ID pointer is an un-scrambled 20bit value    
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief Function to resync the JTAG connection and execute a Power-On-Reset
//! \return word (STATUS_SUCCESS if operation was successful, STATUS_ERROR 
//! otherwise)
static word SyncJtag_AssertPor (void)
{
    word i = 0;
   
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x1501);                  // Set device into JTAG mode + read
    
    if ((IR_Shift(IR_CNTRL_SIG_CAPTURE) != JTAG_ID91) && (IR_Shift(IR_CNTRL_SIG_CAPTURE) != JTAG_ID99)) //****************
    {
      return(STATUS_ERROR);
    }
    // wait for sync
    while(!(DR_Shift16(0) & 0x0200) && i < 50)
    {
        i++;
    };
    // continues if sync was successful
    if(i >= 50)
    {
        return(STATUS_ERROR);
    }

    // execute a Power-On-Reset
    if(ExecutePOR_430Xv2() != STATUS_SUCCESS)
    {
        return(STATUS_ERROR);
    }
  
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief Function to take target device under JTAG control. Disables the 
//! target watchdog. Sets the global DEVICE variable as read from the target 
//! device.
//! \return word (STATUS_ERROR if fuse is blown, incorrect JTAG ID or
//! synchronizing time-out; STATUS_SUCCESS otherwise)
word GetDevice_430Xv2(void)
{  
    if(GetCoreID () != STATUS_SUCCESS)
    { 
        return(STATUS_ERROR);
    }
    if (IsLockKeyProgrammed())                 // Stop here if fuse is already blown
    {
        return(STATUS_FUSEBLOWN);
    }    
    if (GetCoreipIdXv2()!= STATUS_SUCCESS)
    { 
        return(STATUS_ERROR);
    }
    if(SyncJtag_AssertPor() != STATUS_SUCCESS)
    {
        return(STATUS_ERROR);
    }
    // CPU is now in Full-Emulation-State
  
    // read DeviceId from memory
    ReadMemQuick_430Xv2(DeviceIdPointer + 4, 1, (word*)&DeviceId);
    
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief Function to release the target device from JTAG control
//! \param[in] word Addr (0xFFFE: Perform Reset, means Load Reset Vector into 
//! PC, otherwise: Load Addr into PC)
void ReleaseDevice_430Xv2(unsigned long Addr)
{
    switch(Addr)
    {
        case V_BOR:
        
        // perform a BOR via JTAG - we loose control of the device then...
        IR_Shift(IR_TEST_REG);
        DR_Shift16(0x0200);
        MsDelay(5);     // wait some time before doing any other action
        // JTAG control is lost now - GetDevice() needs to be called again to gain control.
        break;
        
        case V_RESET:
        
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x0C01);                 // Perform a reset
        DR_Shift16(0x0401);
        IR_Shift(IR_CNTRL_SIG_RELEASE);
        break;
        
        default:
        
        SetPC_430Xv2(Addr);                 // Set target CPU's PC
        // prepare release & release
        SetTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x0401);
        IR_Shift(IR_ADDR_CAPTURE);
        IR_Shift(IR_CNTRL_SIG_RELEASE);
    }
}

//----------------------------------------------------------------------------
//! \brief This function writes one byte/word at a given address ( <0xA00)
//! \param[in] word Format (F_BYTE or F_WORD)
//! \param[in] word Addr (Address of data to be written)
//! \param[in] word Data (shifted data)
void WriteMem_430Xv2(word Format, unsigned long Addr, word Data)
{
    // Check Init State at the beginning
    IR_Shift(IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(0) & 0x0301)
    {
        ClrTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        if  (Format == F_WORD)
        {
            DR_Shift16(0x0500);
        }
        else
        {
            DR_Shift16(0x0510);
        }
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(Addr);
        
        SetTCLK();
        // New style: Only apply data during clock high phase
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(Data);           // Shift in 16 bits
        ClrTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x0501);
        SetTCLK();
        // one or more cycle, so CPU is driving correct MAB
        ClrTCLK();
        SetTCLK();
        // Processor is now again in Init State
    }
}

//----------------------------------------------------------------------------
//! \brief This function writes an array of words into the target memory.
//! \param[in] word StartAddr (Start address of target memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
void WriteMemQuick_430Xv2(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    unsigned long i;
  
    for (i = 0; i < Length; i++)
    {
        WriteMem_430Xv2(F_WORD, StartAddr, DataArray[i]);
        StartAddr += 2;
    }
}

//----------------------------------------------------------------------------
//! \brief This function programs/verifies an array of words into the FLASH
//! memory by using the FLASH controller.
//! \param[in] word StartAddr (Start address of FLASH memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
void WriteFLASH_430Xv2(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    word loadAddr  = RAM_START_ADDRESS;            // RAM start address specified in config header file
    word startAddr = loadAddr + FlashWrite_o[0];   // start address of the program in traget RAM

    FlashWrite_o[2] = (unsigned short)(StartAddr);     // set write start address
    FlashWrite_o[3] = (unsigned short)(StartAddr>>16);
    FlashWrite_o[4] = (unsigned short)(Length);        // set number of words to write
    FlashWrite_o[5] = (unsigned short)(Length>>16);
    FlashWrite_o[6] = SegmentInfoAKey5xx;               // FCTL3: lock/unlock INFO Segment A
                                                        // default = locked

    WriteMemQuick_430Xv2(loadAddr, FlashWrite_o_length/2, (word*)FlashWrite_o);
    ReleaseDevice_430Xv2(startAddr);

    {
        unsigned long Jmb = 0;
        unsigned long Timeout = 0;
    
        do
        {
            Jmb = i_ReadJmbOut();
            Timeout++;
        }
        while(Jmb != 0xABADBABE && Timeout < 3000);
        
        if(Timeout < 3000)
        {
            unsigned long i;
          
            for(i = 0; i < Length; i++)
            {
                i_WriteJmbIn(DataArray[i]);
                //usDelay(100);            // delay 100us  - added by GC       
            }
        }
    }
    {
        unsigned long Jmb = 0;
        unsigned long Timeout = 0;
        
        do
        {
            Jmb = i_ReadJmbOut();
            Timeout++;
        }
        while(Jmb != 0xCAFEBABE && Timeout < 3000);
    }

    SyncJtag_AssertPor();

    // clear RAM here - init with JMP $
    {
        word i;

        for (i = 0; i < FramWrite_o_length/2; i++)
        {
            WriteMem_430Xv2(F_WORD, loadAddr, 0x3fff);
            loadAddr += 2;
        }
    }
}

//----------------------------------------------------------------------------
//! \brief This function programs/verifies an array of words into the Fram
//! memory.
//! \param[in] word StartAddr (Start address of Fram memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
void WriteFram_430Xv2(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    word loadAddr  = RAM_START_ADDRESS;               // RAM start address specified in config header file
    word startAddr = loadAddr + FramWrite_o[0];       // start address of the program in target RAM

    FramWrite_o[2] = (unsigned short)(StartAddr);     // set write start address
    FramWrite_o[3] = (unsigned short)(StartAddr>>16);
    FramWrite_o[4] = (unsigned short)(Length);        // set number of words to write
    FramWrite_o[5] = (unsigned short)(Length>>16);

    WriteMemQuick_430Xv2(loadAddr, FramWrite_o_length/2, (word*)FramWrite_o);
    ReleaseDevice_430Xv2(startAddr);
    
    {
        unsigned long Jmb = 0;
        unsigned long Timeout = 0;
        
        do
        {
            Jmb = i_ReadJmbOut();
            Timeout++;
        }
        while(Jmb != 0xABADBABE && Timeout < 3000);
        
        if(Timeout < 3000)
        {
            unsigned long i;
          
            for(i = 0; i < Length; i++)
            {
                i_WriteJmbIn(DataArray[i]);
                //usDelay(100);            // delay 100us  - added by GC       
            }
        }
    }

    {
        unsigned long Jmb = 0;
        unsigned long Timeout = 0;
        
        do
        {
            Jmb = i_ReadJmbOut();
            Timeout++;
        }
        while(Jmb != 0xCAFEBABE && Timeout < 3000);
    }

    SyncJtag_AssertPor();

    // clear RAM here - init with JMP $
    {
        word i;

        for (i = 0; i < FlashWrite_o_length/2; i++)
        {
            WriteMem_430Xv2(F_WORD, loadAddr, 0x3fff);
            loadAddr += 2;
        }
    }
}

//----------------------------------------------------------------------------
//! \brief This function programs/verifies a set of data arrays of words into an FRAM
//! memory by using the "WriteMemQuick()" function. It conforms with the
//! "CodeArray" structure convention of file "Target_Code.s43" or "Target_Code.h".
//! \param[in] const unsigned int  *DataArray (Pointer to array with the data)
//! \param[in] const unsigned long *address (Pointer to array with the startaddresses)
//! \param[in] const unsigned long *length_of_sections (Pointer to array with the number of words counting from startaddress)
//! \param[in] const unsigned long sections (Number of sections in code file)
//! \return word (STATUS_SUCCESS if verification was successful,
//! STATUS_ERROR otherwise)
word WriteFRAMallSections_430Xv2(const unsigned int *data, const unsigned long *address, const unsigned long *length_of_sections, const unsigned long sections)
{
#ifdef MailBoxWriteFram
    int i, init = 1;
    for(i = 0; i < sections; i++)
    {  
        WriteFram_430Xv2(address[i], length_of_sections[i], (word*)&data[init-1]);        
        if (!VerifyMem(address[i], length_of_sections[i], (word*)&data[init-1]))
        {
            return(STATUS_ERROR);
        }
        init += length_of_sections[i];             
    }  
    return(STATUS_SUCCESS);

#else 
    int i, init = 1;
    for(i = 0; i < sections; i++)
    {  
        WriteMemQuick_430Xv2(address[i], length_of_sections[i], (word*)&data[init-1]);        
        if (!VerifyMem(address[i], length_of_sections[i], (word*)&data[init-1]))
        {
            return(STATUS_ERROR);
        }
        init += length_of_sections[i];             
    }  
    return(STATUS_SUCCESS);
#endif
}

//----------------------------------------------------------------------------
//! \brief This function programs/verifies a set of data arrays of words into a FLASH
//! memory by using the "WriteFLASH()" function. It conforms with the
//! "CodeArray" structure convention of file "Target_Code_5xx_(IDE).s43" or "Target_Code.h".
//! \param[in] const unsigned int  *DataArray (Pointer to array with the data)
//! \param[in] const unsigned long *address (Pointer to array with the startaddresses)
//! \param[in] const unsigned long *length_of_sections (Pointer to array with the number of words counting from startaddress)
//! \param[in] const unsigned long sections (Number of sections in code file)
//! \return word (STATUS_SUCCESS if verification was successful,
//! STATUS_ERROR otherwise)
word WriteFLASHallSections_430Xv2(const unsigned int *data, const unsigned long *address, const unsigned long *length_of_sections, const unsigned long sections)
{
    int i, init = 1;
  
    for(i = 0; i < sections; i++)
    {
        // Write/Verify(PSA) one FLASH section 
        WriteFLASH(address[i], length_of_sections[i], (word*)&data[init-1]);        
        if (!VerifyMem(address[i], length_of_sections[i], (word*)&data[init-1]))
        {
            return(STATUS_ERROR);
        }
        init += length_of_sections[i];      
    }    
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief This function reads one byte/word from a given address in memory
//! \param[in] word Format (F_BYTE or F_WORD)
//! \param[in] word Addr (address of memory)
//! \return word (content of the addressed memory location)
word ReadMem_430Xv2(word Format, unsigned long Addr)
{
    word TDOword = 0;
  
    // Check Init State at the beginning
    IR_Shift(IR_CNTRL_SIG_CAPTURE);
    if(DR_Shift16(0) & 0x0301)
    {
        // Read Memory
        ClrTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        if  (Format == F_WORD)
        {
            DR_Shift16(0x0501);             // Set word read
        }
        else
        {
            DR_Shift16(0x0511);             // Set byte read
        }
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(Addr);                   // Set address
        IR_Shift(IR_DATA_TO_ADDR);
        SetTCLK();
        ClrTCLK();
        TDOword = DR_Shift16(0x0000);       // Shift out 16 bits
        
        SetTCLK();
        // one or more cycle, so CPU is driving correct MAB
        ClrTCLK();
        SetTCLK();
        // Processor is now again in Init State
    }
    
    return TDOword;
}

//----------------------------------------------------------------------------
//! \brief This function reads an array of words from the memory.
//! \param[in] word StartAddr (Start address of memory to be read)
//! \param[in] word Length (Number of words to be read)
//! \param[out] word *DataArray (Pointer to array for the data)
void ReadMemQuick_430Xv2(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    unsigned long i;
  
    SetPC_430Xv2(StartAddr);
    SetTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x0501);
    IR_Shift(IR_ADDR_CAPTURE);
  
    IR_Shift(IR_DATA_QUICK);
  
    for (i = 0; i < Length; i++)
    {
        SetTCLK();
        ClrTCLK(); 
        *DataArray++   = DR_Shift16(0);  // Read data from memory.         
    }
}


//----------------------------------------------------------------------------
//! \brief This function performs an erase of a user defined FRAM memory section.
//! For FRAM devices the erase equals a write operation of 0xFFFF to the
//! respective memory section. (Could be extended with erase check via PSA)
//! \param[in] word StartAddr (start address for the erase)
//! \param[in] word Length (length of the memory section in WORDS)
void EraseFRAM_430Xv2(unsigned long EraseAddr, unsigned long Length)
{
    word loadAddr  = RAM_START_ADDRESS;           // RAM start address specified in config header file
    word startAddr = loadAddr + FramErase_o[0];   // start address of the program in target RAM

    FramErase_o[2] = (unsigned short)(EraseAddr);     // set dummy write address
    FramErase_o[3] = (unsigned short)(EraseAddr>>16);
    FramErase_o[4] = (unsigned short)(Length);      
    FramErase_o[5] = (unsigned short)(Length>>16);           
    FramErase_o[5] = SegmentInfoAKey5xx;              // FCTL3: lock/unlock INFO Segment A
                                                      // default = locked
    
    WriteMemQuick_430Xv2(loadAddr, FramErase_o_length/2, (word*)FramErase_o);
    ReleaseDevice_430Xv2(startAddr);

    {
        unsigned long Jmb = 0;
        unsigned long Timeout = 0;
        
        do
        {
            Jmb = i_ReadJmbOut();
            Timeout++;
        }
        while(Jmb != 0xCAFEBABE && Timeout < 3000);
    }

    SyncJtag_AssertPor();

    // clear RAM here - init with JMP $
    {
        word i;

        for (i = 0; i < FramErase_o_length/2; i++)
        {
            WriteMem_430Xv2(F_WORD, loadAddr, 0x3fff);
            loadAddr += 2;
        }
    }
}


//----------------------------------------------------------------------------
//! \brief This function performs a mass erase (with and w/o info memory) or a
//! segment erase of a FLASH module specified by the given mode and address.
//! Large memory devices get additional mass erase operations to meet the spec.
//! (Could be extended with erase check via PSA)
//! \param[in] word Mode (could be ERASE_MASS or ERASE_MAIN or ERASE_SGMT)
//! \param[in] word Addr (any address within the selected segment)
void EraseFLASH_430Xv2(word EraseMode, unsigned long EraseAddr)
{
    word loadAddr  = RAM_START_ADDRESS;            // RAM start address specified in config header file
    word startAddr = loadAddr + FlashErase_o[0];   // start address of the program in target RAM

    FlashErase_o[2] = (unsigned short)(EraseAddr);     // set dummy write address
    FlashErase_o[3] = (unsigned short)(EraseAddr>>16);
    FlashErase_o[4] = EraseMode;                       // set erase mode
    FlashErase_o[5] = SegmentInfoAKey5xx;               // FCTL3: lock/unlock INFO Segment A
                                                        // default = locked

    WriteMemQuick_430Xv2(loadAddr, FlashErase_o_length/2, (word*)FlashErase_o);
    ReleaseDevice_430Xv2(startAddr);

    {
        unsigned long Jmb = 0;
        unsigned long Timeout = 0;
        
        do
        {
            Jmb = i_ReadJmbOut();
            Timeout++;
        }
        while(Jmb != 0xCAFEBABE && Timeout < 3000);
    }

    SyncJtag_AssertPor();

    // clear RAM here - init with JMP $
    {
        word i;

        for (i = 0; i < FlashErase_o_length/2; i++)
        {
            WriteMem_430Xv2(F_WORD, loadAddr, 0x3fff);
            loadAddr += 2;
        }
    }
}

//----------------------------------------------------------------------------
//! \brief This function performs a mass erase (with and w/o info memory) or a
//! segment erase of a FLASH module specified by the given mode and address
//! without releasing the device from JTAG control.
//! Large memory devices get additional mass erase operations to meet the spec.
//! (Could be extended with erase check via PSA)
//! \param[in] word Mode (could be ERASE_MASS or ERASE_MAIN or ERASE_SGMT)
//! \param[in] word Addr (any address within the selected segment)
void EraseFLASH_430Xv2_wo_release(word EraseMode, unsigned long EraseAddr)
{
    word loadAddr  = RAM_START_ADDRESS;            // RAM start address specified in config header file
    word startAddr = loadAddr + FlashErase_o[0];   // start address of the program in target RAM

    FlashErase_o[2] = (unsigned short)(EraseAddr);     // set dummy write address
    FlashErase_o[3] = (unsigned short)(EraseAddr>>16);
    FlashErase_o[4] = EraseMode;                       // set erase mode
    FlashErase_o[5] = SegmentInfoAKey5xx;               // FCTL3: lock/unlock INFO Segment A
                                                        // default = locked

    WriteMemQuick_430Xv2(loadAddr, FlashErase_o_length/2, (word*)FlashErase_o);
  
    {
        word i;
        SetPC_430Xv2(startAddr);
        for (i=110; i--;)
        {
            ClrTCLK();
            SetTCLK();
        }
    }

    //max mass/segment erase time for F543x is 32ms
    //do not check mailbox, just wait..
    MsDelay(35);

    //let Erase function finish
    {
        word i;
        for (i=110; i--;) //110 to let the erase routine finish the restauration
        {
            ClrTCLK();
            SetTCLK();
        }
    }

    SyncJtag_AssertPor();

    // clear RAM here - init with JMP $
    {
        word i;

        for (i = 0; i < FlashErase_o_length/2; i++)
        {
            WriteMem_430Xv2(F_WORD, loadAddr, 0x3fff);
            loadAddr += 2;
        }
    }
}

//----------------------------------------------------------------------------
//! \brief This function performs an Erase Check over the given memory range
//! \param[in] word StartAddr (Start address of memory to be checked)
//! \param[in] word Length (Number of words to be checked)
//! \return word (STATUS_SUCCESS if erase check was successful, STATUS_ERROR 
//! otherwise)
word EraseCheck_430Xv2(unsigned long StartAddr, unsigned long Length)
{
    return (VerifyPSA_430Xv2(StartAddr, Length, 0));
}

//----------------------------------------------------------------------------
//! \brief This function performs a Verification over the given memory range
//! \param[in] word StartAddr (Start address of memory to be verified)
//! \param[in] word Length (Number of words to be verified)
//! \param[in] word *DataArray (Pointer to array with the data)
//! \return word (STATUS_SUCCESS if verification was successful, STATUS_ERROR
//! otherwise)
word VerifyMem_430Xv2(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    return (VerifyPSA_430Xv2(StartAddr, Length, DataArray));
}
//------------------------------------------------------------------------
//! \brief This function blows the security fuse.
//! \return word (TRUE if fuse blow was successful, FALSE otherwise)
word ProgramLockKey(void)
{
// #ifdef  FRAM_MEMORY

        // word LockKey[2] = { 0x5555, 0x5555 };
   
        // // write JTAG lock key to flash memory
        // WriteMemQuick_430Xv2(0xff80, 2, LockKey);
      
        // // now perform a BOR via JTAG - we loose control of the device then...
        // IR_Shift(IR_TEST_REG);
        // DR_Shift16(0x0200);
        // MsDelay(5);     // wait some time until Bootcode is executed
        
        // // -> get it under JTAG control again
        // // and return result of "is fuse blown?"
        // return(GetDevice() == STATUS_FUSEBLOWN);
    
// #else   // device has flash memory

        word LockKey[2] = { 0xDEAD, 0xBABE };

        // unprotect BSL memory by writing to the SYSBSLC register
        // to reset the BSL protection bit (SYSBSLPE)
        WriteMem(F_WORD, 0x0182, 0x0003);
    
        // write JTAG lock key to flash memory
        WriteFLASH(0x17FC, 2, LockKey);
      
        // now perform a BOR via JTAG - we loose control of the device then...
        IR_Shift(IR_TEST_REG);
        DR_Shift16(0x0200);
        MsDelay(5);     // wait some time until Bootcode is executed
        
        // -> get it under JTAG control again
        // and return result of "is fuse blown?"
        return(GetDevice() == STATUS_FUSEBLOWN);
// #endif
}

//------------------------------------------------------------------------
//! \brief This function checks if the JTAG lock key is programmed.
//! \return word (STATUS_SUCCESS if fuse is blown, STATUS_ERROR otherwise)
word IsLockKeyProgrammed(void)
{
    word i;

    for (i = 3; i > 0; i--)     //  First trial could be wrong
    {
        IR_Shift(IR_CNTRL_SIG_CAPTURE);
        if (DR_Shift16(0xAAAA) == 0x5555)
        {
            return(STATUS_SUCCESS);  // Fuse is blown
        }
    }
    return(STATUS_ERROR);       // Fuse is not blown
}

//------------------------------------------------------------------------
//! \brief This function disables the Memory Protection Unit (FR57xx devices only)
//! \return word (STATUS_SUCCESS if MPU was disabled successfully, STATUS_ERROR 
//! otherwise)
word DisableMpu430Xv2(void)
{
    word RetState = (STATUS_ERROR);  
    unsigned short MPUCTL0 =0x0000;
    unsigned short FramCtlKey = 0xA500;      
  
    // first read out the MPU control register 0 
    MPUCTL0 = ReadMem_430Xv2(F_WORD,0x05A0); 
  
    // check MPUENA bit: if MPU is not enabled just return no error
    if((MPUCTL0 & 0x1)==0)
    {
        return(STATUS_SUCCESS);
    }  
    // check MPULOCK bit: if MPULOCK is set write access to all MPU
    // registers is disabled until a POR/BOR occurs
    if((MPUCTL0 & 0x3)!=0x1)
    {    
        // feed in magic pattern to stop code execution after BOR
        if( IR_Shift(IR_JMB_EXCHANGE)  == JTAG_ID91)
        {
            if((DR_Shift16(0x01) & 0xFFF0) != 0x1200)
            {
                return(STATUS_ERROR);
            }
            MsDelay(15);
            if(DR_Shift16(0xA55A) != 0xFFFF)
            {
                return(STATUS_ERROR);
            }
        }
        else
        {
            return(STATUS_ERROR);
        }
        // Apply BOR to reset the device
        SetTST();
        MsDelay(20); 
        ClrTST();
    
        SetRST(); 
        MsDelay(20);      
        ClrRST();
        MsDelay(20); 
         
        // connect to device again, apply entry sequence
        ConnectJTAG(); 
         
        // Apply again 4wire/SBW entry Sequence.                        
            StartJtag(RSTHIGH_JTAG);

         // reset TAP state machine -> Run-Test/Idle
         ResetTAP(); 
         // get jtag control back
         if(STATUS_ERROR == SyncJtag_AssertPor())
         {
            return(STATUS_ERROR);
         }
    }
    
    // MPU Registers are unlocked. MPU can now be disabled.
    // Set MPUENA = 0, write Fram MPUCTL0 key 
    WriteMem_430Xv2(F_WORD, 0x05A0, FramCtlKey);
   
    MPUCTL0 = ReadMem_430Xv2(F_WORD,0x05A0);   
    // now check if MPU is disabled
    if((MPUCTL0 & 0x1)==0)
    {
        RetState = (STATUS_SUCCESS);  
    }   
    return RetState;
}

//------------------------------------------------------------------------
//! \brief This function unlocks the BSL memory protection.
void UnlockBsl_430Xv2Flash(void)
{
    unsigned short BslCur;

    // Read SYSBSLC register
    BslCur = ReadMem_430Xv2(F_WORD, 0x0182);
    // Disable BSL area protection
	BslCur &= ~SYSBSLPE;
	// Write back SYSBSLC register
    WriteMem_430Xv2(F_WORD, 0x0182, BslCur);
}

//------------------------------------------------------------------------
//! \brief This function unlocks segment A of the InfoMemory (Flash)
void UnlockInfoA_430Xv2(void)
{
    SegmentInfoAKey5xx = 0xA508;
}

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
