/*
 * 
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
*/

//! \file Target_Code.h
//! \brief Header file for target code (blinking LED)
//! \brief The actual target code can be found in
//! <ul> 
//! <li> Target_Code_5xx_CCS.s43 (for IAR Embedded Workbench)
//! <li> Target_Code_5xx_IAR.s43 (for Code Composer Studio)
//! </ul>
//! \version 1.0

//! \brief Holds the entire target code
extern const unsigned int eprom[10];
//! \brief Holds the destination adresses for different sections of the target code
extern const unsigned long eprom_address[10];             // index 10 only for debugging
//! \brief Holds the length of the different memory sections
extern const unsigned long eprom_length_of_sections[10];
//! \brief Holds the number of memory sections
extern const unsigned long eprom_sections;


//! Cac buoc de chuan bi nap chuong trinh cho target chip
//!
//! 1. Bien dich chuong trinh voi IAR, CCS (*.txt)
//! 2. Hoan doi 2-byte ke nhau: 
//!    01 02 03 04 -> 0201 0403
//! 3. Nap doan ma vao dia chi da quy dinh tai "@"

//===============================================================================================

// MSP430F5510
// Du lieu da xao tron (swap)
// Chuong trinh nhay LED P1.0 su dung Timer & Interrupt

//@8000 (68 bytes/ 34 words)
unsigned int fdata1[]={
0x4031 ,0x3400 ,0x13B0 ,0x8018 ,0x13B0 ,0x8038 ,0xE0F2 ,0x0003,
0x0202 ,0xC392 ,0x0380 ,0x1300 ,0x40B2 ,0x5A80 ,0x015C ,0xD3D2,
0x0204 ,0xD3E2 ,0x0204 ,0x40B2 ,0xAFFF ,0x0392 ,0x40B2 ,0x0292,
0x0380 ,0xD232 ,0x3FFF ,0x4303 ,0x0080 ,0x8040 ,0x3FFF ,0x4303,
0x0080 ,0x803C
};

//@FFE0 (2 bytes/ 1 word)
unsigned int fdata2[]={
	0x800C
};

//@FFFE (2 bytes/ 1 word)
unsigned int fdata3[]={
	0x8000
};
