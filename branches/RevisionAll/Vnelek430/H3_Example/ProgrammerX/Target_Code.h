/*
 * 
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
*/

//! \file Target_Code.h
//! \brief Header file for target code (blinking LED)
//! \brief The actual target code can be found in
//! <ul> 
//! <li> Target_Code_CCS.s43 (for IAR Embedded Workbench)
//! <li> Target_Code_IAR.s43 (for Code Composer Studio)
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

// Du lieu da xao tron (swap)
// Chuong trinh nhay LED P1.0 su dung Timer & Interrupt

//@C000 (35 bytes)
unsigned int fdata1[]={
0x4031, 0x0400, 0x12B0, 0xC00C, 0x12B0, 0xC03C, 0x40B2, 0x5A80,
0x0120, 0x42D2, 0x10FF, 0x0057, 0x42D2, 0x10FE, 0x0056, 0xD3D2,
0x0022, 0x40B2, 0xAFFF, 0x0192, 0x40B2, 0x0292, 0x0180, 0xD232,
0x3FFF, 0xE3D2, 0x0021, 0xC392, 0x0180, 0x1300, 0x4030, 0xC040,
0x4030, 0xC044, 0x3FFF
};

//@FFF8
unsigned int fdata2[]={
	0xC032
};

//@FFFE
unsigned int fdata3[]={
	0xC000
};
