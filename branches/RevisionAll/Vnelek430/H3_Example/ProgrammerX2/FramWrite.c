/*
 * FramWrite.c
 *
 * Version 1.0
 * 
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/* http://srecord.sourceforge.net/ */

//! \file FramWrite.c
//! \brief Funclet for FRAM write operations
//! \details This binary code is placed in the target device's RAM to
//! speed up FRAM write operations. The actual source code has to be
//! compiled separately and converted using SRecord. The procedure is
//! described in the slau320.pdf documentation.
//! \author Berenbrinker Florian
//! \version 1.0

//! \brief Holds the target code for an FRAM write operation
//! \details This code is modified by the FRAM write function depending on it's parameters.
unsigned short FramWrite_o[] =
{
0x0014, 0x0076, 0xBEEF, 0xDEAD, 0xBEEF, 0xDEAD, 0xDEAD, 0x000B, 0xDEAD,
0x000B, 0x40B2, 0x5A80, 0x015C, 0x40B2, 0xABAD, 0x018E, 0x40B2, 0xBABE,
0x018C, 0x180F, 0x4AC0, 0xFFE2, 0x180F, 0x4BC0, 0xFFE0, 0x1F80, 0x405A,
0xFFCE, 0x1F80, 0x405B, 0xFFCC, 0x40B2, 0x0050, 0x0186, 0xB392, 0x0186,
0x27FD, 0x429A, 0x0188, 0x0000, 0xC392, 0x0186, 0x1800, 0x536A, 0x1800,
0x835B, 0x23F3, 0x1F80, 0x405A, 0xFFAA, 0x1F80, 0x405B, 0xFFA8, 0x40B2,
0xCAFE, 0x018E, 0x40B2, 0xBABE, 0x018C, 0x3FFF,
};
unsigned long FramWrite_o_termination = 0x00000000;
unsigned long FramWrite_o_start       = 0x00005C00;
unsigned long FramWrite_o_finish      = 0x00005C78;
unsigned long FramWrite_o_length      = 0x00000078;

#define FRAMWRITE_O_TERMINATION 0x00000000
#define FRAMWRITE_O_START       0x00005C00
#define FRAMWRITE_O_FINISH      0x00005C78
#define FRAMWRITE_O_LENGTH      0x00000078
