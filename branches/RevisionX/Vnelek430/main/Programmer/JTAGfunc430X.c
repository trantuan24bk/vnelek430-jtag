/*==========================================================================*\
|                                                                            |
| JTAGfunc430X.c                                                             |
|                                                                            |
| JTAG Control Sequences for Erasing / Programming / Fuse Burning            |
|----------------------------------------------------------------------------|
| Project:              JTAG Functions                                       |
| Developed using:      IAR Embedded Workbench 3.40A [Kickstart]             |
|             and:      Code Composer Eessentials 2.0                        |
|----------------------------------------------------------------------------|
| Author:               WLUT                                                 |
| Version:              2.1                                                  |
| Initial Version:      04-17-02                                             |
| Last Change:          03-14-13                                             |
|----------------------------------------------------------------------------|
| Version history:                                                           |
| 1.0 04/02 FRGR        Initial version.                                     |
| 1.1 04/02 ALB2        Formatting changes, added comments.                  |
| 1.2 08/02 ALB2        Initial code release with Lit# SLAA149.              |
| 1.3 09/05 JDI         'ResetTAP': added SetTDI for fuse check              |
|                       search for F2xxx to find respective modifications in |
|                       'SetPC', 'HaltCPU', 'VerifyPSA', 'EraseFLASH'        |
|                       'WriteFLASH'                                         |
|           SUN1        Software delays redesigned to use TimerA harware;    |
|                       see MsDelay() routine.                               |
| 1.4 01/06 STO         Added entry sequence for SBW devices                 |
|                       Minor cosmetic changes                               |
| 1.5 03/06 STO         BlowFuse() make correct fuse check after blowing.    |
| 1.6 07/06 STO         Loop in WriteFLASH() changed.                        |
| 1.7 04/06 WLUT        'VerifyPSA','ReadMemQuick changed to TCLK high.      |
|                       WriteFLASHallSections changed due to spec of srec_cat|
|                       Renamed 'ExecutePUC' to 'ExecutePOR'                 |
| 1.8 01/08 WLUT        Added usDelay(5) in ResetTAP() to ensure at least    |
|                       5us low phase of TMS during JTAG fuse check.         |
| 1.9 08/08 WLUT        Removed references to Shift() and Shift_430X()       |
|                       functions and replaced with AllShifts() reference.   |
| 2.0 06/12 RL          Updated commentaries                                 |
| 2.1 03/13 RL/MD       Added unlock function for Info A                     |
|----------------------------------------------------------------------------|
| Designed 2002 by Texas Instruments Germany                                 |
\*==========================================================================*/


//! \file JTAGfunc430X.c
//! \brief JTAG Control Sequences for Erasing / Programming / Fuse Burning
//! \author Robert Lessmeier
//! \date 03/14/2013
//! \version 2.1

/****************************************************************************/
/* INCLUDES                                                                 */
/****************************************************************************/

#include "JTAGfunc430X.h"
#include "LowLevelFunc430X.h"
#include "Devices430X.h"

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
    return((word)AllShifts(F_WORD, data));
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
    SetTMS();
    // JTAG FSM is now in Run-Test/IDLE

    // Perform fuse check
    ClrTMS();
    usDelay(5); // at least 5us low required
    SetTMS();
    ClrTMS();
    usDelay(5); // at least 5us low required
    SetTMS();
}

//----------------------------------------------------------------------------
//! \brief Function to execute a Power-On Reset (POR) using JTAG CNTRL SIG 
//! register
//! \return word (STATUS_SUCCESS if target is in Full-Emulation-State afterwards,
//! STATUS_ERROR otherwise)
static word ExecutePOR_430X(void)
{
    word JtagVersion;

    // Perform Reset
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2C01);                      // Apply Reset
    DR_Shift16(0x2401);                      // Remove Reset
    ClrTCLK();
    SetTCLK();
    ClrTCLK();
    SetTCLK();
    ClrTCLK();
    JtagVersion = IR_Shift(IR_ADDR_CAPTURE); // read JTAG ID, checked at function end
    SetTCLK();

    WriteMem_430X(F_WORD, 0x0120, 0x5A80);   // Disable Watchdog on target device

    if (JtagVersion != JTAG_ID)
    {
        return(STATUS_ERROR);
    }
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief Function to set target CPU JTAG FSM into the instruction fetch state
//! \return word (STATUS_SUCCESS if instr. fetch was set, STATUS_ERROR otherwise)
static word SetInstrFetch(void)
{
    word i;

    IR_Shift(IR_CNTRL_SIG_CAPTURE);

    // Wait until CPU is in instr. fetch state, timeout after limited attempts
    for (i = 50; i > 0; i--)
    {
        if (DR_Shift16(0x0000) & 0x0080)
        {
            return(STATUS_SUCCESS);
        }
        ClrTCLK();
        SetTCLK();
    }
    return(STATUS_ERROR);
}

//----------------------------------------------------------------------------
//! \brief Load a given address into the target CPU's program counter (PC).
//! \param[in] word Addr (destination address)
static void SetPC_430X(unsigned long Addr)
{
    SetInstrFetch();              // Set CPU into instruction fetch mode, TCLK=1

    // Load PC with address
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x3401);           // CPU has control of RW & BYTE.
    IR_Shift(IR_DATA_16BIT);
    DR_Shift16((word)(0x0080 | (((Addr)>>8) & 0x0F00))); // "mova #addr20,PC" instruction

    ClrTCLK();
    SetTCLK();
    DR_Shift16(Addr);             // second word of "mova #addr20,PC" instruction
    ClrTCLK();
    IR_Shift(IR_ADDR_CAPTURE);
    SetTCLK();
    ClrTCLK();                    // Now the PC should be on Addr
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2401);           // JTAG has control of RW & BYTE.
}

//----------------------------------------------------------------------------
//! \brief Function to set the CPU into a controlled stop state
static void HaltCPU(void)
{
    SetInstrFetch();              // Set CPU into instruction fetch mode

    IR_Shift(IR_DATA_16BIT);
    DR_Shift16(0x3FFF);           // Send JMP $ instruction
    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2409);           // Set JTAG_HALT bit
    SetTCLK();
}

//----------------------------------------------------------------------------
//! \brief Function to release the target CPU from the controlled stop state
static void ReleaseCPU(void)
{
    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2401);           // Clear the HALT_JTAG bit
    IR_Shift(IR_ADDR_CAPTURE);
    SetTCLK();
}


//----------------------------------------------------------------------------
//! \brief This function compares the computed PSA (Pseudo Signature Analysis)
//! value to the PSA value shifted out from the target device.
//! It is used for very fast data block write or erasure verification.
//! \param[in] unsigned long StartAddr (Start address of data block to be checked)
//! \param[in] unsigned long Length (Number of words within data block)
//! \param[in] word *DataArray (Pointer to array with the data, 0 for Erase Check)
//! \return word (STATUS_SUCCESS if comparison was successful, STATUS_ERROR otherwise)
word VerifyPSA_430X(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    word TDOword;
    unsigned int i;
    const word POLY = 0x0805;             // Polynom value for PSA calculation
    word PSA_CRC = (word)(StartAddr-2);   // Start value for PSA calculation

    ExecutePOR_430X();

    if(DeviceHas_EnhVerify())
    {
        SetPC_430X(StartAddr-4);
        HaltCPU();
        ClrTCLK();
        IR_Shift(IR_DATA_16BIT);
        DR_Shift20(StartAddr-2);
    }
    else
    {
        SetPC_430X(StartAddr-2);
        SetTCLK();
        ClrTCLK();
    }

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

        // Clock through the PSA
        SetTCLK();

        //ClrTCLK();          // set here -> Fixes problem with F123 PSA in RAM

        ClrTCK(); 
        MsDelay(1); 
        SetTMS();

        SetTCK();           // Select DR scan
        ClrTCK();
        ClrTMS();

        SetTCK();            // Capture DR
        ClrTCK(); 

        SetTCK();           // Shift DR
        ClrTCK();
        SetTMS();
		
        SetTCK();           // Exit DR
        ClrTCK();
        SetTCK();
        ClrTMS();
        ClrTCK();
        SetTCK();

        ClrTCLK();           // set here -> future purpose
        MsDelay(10); 
    }
    IR_Shift(IR_SHIFT_OUT_PSA);
    TDOword = DR_Shift16(0x0000);     // Read out the PSA value
    SetTCLK();

    if(DeviceHas_EnhVerify())
    {
        ReleaseCPU();
    }
    ExecutePOR_430X();

    return((TDOword == PSA_CRC) ? STATUS_SUCCESS : STATUS_ERROR);

}

/****************************************************************************/
/* High level routines for accessing the target device via JTAG:            */
/*                                                                          */
/* From the following, the user is relieved from coding anything.           */
/* To provide better understanding and clearness, some functionality is     */
/* coded generously. (Code and speed optimization enhancements may          */
/* be desired)                                                              */
/****************************************************************************/

//----------------------------------------------------------------------------
//! \brief Function to start the JTAG communication
static void StartJtag (void)
{
    // drive JTAG/TEST signals
    {
      DrvSignals();
      MsDelay(10);             // delay 10ms
    }

    ResetTAP();  // reset TAP state machine -> Run-Test/Idle
    IR_Shift(IR_BYPASS);
    JtagId = (byte)IR_Shift(IR_BYPASS);
}

//----------------------------------------------------------------------------
//! \brief Function to stop the JTAG communication
static void StopJtag (void)
{
    // release JTAG/TEST signals
    {
      RlsSignals();
      MsDelay(10);             // delay 10ms
    }
}

//----------------------------------------------------------------------------
//! \brief Function to take target device under JTAG control. Disables the 
//! target watchdog. Sets the global DEVICE variable as read from the target 
//! device.
//! \return word (STATUS_ERROR if fuse is blown, incorrect JTAG ID or
//! synchronizing time-out; STATUS_SUCCESS otherwise)
word GetDevice_430X(void)
{
    word i;

    for (i = 0; i < MAX_ENTRY_TRY; i++)
    {
        JtagId = 0;    // Initialize JtagId with an invalid value
        StopJtag();    // Release JTAG/TEST signals to savely reset the test logic
        StartJtag();   // Establish the physical connection to the JTAG interface
        if(JtagId == JTAG_ID) // Break if a valid JTAG ID is being returned
        break;
    }
    if(i >= MAX_ENTRY_TRY)
    {
        return(STATUS_ERROR);
    }

    ResetTAP();                          // Reset JTAG state machine, check fuse HW

    if (IsFuseBlown())                   // Stop here if fuse is already blown
    {
        return(STATUS_FUSEBLOWN);
    }
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2401);                  // Set device into JTAG mode + read
    if (IR_Shift(IR_CNTRL_SIG_CAPTURE) != JTAG_ID)
    {
        return(STATUS_ERROR);
    }

    // Wait until CPU is synchronized, timeout after a limited # of attempts
    for (i = 50; i > 0; i--)
    {
        if (DR_Shift16(0x0000) & 0x0200)
        {
            word DeviceId;
            DeviceId = ReadMem(F_WORD, 0x0FF0);// Get target device type
                                               //(bytes are interchanged)
            DeviceId = (DeviceId << 8) + (DeviceId >> 8); // swop bytes
            //Set Device index, which is used by functions in Device.c
            SetDevice(DeviceId);
            break;
        }
        else
        {
            if (i == 1)
            {
                return(STATUS_ERROR);      // Timeout reached, return false
            }
        }
    }
    if (!ExecutePOR_430X())                // Perform PUC, Includes
    {
        return(STATUS_ERROR);              // target Watchdog disable.
    }
    return(STATUS_SUCCESS);
}

//----------------------------------------------------------------------------
//! \brief Function to release the target device from JTAG control
//! \param[in] word Addr (0xFFFE: Perform Reset, means Load Reset Vector into 
//! PC, otherwise: Load Addr into PC)
void ReleaseDevice_430X(unsigned long Addr)
{
    if (Addr == V_RESET)
    {
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x2C01);                 // Perform a reset
        DR_Shift16(0x2401);
    }
    else
    {
        SetPC_430X(Addr);                   // Set target CPU's PC
    }
    IR_Shift(IR_CNTRL_SIG_RELEASE);
}

//----------------------------------------------------------------------------
//! \brief This function writes one byte/word at a given address ( <0xA00)
//! \param[in] word Format (F_BYTE or F_WORD)
//! \param[in] word Addr (Address of data to be written)
//! \param[in] word Data (shifted data)
void WriteMem_430X(word Format, unsigned long Addr, word Data)
{
    HaltCPU();

    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    if  (Format == F_WORD)
    {
        DR_Shift16(0x2408);     // Set word write
    }
    else
    {
        DR_Shift16(0x2418);     // Set byte write
    }
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(Addr);           // Set addr
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(Data);           // Shift in 16 bits
    SetTCLK();

    ReleaseCPU();
}

//----------------------------------------------------------------------------
//! \brief This function writes an array of words into the target memory.
//! \param[in] word StartAddr (Start address of target memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
void WriteMemQuick_430X(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    unsigned long i;

    // Initialize writing:
    SetPC_430X(StartAddr-4);
    HaltCPU();

    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2408);             // Set RW to write
    IR_Shift(IR_DATA_QUICK);
    for (i = 0; i < Length; i++)
    {
        DR_Shift16(DataArray[i]);   // Shift in the write data
        SetTCLK();
        ClrTCLK();                  // Increment PC by 2
    }
    ReleaseCPU();
}

//----------------------------------------------------------------------------
//! \brief This function programs/verifies an array of words into the FLASH
//! memory by using the FLASH controller.
//! \param[in] word StartAddr (Start address of FLASH memory)
//! \param[in] word Length (Number of words to be programmed)
//! \param[in] word *DataArray (Pointer to array with the data)
void WriteFLASH_430X(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    word i;                            // Loop counter
    unsigned long addr = StartAddr;    // Address counter
    word FCTL3_val = SegmentInfoAKey;  // Lock/Unlock SegA InfoMem Seg.A, def=locked

    HaltCPU();

    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2408);         // Set RW to write
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(0x0128);         // FCTL1 register
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(0xA540);         // Enable FLASH write
    SetTCLK();

    ClrTCLK();
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(0x012A);         // FCTL2 register
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(0xA540);         // Select MCLK as source, DIV=1
    SetTCLK();

    ClrTCLK();
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(0x012C);         // FCTL3 register
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(FCTL3_val);      // Clear FCTL3; F2xxx: Unlock Info-Seg.
                                // A by toggling LOCKA-Bit if required,
    SetTCLK();

    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);

    for (i = 0; i < Length; i++, addr += 2)
    {
        DR_Shift16(0x2408);             // Set RW to write
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(addr);               // Set address
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(DataArray[i]);       // Set data
        SetTCLK();
        ClrTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x2409);             // Set RW to read

        TCLKstrobes(35);        // Provide TCLKs, min. 33 for F149 and F449
                                // F2xxx: 29 are ok
    }

    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2408);         // Set RW to write
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(0x0128);         // FCTL1 register
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(0xA500);         // Disable FLASH write
    SetTCLK();

    // set LOCK-Bits again
    ClrTCLK();
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(0x012C);         // FCTL3 address
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(FCTL3_val);      // Lock Inf-Seg. A by toggling LOCKA and set LOCK again
    SetTCLK();

    ReleaseCPU();
}

//----------------------------------------------------------------------------
//! \brief This function programs/verifies a set of data arrays of words into a FLASH
//! memory by using the "WriteFLASH()" function. It conforms with the
//! "CodeArray" structure convention of file "Target_Code_(IDE).s43" or "Target_Code.h".
//! \param[in] const unsigned int  *DataArray (Pointer to array with the data)
//! \param[in] const unsigned long *address (Pointer to array with the startaddresses)
//! \param[in] const unsigned long *length_of_sections (Pointer to array with the number of words counting from startaddress)
//! \param[in] const unsigned long sections (Number of sections in code file)
//! \return word (STATUS_SUCCESS if verification was successful,
//! STATUS_ERROR otherwise)
word WriteFLASHallSections_430X(const unsigned int *data, const unsigned long *address, const unsigned long *length_of_sections, const unsigned long sections)
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
word ReadMem_430X(word Format, unsigned long Addr)
{
    word TDOword;

    HaltCPU();

    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    if  (Format == F_WORD)
    {
        DR_Shift16(0x2409);             // Set word read
    }
    else
    {
        DR_Shift16(0x2419);             // Set byte read
    }
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(Addr);                   // Set address
    IR_Shift(IR_DATA_TO_ADDR);    
    SetTCLK();
    ClrTCLK();    
    TDOword = DR_Shift16(0x0000);       // Shift out 16 bits
    ReleaseCPU();
    return(Format == F_WORD ? TDOword : TDOword & 0x00FF);
}

//----------------------------------------------------------------------------
//! \brief This function reads an array of words from the memory.
//! \param[in] word StartAddr (Start address of memory to be read)
//! \param[in] word Length (Number of words to be read)
//! \param[out] word *DataArray (Pointer to array for the data)
void ReadMemQuick_430X(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    unsigned long i;

    // Initialize reading:
    SetPC_430X(StartAddr-4);
    HaltCPU();

    ClrTCLK();
    IR_Shift(IR_CNTRL_SIG_16BIT);
    DR_Shift16(0x2409);                    // Set RW to read
    IR_Shift(IR_DATA_QUICK);

    for (i = 0; i < Length; i++)
    {
        SetTCLK();
        DataArray[i] = DR_Shift16(0x0000); // Shift out the data
                                          // from the target.
        ClrTCLK();
    }
    ReleaseCPU();
}

//----------------------------------------------------------------------------
//! \brief This function performs a mass erase (with and w/o info memory) or a
//! segment erase of a FLASH module specified by the given mode and address.
//! Large memory devices get additional mass erase operations to meet the spec.
//! (Could be extended with erase check via PSA)
//! \param[in] word Mode (could be ERASE_MASS or ERASE_MAIN or ERASE_SGMT)
//! \param[in] word Addr (any address within the selected segment)
void EraseFLASH_430X(word EraseMode, unsigned long EraseAddr)
{
    word StrobeAmount = 4820;           // default for Segment Erase
    volatile word i, loopcount = 1;     // erase cycle repeating for Mass Erase
    word FCTL3_val = SegmentInfoAKey;   // Lock/Unlock SegA InfoMem Seg.A, def=locked
    
    if ((EraseMode == ERASE_MASS) ||
        (EraseMode == ERASE_MAIN) ||
        (EraseMode == ERASE_ALLMAIN) ||
        (EraseMode == ERASE_GLOB)
       )
    {
        if(DeviceHas_FastFlash())
        {
            StrobeAmount = 10600;   // Larger Flash memories require
        }
        else
        {
            StrobeAmount = 5300;    // Larger Flash memories require
            loopcount = 19;         // additional cycles for erase.
        }
    }
    HaltCPU();

    for (i = loopcount; i > 0; i--)
    {
        ClrTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x2408);         // set RW to write
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(0x0128);         // FCTL1 address
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(EraseMode);      // Enable erase mode
        SetTCLK();

        ClrTCLK();
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(0x012A);         // FCTL2 address
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(0xA540);         // MCLK is source, DIV=1
        SetTCLK();

        ClrTCLK();
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(0x012C);         // FCTL3 address
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(FCTL3_val);      // Clear FCTL3; F2xxx: Unlock Info-Seg. A by toggling LOCKA-Bit if required,
        SetTCLK();

        ClrTCLK();
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(EraseAddr);      // Set erase address
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(0x55AA);         // Dummy write to start erase
        SetTCLK();

        ClrTCLK();
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x2409);         // Set RW to read
        TCLKstrobes(StrobeAmount);  // Provide TCLKs
        IR_Shift(IR_CNTRL_SIG_16BIT);
        DR_Shift16(0x2408);         // Set RW to write
        IR_Shift(IR_ADDR_16BIT);
        DR_Shift20(0x0128);         // FCTL1 address
        IR_Shift(IR_DATA_TO_ADDR);
        DR_Shift16(0xA500);         // Disable erase
        SetTCLK();

    }
    // set LOCK-Bits again
    ClrTCLK();
    IR_Shift(IR_ADDR_16BIT);
    DR_Shift20(0x012C);             // FCTL3 address
    IR_Shift(IR_DATA_TO_ADDR);
    DR_Shift16(FCTL3_val);          // Lock Inf-Seg. A by toggling LOCKA (F2xxx) and set LOCK again
    SetTCLK();

    ReleaseCPU();
}

//----------------------------------------------------------------------------
//! \brief This function performs an Erase Check over the given memory range
//! \param[in] word StartAddr (Start address of memory to be checked)
//! \param[in] word Length (Number of words to be checked)
//! \return word (STATUS_SUCCESS if erase check was successful, STATUS_ERROR 
//! otherwise)
word EraseCheck_430X(unsigned long StartAddr, unsigned long Length)
{
    return (VerifyPSA_430X(StartAddr, Length, 0));
}

//----------------------------------------------------------------------------
//! \brief This function performs a Verification over the given memory range
//! \param[in] word StartAddr (Start address of memory to be verified)
//! \param[in] word Length (Number of words to be verified)
//! \param[in] word *DataArray (Pointer to array with the data)
//! \return word (STATUS_SUCCESS if verification was successful, STATUS_ERROR
//! otherwise)
word VerifyMem_430X(unsigned long StartAddr, unsigned long Length, word *DataArray)
{
    return (VerifyPSA_430X(StartAddr, Length, DataArray));
}


//------------------------------------------------------------------------
//! \brief This function checks if the JTAG access security fuse is blown.
//! \return word (STATUS_SUCCESS if fuse is blown, STATUS_ERROR otherwise)
word IsFuseBlown(void)
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
    return(STATUS_ERROR);       // fuse is not blown
}

//------------------------------------------------------------------------
//! \brief This function unlocks segment A of the InfoMemory (Flash)
void UnlockInfoA(void)
{
  SegmentInfoAKey = 0xA540;
}

/****************************************************************************/
/*                         END OF SOURCE FILE                               */
/****************************************************************************/
