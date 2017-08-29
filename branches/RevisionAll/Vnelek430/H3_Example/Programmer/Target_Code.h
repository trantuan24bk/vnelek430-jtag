//! \file Target_Code.h
//! \brief Header file for target code (blinking LED)
//! 
//! \version 1.0

//! \brief Holds the entire target code
unsigned int eprom[10];

//! \brief Holds the destination adresses for different sections of the target code
unsigned long eprom_address[10];             // index 10 only for debugging

//! \brief Holds the length of the different memory sections
unsigned long eprom_length_of_sections[10];

//! \brief Holds the number of memory sections
unsigned long eprom_sections;


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


//===============================================================================================

// // Du lieu chua xao tron.
// Chuong trinh nhay LED P1.0 su dung Timer & Interrupt
// //@C000
// unsigned int fdata1[]={
// 0x31, 0x40, 0x00, 0x04, 0xB0, 0x12, 0x0C, 0xC0, 0xB0, 0x12, 0x3C, 0xC0, 0xB2, 0x40, 0x80, 0x5A,
// 0x20, 0x01, 0xD2, 0x42, 0xFF, 0x10, 0x57, 0x00, 0xD2, 0x42, 0xFE, 0x10, 0x56, 0x00, 0xD2, 0xD3,
// 0x22, 0x00, 0xB2, 0x40, 0xFF, 0xAF, 0x92, 0x01, 0xB2, 0x40, 0x92, 0x02, 0x80, 0x01, 0x32, 0xD2,
// 0xFF, 0x3F, 0xD2, 0xE3, 0x21, 0x00, 0x92, 0xC3, 0x80, 0x01, 0x00, 0x13, 0x30, 0x40, 0x40, 0xC0,
// 0x30, 0x40, 0x44, 0xC0, 0xFF, 0x3F
// };

// //@FFF8
// unsigned int fdata2[]={
	// 0x32, 0xC0
// };

// //@FFFE
// unsigned int fdata3[]={
	// 0x00, 0xC0
// }
