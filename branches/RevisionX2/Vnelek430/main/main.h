/* ==============================================================================
 *  File name:
 *      		main.h
 *	Description:
 * 		
 *
 *  Created on: 
 * 				October 10, 2013
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

// TimerA0, Clock = 24Mhz
#define Clr_INTF_TimerA0()		(TA0CTL &= ~TAIFG)		// Clear INTR flag
#define Pause_TimerA0()			(TA0CTL &= ~MC_3)		// Pause measure timer
#define Start_TimerA0()			(TA0CTL |= MC_2)		// Start measure timer
#define Stop_TimerA0()			(TA0CTL = 0x00)			// Stop measure timer

#define Get_TimerA0(dat)		(dat = TA0R)
#define Set_TimerA0(dat)		(TA0R = dat)
#define EnINT_TimerA0()			(TA0CTL &= ~TAIFG, TA0CTL |= TAIE)
#define DisINT_TimerA0()		(TA0CTL &= ~TAIE)

#define Init_TimerA0(dat)		(TA0CTL = TASSEL_2+ID_0+TACLR+MC_1, TA0CCR0 = dat)


const char ASCII_TAB[] = {'0', '1', '2', '3', '4', '5', '6', '7', 
			 				 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/*===============================================================================================
							Function definition
================================================================================================*/
void Init_Clock(void);
void __delay_20us(unsigned int time);
void __delay_ms(unsigned int time);
void usb_sendArray8(unsigned char *buffer, unsigned char length);
void usb_sendArray16(unsigned int *buffer, unsigned char length);

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
  Name         :  Init_Clock
  Description  :  Using 12Mhz external crystal to produce 12Mhz MCLK
  Argument(s)  :  
  Return value :  None.
================================================================================================*/
void Init_Clock (void)
{	

/* 12Mhz from XT2 */
#if MCLK_12MHZ 	
	P5SEL |= BIT2+BIT3;                       	// Port select XT2
	UCSCTL6 &= ~XT2OFF;                       	// Enable XT2 
	UCSCTL3 |= SELREF_2;                      	// FLLref = REFO
	UCSCTL4 |= SELA_2;                        	// ACLK=REFO,SMCLK=DCO,MCLK=DCO
	do
	{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
		SFRIFG1 &= ~OFIFG;                      	
	}
	while (SFRIFG1&OFIFG);                   	// Test oscillator fault flag

	UCSCTL6 &= ~XT2DRIVE0;                    	// Decrease XT2 Drive according to expected frequency
	UCSCTL4 |= SELS_5 + SELM_5;               	// SMCLK=MCLK=XT2
#endif

// /* 24MHz from XT2 */
// #ifdef MCLK_24MHZ
// 	P5SEL |= BIT2+BIT3;							// Port select XT2
// 	UCSCTL6 &= ~XT2OFF;							// Enable XT2
// 	__delay_cycles(20);

// 	//Start the "USB crystal"
// 	XT2_Start(XT2DRIVE_2);
// 	__delay_cycles(100);

// 	// Selects the ACLK source
// 	UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);
// 	__delay_cycles(100);

// 	// Selects the SMCLK source
// 	UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT2CLK);
// 	__delay_cycles(100);

// 	// Selects the MCLK source
// 	UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__XT2CLK);
// 	__delay_cycles(100);

// 	//Use the XT2 oscillator to source the FLL and ACLK
// 	UCSCTL3 = SELREF__XT2CLK;
// 	__delay_cycles(100);

// 	//MCLK will be driven by the FLL (not by XT2)
// 	Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768);
// 	__delay_cycles(100);

// 	// // Selects the SMCLK source
// 	// UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__DCOCLK);
// 	// __delay_cycles(100);

// 	// // Selects the MCLK source
// 	// UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__DCOCLK);
// 	// __delay_cycles(100);
// #endif

/* 24MHz from REFO */
#ifdef MCLK_24MHZ
	P5SEL |= BIT2+BIT3;							// Port select XT2
	UCSCTL6 &= ~XT2OFF;							// Enable XT2

	//Use the REFO oscillator to source the FLL and ACLK
	UCSCTL3 = SELREF__REFOCLK;
	UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);

	//MCLK will be driven by the FLL (not by XT2), referenced to the REFO
	Init_FLL_Settle(USB_MCLK_FREQ / 1000, USB_MCLK_FREQ / 32768);

	//Start the "USB crystal"
	XT2_Start(XT2DRIVE_2);
#endif
}

/*===============================================================================================
  Name         :  usb_sendArray8
  Description  :  Send array of data to computer using USB communication.
  Argument(s)  :  (uint8_t*) buffer
  Return value :  None.
================================================================================================*/
void usb_sendArray8(unsigned char *buffer, unsigned char length)
{
	unsigned int i = 0, k = 0;
	unsigned char usb_data[MAX_USB_RECEIVE*3];
	
	for(i = 0; i < length; i++)
	{
		usb_data[k+0] = ASCII_TAB[(buffer[i]>> 4)];
		usb_data[k+1] = ASCII_TAB[(buffer[i]&0xF)];
		usb_data[k+2] = ' ';
		k += 3;
	}
	
	cdcSendDataInBackground((BYTE*)usb_data, length*3, CDC0_INTFNUM, 20);
}

/*===============================================================================================
  Name         :  usb_sendArray16
  Description  :  Send array of data to computer using USB communication.
  Argument(s)  :  (uint16_t*) buffer
  Return value :  None.
================================================================================================*/
void usb_sendArray16(unsigned int *buffer, unsigned char length)
{
	unsigned int i = 0, k = 0;
	unsigned char tmp;
	unsigned char usb_data[MAX_USB_RECEIVE*3];
		
	for(i = 0; i < length; i++)
	{
		tmp = buffer[i]>>8;
		usb_data[k+0] = ASCII_TAB[(tmp>> 4)];
		usb_data[k+1] = ASCII_TAB[(tmp&0xF)];
		
		tmp = buffer[i];
		usb_data[k+2]   = ASCII_TAB[(tmp>> 4)];
		usb_data[k+3] = ASCII_TAB[(tmp&0xF)];
		
		usb_data[k+4] = ' ';
		k += 5;
	}
	
	cdcSendDataInBackground((BYTE*)usb_data, length*5, CDC0_INTFNUM, 20);
}

#endif /* MAIN_H_ */
