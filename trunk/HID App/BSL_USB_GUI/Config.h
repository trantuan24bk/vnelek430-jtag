
// Define message color
#define COLOR_RED      0x01
#define COLOR_GREEN    0x02
#define COLOR_BLACK    0x03
#define COLOR_BROWN    0x04

// Define command
#define CMD_READ			0x11
#define CMD_READ_END		0x12
#define CMD_WRITE			0x13
#define CMD_WRITE_FIRST		0x1A
#define CMD_WRITE_END		0x14
#define CMD_VERIFY			0x15
#define CMD_VERIFY_END		0x16
#define CMD_DETECT			0x17
#define CMD_ERASE			0x18
#define CMD_HWTEST			0x19
#define CMD_POWERON			0x20

// Status code
#define STATUS_SUCCESS			0x01
#define STATUS_ERROR			0x00

#define STATUS_CRC_FAILED		0x33
#define STATUS_CRC_SUCCESS		0x34

#define STATUS_WRITE_FAILED		0x35
#define STATUS_WRITE_SUCCESS	0x36

#define STATUS_VERIFY_FAILED	0x37
#define STATUS_VERIFY_SUCCESS	0x38

#define STATUS_POWERON_FAILED	0x39
#define STATUS_POWERON_SUCCESS  0x40

#define STATUS_ERASE_FAILED		0x41
#define STATUS_ERASE_SUCCESS	0x42

#define STATUS_HWTEST_FAILED	0x43
#define STATUS_HWTEST_SUCCESS	0x44

#define STATUS_DETECT_FAILED	0x45
#define STATUS_DETECT_SUCCESS	0x46

#define STATUS_NOT_RESPONDING	0x50

// Define transfer data frame
//  CMD     | Lenght    | addr  | CRC   | data (max 480-byte)
//  1-byte  | 2-byte    | 2-byte| 2-byte| n-bytes

#define START_INDEX		7
#define TX_MAXLEN		(3 * 16)
#define USB_HID_BUFFER	64


#define STATUS_MSG_NOT_RESPRODING			"The programmer is not responding!"

typedef unsigned char byte;