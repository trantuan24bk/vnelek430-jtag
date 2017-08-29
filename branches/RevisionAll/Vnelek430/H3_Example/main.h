/* ==============================================================================
 *  File name:
 *      		main.h
 *	Description:
 * 		
 *
 *  Created on: 
 * 				February 25, 2014
 *  Author: 
 * 				TRAN TUAN
 *	Address:
 * 		Vnelektronics Co.Ltd
 * 		776 Ton Duc Thang, Lien Chieu, Danang
 *		website: www.vnelek.vn
 *
 * 	Complier: 
 * 			IAR Embedded Workbench v5.3
 * ==============================================================================*/

#ifndef MAIN_H_
#define MAIN_H_

/*===============================================================================================
							Global variables definition
================================================================================================*/


const char ASCII_TAB[] = {'0', '1', '2', '3', '4', '5', '6', '7', 
			 				 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/*===============================================================================================
							Function definition
================================================================================================*/
void Init_Clock(void);
void Init_Ports (void);
void __delay_20us(unsigned int time);
void __delay_ms(unsigned int time);

/*===============================================================================================
  Name         :  __delay_us
  Description  :  with freq(MCLK) = 12Mhz
  Argument(s)  :  None.
  Return value :  None.
================================================================================================*/
void __delay_20us(unsigned int time)
{
	while(time--)
	__delay_cycles(87);
}

/*===============================================================================================
  Name         :  __delay_ms
  Description  :  with freq(MCLK) = 12Mhz
  Argument(s)  :  None.
  Return value :  None.
================================================================================================*/
void __delay_ms(unsigned int time)
{
	while(time--)
		__delay_20us(64);
}

/*===============================================================================================
  Name         :  Init_Ports
  Description  :  Initialization of ports (all unused pins as outputs with low-level
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
void Init_Ports (void)
{
    P1OUT = 0x00;
    P1DIR = 0xFF;
	P2OUT = 0x00;
    P2DIR = 0xFF;
    P3OUT = 0x00;
    P3DIR = 0xFF;
    P4OUT = 0x00;
    P4DIR = 0xFF;
    P5OUT = 0x00;
    P5DIR = 0xFF;
    P6OUT = 0x00;
    P6DIR = 0xFF;
}


/*===============================================================================================
  Name         :  Init_Clock
  Description  :  Using 12Mhz external crystal to produce 24Mhz MCLK
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
void Init_Clock (void)
{	
	P5SEL |= 0x0C;                                      //enable XT2 pins
	
	//use REFO for FLL and ACLK
	UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);
	UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

	//MCLK will be driven by the FLL (not by XT2), referenced to the REFO
	Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768);   //Start the FLL, at the freq indicated by the config
																	//constant USB_MCLK_FREQ
	XT2_Start(XT2DRIVE_0);                                          //Start the "USB crystal"
}

#endif /* MAIN_H_ */
