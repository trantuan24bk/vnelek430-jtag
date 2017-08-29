#include <windows.h>

// Define message color
#define COLOR_RED				0x01
#define COLOR_GREEN				0x02
#define COLOR_BLACK				0x03
#define COLOR_BROWN				0x04

// Define command
#define CMD_READ				0x11
#define CMD_READ_END			0x12
#define CMD_WRITE				0x13
#define CMD_WRITE_FIRST			0x1A
#define CMD_WRITE_END			0x14
#define CMD_VERIFY				0x15
#define CMD_VERIFY_END			0x16
#define CMD_DETECT				0x17
#define CMD_ERASE				0x18
#define CMD_HWTEST				0x19
#define CMD_POWERON				0x1A

#define STATUS_SUCCESS			0x01				// Returned status code
#define STATUS_ERROR			0x00

#define STATUS_CRC_FAILED		0x20
#define STATUS_CRC_SUCCESS		0x21

#define STATUS_WRITE_FAILED		0x22
#define STATUS_WRITE_SUCCESS	0x23

#define STATUS_VERIFY_FAILED	0x24
#define STATUS_VERIFY_SUCCESS	0x25

#define STATUS_POWERON_FAILED	0x26
#define STATUS_POWERON_SUCCESS  0x27

#define STATUS_ERASE_FAILED		0x28
#define STATUS_ERASE_SUCCESS	0x29

#define STATUS_HWTEST_FAILED	0x2A
#define STATUS_HWTEST_SUCCESS	0x2B

#define STATUS_DETECT_FAILED	0x2C
#define STATUS_DETECT_SUCCESS	0x2D

#define STATUS_NOT_RESPONDING	0x2E

#define GET_DEVICE_RAM_END			0x60			// Retrieve device's infomation
#define GET_DEVICE_RAM_START		0x61
#define GET_DEVICE_INFO_END			0x62
#define GET_DEVICE_INFO_START		0x63
#define GET_DEVICE_FLASH_END		0x64
#define GET_DEVICE_FLASH_START		0x65

#define GET_DEVICE_HAS_CPUX			0x66
#define GET_DEVICE_HAS_DATAQUICK	0x67
#define GET_DEVICE_ENH_VERIFY		0x68
#define GET_DEVICE_FAST_FLASH		0x69
#define GET_DEVICE_JTAG				0x6A
#define GET_DEVICE_SPYBIWIRE		0x6B
#define GET_DEVICE_HAS_TESTPIN		0x6C

#define DEVICE_NOT_MATCH			0xFFFF

#define STATUS_MSG_NOT_RESPRODING	"The programmer is not responding!"

// Define transfer data frame
//  CMD     | Lenght    | addr  | CRC   | data (max 480-byte)
//  1-byte  | 2-byte    | 2-byte| 2-byte| n-bytes

#define START_INDEX		7
#define TX_MAXLEN		(3 * 16)
#define USB_HID_BUFFER	64

// Global index point to target device
static WORD globalDeviceIdx = DEVICE_NOT_MATCH;

//! \brief Declaration of a struct type that holds all necessary device 
//! information 
struct tsDeviceFeatures {
    WORD Id;
	BYTE SubId;
    BYTE TestPin;
    BYTE CpuX;
    BYTE DataQuick;
    BYTE FastFlash;
    BYTE EnhVerify;
    BYTE JTAG;
    BYTE SpyBiWire;
    WORD RamStart;				// RAM
    WORD RamEnd;
	WORD FlashStart;			// FLASH
	WORD FlashEnd;
	WORD InfoStart;				// INFO
	WORD InfoEnd;
};


//! \brief This struct holds the device information for all relevant MSP430 
//! devices with the original or extended CPU architecture
static const struct tsDeviceFeatures sDeviceCPU89[] =
{
//                     TestPin      DataQuick      EnhVerify     SpyBiWire        RamEnd		  FlashEnd		  InfoEnd
//          Id  subId     |    CpuX     |  FastFlash  |    JTAG     |    RamStart    |   FlashStart	 |	 InfoStart	 |
//           |    |       |      |      |      |      |     |       |        |       |       |		 |		 |		 |

// AFE2xx: MSP430AFE22x
        { 0x0253, 0x00,  TRUE,  FALSE, TRUE , TRUE , FALSE, TRUE , TRUE  , 0xF000, 0xFFFF, 0xF000, 0x0000, 0x0000, 0x0000 },
//Memory3  = FLASH            Memory      0xF000      0xFFFF    R
//Memory3  = FLASH            Memory      0xF000      0xFFFF    R
//Memory3  = FLASH            Memory      0xC000      0xFFFF    R
//Memory3  = FLASH            Memory      0xE000      0xFFFF    R
//Memory3  = FLASH            Memory      0xE000      0xFFFF    R
//Memory3  = FLASH            Memory      0xC000      0xFFFF    R
//Memory3  = FLASH            Memory      0xF000      0xFFFF    R
//Memory3  = FLASH            Memory      0xE000      0xFFFF    R
//Memory3  = FLASH            Memory      0xC000      0xFFFF    R

// F11x1(A): MSP430F1121A
        { 0xF112, 0x00,  TRUE,  FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0xFC00, 0xFFFF, 0xF000, 0x0000, 0x0000, 0x0000 },
//Memory3  = FLASH            Memory      0xFC00      0xFFFF      R
//Memory3  = FLASH            Memory      0xF800      0xFFFF      R
//Memory3  = FLASH            Memory      0xFC00      0xFFFF      R
//Memory3  = FLASH            Memory      0xF000      0xFFFF      R
//Memory3  = FLASH            Memory      0xF800      0xFFFF      R
//Memory3  = FLASH            Memory      0xF000      0xFFFF      R

// F11x2: MSP430F1132
        { 0x1132, 0x00,  TRUE,  FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0xF000, 0xFFFF, 0xF000, 0x0000, 0x0000, 0x0000 },
//Memory3  = FLASH            Memory      0xF000      0xFFFF      R
//Memory3  = FLASH            Memory      0xE000      0xFFFF      R

// F12x(A): MSP430F123
        { 0xF123, 0x00,  TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE , FALSE , 0xF000, 0xFFFF, 0xF000, 0x0000, 0x0000, 0x0000 },
//Memory3  = FLASH            Memory      0xE000      0xFFFF      R
//Memory3  = FLASH            Memory      0xF000      0xFFFF      R

// F12x2 : MSP430F1232
        { 0x1232, 0x00,  TRUE,  FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0xF000, 0xFFFF, 0xF000, 0x0000, 0x0000, 0x0000 },
//Memory3  = FLASH            Memory      0xF000      0xFFFF      R
//Memory3  = FLASH            Memory      0xE000      0xFFFF      R

// F13x, F14x, F14x1 : MSP430F149
        { 0xF149, 0x00,  FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0xE000, 0xFFFF, 0xF000, 0x0000, 0x0000, 0x0000 },
 //Memory3  = FLASH            Memory      0xE000      0xFFFF      R
 //Memory3  = FLASH            Memory      0xC000      0xFFFF      R
 //Memory3  = FLASH            Memory      0x1100      0xFFFF      R
 //Memory3  = FLASH            Memory      0x8000      0xFFFF      R
 //Memory3  = FLASH            Memory      0x4000      0xFFFF      R
 //Memory3  = FLASH            Memory      0x8000      0xFFFF      R
 //Memory3  = FLASH            Memory      0x1100      0xFFFF      R
 //Memory3  = FLASH            Memory      0x4000      0xFFFF      R

// F15x, F16x : MSP430F169
        { 0xF169, 0x00,  FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x09FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F161x:   MSP430F1610  
        { 0xF16C, 0x00,  FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x1100, 0x24FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F20x1:
        { 0xF201, 0x01,  TRUE,  FALSE, TRUE , TRUE,  FALSE, TRUE , TRUE  , 0x0200, 0x027F, 0xF000, 0x0000, 0x0000, 0x0000 },

// F20x2:
        { 0xF201, 0x02,  TRUE,  FALSE, TRUE , TRUE,  FALSE, TRUE , TRUE  , 0x0200, 0x027F, 0xF000, 0x0000, 0x0000, 0x0000 },

// F20x3:
        { 0xF201, 0x03,  TRUE,  FALSE, TRUE , TRUE,  FALSE, TRUE , TRUE  , 0x0200, 0x027F, 0xF000, 0x0000, 0x0000, 0x0000 },

// F21x1:
        { 0xF213, 0x01,  TRUE,  FALSE, TRUE , FALSE, TRUE,  TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F21x2:
        { 0xF213, 0x02,  TRUE,  FALSE, TRUE , TRUE,  TRUE,  TRUE , TRUE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F22x2, F22x4, F2x44: MSP430F2274
        { 0xF227, 0x00,  TRUE,  FALSE, TRUE , TRUE,  TRUE,  TRUE , TRUE  , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F23x, F24x, F24x1, F2410:   MSP430F249  
        { 0xF249, 0x00,  FALSE, FALSE, TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x0200, 0x09FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F23x0: MSP430F2370
        { 0xF237, 0x00,  TRUE,  FALSE, TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x0200, 0x09FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F241x, F261x: MSP430F2619
        { 0xF26F, 0x00,  FALSE, TRUE,  TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x1100, 0x20FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F41x: MSP430F413
        { 0xF413, 0x00,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F41x2:
        { 0x4152, 0x00,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  TRUE , TRUE ,  0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F(E)42x:
        { 0xF427, 'E',   FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F(E)42xA :   MSP430FE427A
        { 0x427A, 'E',   FALSE, FALSE, TRUE , TRUE , FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F(G)42x0 :
        { 0xF427, 'G',   FALSE, FALSE, TRUE , FALSE ,FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F43x (80-pin):  MSP430F437
        { 0xF437, 0x00,  FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F43x,F44x (100-pin):
        { 0xF449, 0x00,  FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F471xx : 
        { 0xF47F, 0x00,  FALSE, TRUE,  TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// F47xx : 
        { 0xF449, 0x02,  FALSE, FALSE, TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// FE42x2 : 
        { 0x4252, 'E',   FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// FG43x:  MSP430FG439
        { 0xF439, 'G',   FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x09FF, 0xF000, 0x0000, 0x0000, 0x0000 },
        
// F(G)461x, F461x1: MSP430FG4619
        { 0xF46F, 'G',   FALSE, TRUE,  TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x1100, 0x20FF, 0xF000, 0x0000, 0x0000, 0x0000 },
        
// F(G)47x : 
        { 0xF479, 'G',   FALSE, FALSE, TRUE , TRUE,  TRUE,  TRUE , FALSE , 0x1100, 0x20FF, 0xF000, 0x0000, 0x0000, 0x0000 },
        
// FW428 :
        { 0xF429, 'W',   FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x1100, 0x20FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// FW429:
        { 0xF429, 'W',   FALSE, FALSE, TRUE , FALSE, FALSE, TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },
//------------------------------------------------------------------------------------------------------
// FW42x:
        { 0xF427, 'W',   FALSE, FALSE, TRUE , FALSE, FALSE,  TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// G2x01, G2x11 : 
        { 0xF201, 0x01,  TRUE,  FALSE, TRUE , TRUE,  FALSE,  TRUE , TRUE  , 0x0200, 0x027F, 0xF000, 0x0000, 0x0000, 0x0000 },

// G2x21, G2x31 : 
        { 0xF201, 0x02,  TRUE,  FALSE, TRUE , TRUE,  FALSE,  TRUE , TRUE  , 0x0200, 0x027F, 0xF000, 0x0000, 0x0000, 0x0000 },

// G2xx2: MSP430G2112
        { 0x2452, 0x00,  TRUE,  FALSE, TRUE , TRUE,  FALSE,  TRUE , TRUE  , 0x0200, 0x027F, 0xF000, 0x0000, 0x0000, 0x0000 },

// G2xx3: MSP430G2113
        { 0x2553, 0x00,  TRUE,  FALSE, TRUE , TRUE,  FALSE,  TRUE , TRUE  , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// G2x55: MSP430G2755
        { 0x2955, 0x00,  TRUE,  FALSE, TRUE , TRUE,  FALSE,  TRUE , TRUE  , 0x0200, 0x09FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// TCH5E: MSP430TCH5E
        { 0x255C, 0x00,  TRUE,  FALSE, TRUE , TRUE,  FALSE,  TRUE , TRUE  , 0x0200, 0x03FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// i20xx: MSP430i2040
        { 0x4020, 0x00,  TRUE,  FALSE, TRUE , TRUE,  TRUE,   TRUE , TRUE  , 0x0200, 0x05FF, 0xF000, 0x0000, 0x0000, 0x0000 },

// GENERIC:   
		{ 0xFFFF, 0x00,  FALSE, FALSE, TRUE , FALSE, FALSE,  TRUE , FALSE , 0x0200, 0x02FF, 0xF000, 0x0000, 0x0000, 0x0000 }
};



/****************************************************************************/
/* FUNCTIONS                                                                */
/****************************************************************************/

//! \brief This function accepts a Device ID and 
//! extracts the corresponding device information from the sDeviceCPU89 
//! struct
//! \param[in] wDeviceId Device ID (determined at runtime)
WORD SetDevice (WORD wDeviceId, BYTE wSubId);


//! \brief Function to check if current device has a test pin
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_TestPin(WORD DeviceIdx);


//! \brief Function to check if current device has the extended CPUX
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_CpuX(WORD DeviceIdx);


//! \brief Function to check if current device supports DataQuick
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_DataQuick(WORD DeviceIdx);


//! \brief Function to check if current device supports FastFlash
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_FastFlash(WORD DeviceIdx);


//! \brief Function to check if current device supports EnhVerify
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_EnhVerify(WORD DeviceIdx);


//! \brief Function to check if current device supports JTAG
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_JTAG(WORD DeviceIdx);


//! \brief Function to check if current device supports SpyBiWire
//! \return BYTE True = feature available, False = feature not available
BYTE DeviceHas_SpyBiWire(WORD DeviceIdx);


//! \brief This function returns the start address of the device's RAM
//! \return WORD Start address of the target device's RAM memory
WORD Device_RamStart(WORD DeviceIdx);
WORD Device_RamEnd(WORD DeviceIdx);

WORD Device_FlashStart(WORD DeviceIdx);
WORD Device_FlashEnd(WORD DeviceIdx);

WORD Device_InfoStart(WORD DeviceIdx);
WORD Device_InfoEnd(WORD DeviceIdx);

