﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.IO.Ports;
using ComTypes = System.Runtime.InteropServices.ComTypes;

using System.Collections.ObjectModel;


namespace AppForm
{    
    public partial class frmMSP430 : Form
    {
        // Define message color
        const int COLOR_RED     = 0x01;
        const int COLOR_GREEN   = 0x02;
        const int COLOR_BLACK   = 0x03;
        const int COLOR_BROWN   = 0x04;
        

        // Define command
        const byte CMD_READ = 0x11;
        const byte CMD_READ_END = 0x12;
        const byte CMD_WRITE = 0x13;
        const byte CMD_WRITE_FIRST = 0x1A;
        const byte CMD_WRITE_END = 0x14;
        const byte CMD_VERIFY = 0x15;
        const byte CMD_VERIFY_END = 0x16;
        const byte CMD_DETECT = 0x17;
        const byte CMD_ERASE = 0x18;
        const byte CMD_HWTEST = 0x19;
        const byte CMD_POWERON = 0x20;

        // Status code
        const int STATUS_SUCCESS = 0x01;
        const int STATUS_ERROR = 0x00;

        const int STATUS_CRC_FAILED = 0x33;
        const int STATUS_CRC_SUCCESS = 0x34;

        const int STATUS_WRITE_FAILED = 0x35;
        const int STATUS_WRITE_SUCCESS = 0x36;

        const int STATUS_VERIFY_FAILED = 0x37;
        const int STATUS_VERIFY_SUCCESS = 0x38;

        const int STATUS_POWERON_FAILED = 0x39;
        const int STATUS_POWERON_SUCCESS = 0x40;

        const int STATUS_ERASE_FAILED = 0x41;
        const int STATUS_ERASE_SUCCESS = 0x42;

        const int STATUS_HWTEST_FAILED = 0x43;
        const int STATUS_HWTEST_SUCCESS = 0x44;

        const int STATUS_DETECT_FAILED = 0x45;
        const int STATUS_DETECT_SUCCESS = 0x46;
                
        // Define transfer data frame
        //  CMD     | Lenght    | addr  | CRC   | data (max 480-byte)
        //  1-byte  | 2-byte    | 2-byte| 2-byte| n-bytes

        const int START_INDEX   = 7;
        const int TX_MAXLEN     = 30 * 16;
        byte    preCommand      = 0x00;
        int     usbIndex        = 0x00;
        int     usbAddress      = 0x00;
        byte[]  usbDataFrame    = new byte[START_INDEX + TX_MAXLEN];
        
        // Path contains hex file
        string txtFilePath      = "";
        string txtFileName      = "";
        
        // Contains code data
        string txtTIHex         = "";

        // For COM ports
        private delegate void SetTextDeleg(byte[] text);
        SerialPort UsbSerial = new SerialPort();

        // Create new instance of MSP430lib
        MSP430lib MP430func = new MSP430lib();

        /* =========================================================================
         * Function     : frmMSP430
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        public frmMSP430()
        {
            InitializeComponent();
        }

        /* =========================================================================
         * Function     : updateMessageLog
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        void updateMessageLog(string txt, int color)
        {
            // Update message log
            txt += Environment.NewLine;

            switch (color)
            {
                case COLOR_BLACK:
                    textBoxLog.AppendText(txt, Color.Black);
                    break;
                case COLOR_RED:
                    textBoxLog.AppendText(txt, Color.Red);
                    break;
                case COLOR_GREEN:
                    textBoxLog.AppendText(txt, Color.Green);
                    break;
                case COLOR_BROWN:
                    textBoxLog.AppendText(txt, Color.Brown);
                    break;
                default:
                    break;
            }
            
        }

        /* =========================================================================
         * Function     : Init_SerialUSB
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void Init_SerialUSB(string usb_port)
        {
            try
            {
                UsbSerial = new SerialPort(usb_port);
                UsbSerial.BaudRate = 9600;
                UsbSerial.Parity = Parity.None;
                UsbSerial.StopBits = StopBits.One;
                UsbSerial.DataBits = 8;
                UsbSerial.ReadTimeout = -1;
                UsbSerial.Handshake = Handshake.None;
                UsbSerial.DataReceived += new SerialDataReceivedEventHandler(DataReceivedHandler);
                UsbSerial.Open();

                // Update success message
                updateMessageLog("Serial port " + usb_port + " opened", COLOR_GREEN);
            }
            catch
            {
                // Update error message
                updateMessageLog("Serial port " + usb_port + " openning error!", COLOR_RED);
            }
        }

        /* =========================================================================
         * Function     : DataReceivedHandler
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void DataReceivedHandler(object sender, SerialDataReceivedEventArgs e)
        {
            int numOfByte = 5;
            byte[] buffer = new byte[numOfByte];
            SerialPort sp = (SerialPort)sender;

            // Read data from usb port.
            sp.Read(buffer, 0, numOfByte);
            this.BeginInvoke(new SetTextDeleg(Excute_Command), new object[] { buffer });
        }

        /* =========================================================================
         * Function     : Excute_Command
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void Excute_Command(byte[] buffer)
        {
            switch (buffer[0])
            {                
                case STATUS_CRC_FAILED:
                    // Re-send previous data to host
                    TX_TransmitData(UsbSerial, usbDataFrame, preCommand);
                    break;
                
                case STATUS_DETECT_SUCCESS:
                    // Print success message
                    updateMessageLog("Detected device:", COLOR_GREEN);

                    updateMessageLog("JTAG ID: 0x" + buffer[1].ToString("X2"), COLOR_BLACK);
                    updateMessageLog("Revision: " + buffer[2].ToString("X2") + buffer[3].ToString("X2") + " - " + buffer[4].ToString("X2"), COLOR_BLACK);

                    // Erase all data before write to target                    
                    //TX_TransmitData(UsbSerial, usbDataFrame, CMD_ERASE);
                    break;

                case STATUS_DETECT_FAILED:
                    // Print success message
                    updateMessageLog("Detect device failed!", COLOR_GREEN);
                    // Close COM Port
                    UsbSerial.Close();
                    break;

                case STATUS_ERASE_SUCCESS:
                    // Print success message
                    updateMessageLog("Erase FLASH was successful!", COLOR_GREEN);
                    // Start transmit data to programmer
                    TX_WriteFLASH(txtTIHex);
                    break;

                case STATUS_ERASE_FAILED:
                    // Print success message
                    updateMessageLog("Erase FLASH failed!", COLOR_GREEN);
                    // Close COM Port
                    UsbSerial.Close();
                    break;

                case STATUS_WRITE_SUCCESS:
                    // Start transmit data to host
                    TX_WriteFLASH(txtTIHex);
                    break;

                case STATUS_WRITE_FAILED:
                    // Print error message
                    updateMessageLog("An error orccured when trying to write to target!", COLOR_RED);
                    // Close COM Port
                    UsbSerial.Close();
                    break;

                case STATUS_SUCCESS:
                    // Print success message
                    updateMessageLog("Done!", COLOR_GREEN);
                    // Close COM Port
                    UsbSerial.Close();
                    break;

                default:
                    break;
            }
        }
                

        /* =========================================================================
         * Function     : btnOpenRecentFiles_ButtonClick
         * Description  : 
         *                hex430 --ti_txt "CCS.out" -o "CSS1.txt" -order MS -romwidth 16
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void btnOpenRecentFiles_ButtonClick(object sender, EventArgs e)
        {
            OpenFileDialog openTXT = new OpenFileDialog();

            openTXT.Title = "Select your hex file...";
            openTXT.Filter = "Hex file (*.txt,*.out)|*.txt;*.out";
            openTXT.FilterIndex = 1;
            openTXT.InitialDirectory = "Documents";
            openTXT.Multiselect = false;

            if (openTXT.ShowDialog() == DialogResult.OK)
            {
                // Update message log
                updateMessageLog("Opened hex file...", COLOR_GREEN);

                updateMessageLog(openTXT.SafeFileName, COLOR_BLACK);

                // Get TEMP folder
                txtFilePath = System.Environment.GetEnvironmentVariable("TEMP") + "\\" + 
                          openTXT.SafeFileName.Substring(0, openTXT.SafeFileName.Length - 3) + "txt";

                // Get file extension
                string ext = openTXT.FileName.Substring(openTXT.FileName.Length - 3, 3);

                // Get file name of txt file. Doesn't include path
                txtFileName = openTXT.SafeFileName;
                
                string agrument = "--ti_txt " + "\"" + openTXT.FileName + "\"" + " -o " + "\"" + txtFilePath + 
                                  "\"" + " -order MS -romwidth 16";

                if (ext == "out")
                {
                    // Convert *.out -> *.txt
                    ProcessStartInfo hex430 = new ProcessStartInfo();
                    hex430.FileName = "hex430.exe";
                    hex430.Arguments = agrument;
                    hex430.WindowStyle = ProcessWindowStyle.Hidden;
                    Process.Start(hex430);
                }
                else
                {
                    // Get *.txt path
                    txtFilePath = openTXT.FileName;
                }

                // Update textbox hex
                textBoxHex.Text = "";
                string[] lines = File.ReadAllLines(txtFilePath);
                foreach (string line in lines)
                {
                    textBoxHex.AppendText(line + Environment.NewLine, Color.Black);
                }
            }
        }

        /* =========================================================================
         * Function     : btnReload_Click
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void btnReload_Click(object sender, EventArgs e)
        {
            string txt;

            // Update textbox hex
            textBoxHex.Text = "";
            string[] lines = File.ReadAllLines(txtFilePath);

            foreach (string line in lines)
            {
                textBoxHex.AppendText(line + Environment.NewLine, Color.Black);                
            }

            // Update message log
            txt = "Reload hex file..." + Environment.NewLine + txtFileName;
            updateMessageLog(txt, COLOR_BLACK);
        }

        /* =========================================================================
         * Function     : btnDetectDevice_Click
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void btnDetectDevice_Click(object sender, EventArgs e)
        {
            //// Get All ComPorts
            //string txt;
            //List<Win32DeviceMgmt.DeviceInfo> listPorts = Win32DeviceMgmt.GetAllCOMPorts();

            //foreach (Win32DeviceMgmt.DeviceInfo port in listPorts)
            //{
            //    txt = port.decsription + Environment.NewLine;
            //    txt += port.name;
            //    updateMessageLog(txt, COLOR_BLACK);
            //}

            //// Get a list of serial port names. 
            //string[] ports = SerialPort.GetPortNames();
            
            //// Display each port name to the console. 
            //foreach (string port in ports)
            //{
            //    updateMessageLog(port, COLOR_BLACK);
            //}

            if (UsbSerial.IsOpen == false)
                Init_SerialUSB("COM84");

            // Detect device...
            TX_TransmitData(UsbSerial, usbDataFrame, CMD_DETECT);
        }

        /* =========================================================================
         * Function     : btnHardwareTest_Click
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        private void btnHardwareTest_Click(object sender, EventArgs e)
        {

        }

        
        /* =========================================================================
         * Function     : btnProgram_Click
         * Description  : 
         * Agruments    : 
         * Return       : None.
         * ========================================================================= */
        private void btnProgram_Click(object sender, EventArgs e)
        {
            // Open COM Port
            //Init_SerialUSB("COM34");

            if(UsbSerial.IsOpen == false)
                Init_SerialUSB("COM84");

            if (textBoxHex.Text.Length > 0)
            {
                // Copy hex data to global variable
                txtTIHex = textBoxHex.Text;

                // Erase all FLASH
                TX_TransmitData(UsbSerial, usbDataFrame, CMD_ERASE);

                //// Start transmit data to programmer
                //TX_WriteFLASH(txtTIHex);
            }
            else
            {
                // Update message log
                updateMessageLog("Please! Choose your TI-Hex file first!", COLOR_GREEN);
            }
        }


        /* =========================================================================
         * Function     : TX_WriteFLASH
         * Description  : 
         * Agruments    : 
         * Return       : None.
         * ========================================================================= */
        public int TX_WriteFLASH(string txtHex)
        {
            string line;
            int i, indexStart, indexEnd;

            // Initialize variables
            indexStart = 0x00;
            usbIndex = 0x00;
            usbAddress = 0x00;

            while (true)
            {
                // Read a line of hex-txt
                indexEnd = txtHex.IndexOf('\n', indexStart);
                line = txtHex.Substring(indexStart, indexEnd - indexStart);
                
                // Beginning of a new address
                if (line.Substring(0, 1) == "@")
                {
                    // Is data available before?
                    if (usbIndex > 0)
                    {
                        // Update LENGTH of sequence of data
                        usbIndex = usbIndex - START_INDEX;
                        usbDataFrame[1] = Convert.ToByte(usbIndex >> 8);
                        usbDataFrame[2] = Convert.ToByte(usbIndex & 0xFF);

                        // Transmit block of data to host
                        TX_TransmitData(UsbSerial, usbDataFrame, CMD_WRITE);

                        // Update address to TI-TXT hex
                        txtTIHex = txtTIHex.Substring(indexStart, txtTIHex.Length - indexStart);

                        // Exit while loop
                        break;
                    }
                    else
                    {
                        // Index of first byte of data sequence
                        usbIndex = START_INDEX;

                        // Start address of block
                        usbDataFrame[3] = Convert.ToByte(line.Substring(1, 2), 16);   // High-byte
                        usbDataFrame[4] = Convert.ToByte(line.Substring(3, 2), 16);   // Low-byte
                    }
                }
                else if (line == "q")
                {
                    // Process when reach end of source code

                    // Update length of sequence of data
                    usbIndex = usbIndex - START_INDEX;
                    usbDataFrame[1] = Convert.ToByte(usbIndex >> 8);
                    usbDataFrame[2] = Convert.ToByte(usbIndex & 0xFF);

                    // Transmit block of data to programmer
                    TX_TransmitData(UsbSerial, usbDataFrame, CMD_WRITE_END);

                    // Exit while loop
                    break;
                }
                else
                {
                    // Remove last character: " "
                    if (line.Substring(line.Length - 1, 1) == " ")
                        line = line.Substring(0, line.Length - 1);

                    // Parsing contents of a line to bytes
                    for (i = 0; i < line.Length; i += 3)
                    {
                        // Continue update block of data
                        usbDataFrame[usbIndex++] = Convert.ToByte(line.Substring(i, 2), 16);
                    }

                    if (usbIndex >= (TX_MAXLEN + START_INDEX))
                    {
                        // The index has exceeded block of data

                        // Update length of sequence of data
                        usbIndex = usbIndex - START_INDEX;
                        usbDataFrame[1] = Convert.ToByte(usbIndex >> 8);
                        usbDataFrame[2] = Convert.ToByte(usbIndex & 0xFF);

                        // Transmit block of data to programmer
                        TX_TransmitData(UsbSerial, usbDataFrame, CMD_WRITE);

                        // Update new address
                        usbAddress = (usbDataFrame[3] << 8) + usbDataFrame[4] + usbIndex;

                        // Update address to TI-TXT hex
                        txtTIHex = "@" + usbAddress.ToString("X4") + '\n' + txtTIHex.Substring(usbIndex, txtTIHex.Length - usbIndex);

                        // Exit while loop
                        break;
                    }
                }

                // Increase index in TI-TXT
                indexStart = indexEnd + 1;
            }

            // Success
            return STATUS_SUCCESS;
        }

        /* =========================================================================
         * Function     : TX_TransmitData
         * Description  : 
         * Agruments    : 
         * Return       : None.
         * ========================================================================= */
        public int TX_TransmitData(SerialPort usbPort, byte[] data, byte command)
        {
            int length, address;

            // Assign command
            data[0] = command;
            preCommand = command;

            // Get length of sequence of data
            length = (data[1] << 8) + data[2] + START_INDEX;

            // Retrieve address
            address = (data[3] << 8) + data[4];

            try
            {
                usbPort.Write(data, 0, length);

                if ((command == CMD_WRITE) || (command == CMD_WRITE_END))
                {
                    length = length - START_INDEX;
                    updateMessageLog("Sent sequence:", COLOR_GREEN);
                    updateMessageLog("Address: " + address.ToString("X4"), COLOR_BLACK);
                    updateMessageLog("Length: " + length.ToString(), COLOR_BLACK);
                }
            }
            catch (Exception e)
            {
                // Update message log
                updateMessageLog(e.ToString(), COLOR_RED);
                return STATUS_ERROR;
            }

            // Success
            return STATUS_SUCCESS;
        }

        private void btnEraseAll_Click(object sender, EventArgs e)
        {
            TX_TransmitData(UsbSerial, usbDataFrame, CMD_ERASE);
        }
    }
     
    public class MSP430lib
    {
        //[DllImport("MSP430lib.dll")]
        //public static extern unsafe UInt16 JTAG_DetectDevice(UInt16 *msp430Family, char* msp430Prefix, UInt16 *msp430PartNumber);

        //[DllImport("BSL430.dll")]
        ////static extern unsafe UInt16 JTAG_HardwareTest(UInt16* version, string* manufacturer, string* product);
        //public static extern int BSL_openTI_TextForRead(IntPtr filename);


        struct MSP430CPU
        {
            private readonly int Id, subId;
            private readonly bool TestPin;
            private readonly bool CpuX;
            private readonly bool DataQuick;
            private readonly bool FastFlash;
            private readonly bool EnhVerify;
            private readonly bool JTAG;
            private readonly bool SpyBiWire;
            private readonly int RamStart;
            private readonly int RamEnd;
            private readonly int MainStart;

            public MSP430CPU(int Id, int subId, bool TestPin, bool CpuX, bool DataQuick, bool FastFlash, bool EnhVerify,
                            bool JTAG, bool SpyBiWire, int RamStart, int RamEnd, int MainStart)
            {
                this.Id = Id;
                this.subId = subId;
                this.TestPin = TestPin;
                this.CpuX = CpuX;
                this.DataQuick = DataQuick;
                this.FastFlash = FastFlash;
                this.EnhVerify = EnhVerify;
                this.JTAG = JTAG;
                this.SpyBiWire = SpyBiWire;
                this.RamStart = RamStart;
                this.RamEnd = RamEnd;
                this.MainStart = MainStart;
            }

            public int DeviceGetId { get { return Id; } }
            public int DeviceGetSubId { get { return subId; } }
            public bool DeviceHas_TestPin { get { return TestPin; } }
            public bool DeviceHas_CpuX { get { return CpuX; } }
            public bool DeviceHas_DataQuick { get { return DataQuick; } }
            public bool DeviceHas_FastFlash { get { return FastFlash; } }
            public bool DeviceHas_EnhVerify { get { return EnhVerify; } }
            public bool DeviceHas_JTAG { get { return JTAG; } }
            public bool DeviceHas_SpyBiWire { get { return SpyBiWire; } }
            public int Device_RamStart { get { return RamStart; } }
            public int Device_RamEnd { get { return RamEnd; } }
            public int Device_MainStart { get { return MainStart; } }
        }

        /* =========================================================================
         * Function     : SetDevice
         * Description  : Get index of the device corresponding with Id and subId.
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        public int SetDevice(int id, int subId)
        {
            // Lookup for the first time
            for (int i = 0; i < MSP430CPU0.Count; i++)
            {
                if ((MSP430CPU0[i].DeviceGetId == id) && (MSP430CPU0[i].DeviceGetSubId == subId))
                {
                    // Return index
                    return i;
                }
            }

            // Lookup for the second time. Ignore subId
            for (int i = 0; i < MSP430CPU0.Count; i++)
            {
                if (MSP430CPU0[i].DeviceGetId == id)
                {
                    // Return index
                    return i;
                }
            }

            // Not found
            return (-1);
        }

        // Infomation:
        //
        // Not yet update: RamStart, RamEnd, MainStart
        //

        static readonly IList<MSP430CPU> MSP430CPU0 = new ReadOnlyCollection<MSP430CPU>
            (new[] {
        //                          TestPin      DataQuick      EnhVerify     SpyBiWire        RamEnd
        //               Id  subId     |    CpuX     |  FastFlash  |    JTAG     |    RamStart    |   MainStart
        //                |    |       |      |      |      |      |     |       |        |       |       |
// AFE2xx: MSP430AFE22x
        new MSP430CPU( 0x0253, 0xFF,  true,  false, true , true , false, true , true  , 0x0200, 0x02FF, 0xF000 ), //

// F11x1(A): MSP430F1121A
        new MSP430CPU( 0xF112, 0xFF,  true,  false, true , false, false, true , false , 0x0200, 0x02FF, 0xF000 ), //

// F11x2: MSP430F1132
        new MSP430CPU( 0x1132, 0xFF,  true,  false, true , false, false, true , false , 0x0200, 0x02FF, 0xE000 ), //

// F12x(A): MSP430F123
        new MSP430CPU( 0xF123, 0xFF,  true,  false, false, false, false, true , false , 0x0200, 0x02FF, 0xE000 ), 

// F12x2 : MSP430F1232
        new MSP430CPU( 0x1232, 0xFF,  true,  false, true , false, false, true , false , 0x0200, 0x02FF, 0xE000 ), 

// F13x, F14x, F14x1 : MSP430F149
        new MSP430CPU( 0xF149, 0xFF,  false, false, true , false, false, true , false , 0x0200, 0x09FF, 0x1100 ),
 
// F15x, F16x : MSP430F169
        new MSP430CPU( 0xF169, 0xFF,  false, false, true , false, false, true , false , 0x0200, 0x09FF, 0x1100 ), 

// F161x:   MSP430F1610  
        new MSP430CPU( 0xF16C, 0xFF,  false, false, true , false, false, true , false , 0x1100, 0x24FF, 0x8000 ), 

// F20x1:
        new MSP430CPU( 0xF201, 0x01,  true,  false, true , true,  false, true , true  , 0x0200, 0x027F, 0xF800 ), 

// F20x2:
        new MSP430CPU( 0xF201, 0x02,  true,  false, true , true,  false, true , true  , 0x0200, 0x027F, 0xF800 ), 

// F20x3:
        new MSP430CPU( 0xF201, 0x03,  true,  false, true , true,  false, true , true  , 0x0200, 0x027F, 0xF800 ), 

// F21x1:
        new MSP430CPU( 0xF213, 0x01,  true,  false, true , false, true, true , false , 0x0200, 0x02FF, 0xE000 ), 

// F21x2:
        new MSP430CPU( 0xF213, 0x02,  true,  false, true , true,  true, true , true , 0x0200, 0x02FF, 0xE000 ), 

// F22x2, F22x4, F2x44: MSP430F2274
        new MSP430CPU( 0xF227, 0xFF,  true,  false, true , true,  true,  true , true  , 0x0200, 0x05FF, 0x8000 ), 

// F23x, F24x, F24x1, F2410:   MSP430F249  
        new MSP430CPU( 0xF249, 0xFF,  false, false, true , true,  true,  true , false , 0x0200, 0x09FF, 0x1100 ), 

// F23x0: MSP430F2370
        new MSP430CPU( 0xF237, 0xFF,  true,  false, true , true,  true,  true , false , 0x0200, 0x09FF, 0x8000 ), 

// F241x, F261x: MSP430F2619
        new MSP430CPU( 0xF26F, 0xFF,  false, true,  true , true,  true,  true , false , 0x1100, 0x20FF, 0x2100 ), 

// F41x: MSP430F413
        new MSP430CPU( 0xF413, 0xFF,  false, false, false, false, false, true , false , 0x0200, 0x02FF, 0xE000 ), 

// F41x2:
        new MSP430CPU( 0x4152, 0xFF,  true,  false, true,  true,  true,  true , true , 0x0200, 0x02FF, 0xE000 ), 

// F(E)42x:
        new MSP430CPU( 0xF427, 'E',   false, false, true , false, false, true , false , 0x0200, 0x02FF, 0xE000 ),

// F(E)42xA :   MSP430FE427A
        new MSP430CPU( 0x427A, 'E',   false, false, true , true , false, true , false , 0x0200, 0x02FF, 0xE000 ), 

// F(G)42x0 :
        new MSP430CPU( 0xF427, 'G',   false, false, true , false , false, true , false , 0x0200, 0x02FF, 0xE000 ), 

// F43x (80-pin):  MSP430F437
        new MSP430CPU( 0xF437, 0xFF,  false, false, true , false, false, true , false , 0x0200, 0x05FF, 0xA000 ), 

// F43x,F44x (100-pin):
        new MSP430CPU( 0xF449, 0xFF,  false, false, true , false, false, true , false , 0x0200, 0x05FF, 0xA000 ), 

// F471xx : 
        new MSP430CPU( 0xF47F, 0xFF,  false, true,  true , true,  true,  true , false , 0x0200, 0x05FF, 0xA000 ), 

// F47xx : 
        new MSP430CPU( 0xF449, 0x02,  false, false, true , true,  true,  true , false , 0x0200, 0x05FF, 0xA000 ), 

// FE42x2 : 
        new MSP430CPU( 0x4252, 'E',   false, false, true , false, false, true , false , 0x0200, 0x05FF, 0xA000 ), 

// FG43x:  MSP430FG439
        new MSP430CPU( 0xF439, 'G',   false, false, true , false, false, true , false , 0x0200, 0x09FF, 0x1100 ), 
        
// F(G)461x, F461x1: MSP430FG4619
        new MSP430CPU( 0xF46F, 'G',   false, true,  true , true,  true,  true , false , 0x1100, 0x20FF, 0x2100 ), 
        
// F(G)47x : 
        new MSP430CPU( 0xF479, 'G',   false, false, true , true,  true,  true , false , 0x1100, 0x20FF, 0x2100 ), 
        
// FW428 :
        new MSP430CPU( 0xF429, 'W',   false, false, true , false, false, true , false , 0x1100, 0x20FF, 0x2100 ), 

// FW429:
        new MSP430CPU( 0xF429, 'W',   false, false, true , false, false, true , false , 0x0200, 0x02FF, 0xE000 ),

// FW42x:
        new MSP430CPU( 0xF427, 'W',   false, false, true , false, false, true , false , 0x0200, 0x02FF, 0xE000 ),

// G2x01, G2x11 : 
        new MSP430CPU( 0xF201, 0x01,  true,  false, true , true, false,  true , true  , 0x0200, 0x027F, 0xFC00 ), 

// G2x21, G2x31 : 
        new MSP430CPU( 0xF201, 0x02,  true,  false, true , true, false,  true , true  , 0x0200, 0x027F, 0xFC00 ), 

// G2xx2: MSP430G2112
        new MSP430CPU( 0x2452, 0xFF,  true,  false, true , true, false,  true , true  , 0x0200, 0x027F, 0xFC00 ), 

// G2xx3: MSP430G2113
        new MSP430CPU( 0x2553, 0xFF,  true,  false, true , true, false,  true , true  , 0x0200, 0x02FF, 0xFC00 ), 

// G2x55: MSP430G2755
        new MSP430CPU( 0x2955, 0xFF,  true,  false, true , true,  true,  true , true  , 0x0200, 0x09FF, 0x8000 ), 

// TCH5E: MSP430TCH5E
        new MSP430CPU( 0x255C, 0xFF,  true,  false, true , true, false,  true , true  , 0x0200, 0x03FF, 0xC000 ), 

// i20xx: MSP430i2040
        new MSP430CPU( 0x4020, 0xFF,  true,  false, true , true,  true,  true , true  , 0x0200, 0x05FF, 0xC000 ), 

// GENERIC:   
        new MSP430CPU( 0xFFFF, 0xFF,  false, false, true , false, false, true , false , 0x0200, 0x02FF, 0xE000 )
        });        
    }

    public class Win32DeviceMgmt
    {
        private const UInt32 DIGCF_PRESENT = 0x00000002;
        private const UInt32 DIGCF_DEVICEINTERFACE = 0x00000010;
        private const UInt32 SPDRP_DEVICEDESC = 0x00000000;
        private const UInt32 DICS_FLAG_GLOBAL = 0x00000001;
        private const UInt32 DIREG_DEV = 0x00000001;
        private const UInt32 KEY_QUERY_VALUE = 0x0001;
        private const string GUID_DEVINTERFACE_COMPORT = "{86E0D1E0-8089-11D0-9CE4-08003E301F73}";

        [StructLayout(LayoutKind.Sequential)]
        private struct SP_DEVINFO_DATA
        {
            public Int32  cbSize;
            public Guid   ClassGuid;
            public Int32  DevInst;
            public UIntPtr Reserved;
        };

        [DllImport("setupapi.dll")]
        private static extern Int32 SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);

        [DllImport("setupapi.dll")]
        private static extern bool SetupDiEnumDeviceInfo(IntPtr DeviceInfoSet, Int32 MemberIndex, ref  SP_DEVINFO_DATA DeviceInterfaceData);

        [DllImport("setupapi.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern bool SetupDiGetDeviceRegistryProperty(IntPtr deviceInfoSet, ref SP_DEVINFO_DATA deviceInfoData,
        uint property, out UInt32 propertyRegDataType, StringBuilder propertyBuffer, uint propertyBufferSize, out UInt32 requiredSize);

        [DllImport("setupapi.dll", SetLastError = true)]
        private static extern IntPtr SetupDiGetClassDevs(ref Guid gClass, UInt32 iEnumerator, IntPtr hParent, UInt32 nFlags);

        [DllImport("Setupapi", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr SetupDiOpenDevRegKey(IntPtr hDeviceInfoSet, ref SP_DEVINFO_DATA deviceInfoData, uint scope,
        uint hwProfile, uint parameterRegistryValueKind, uint samDesired);

        [DllImport("advapi32.dll", CharSet = CharSet.Unicode, EntryPoint = "RegQueryValueExW", SetLastError = true)]
        private static extern int RegQueryValueEx(IntPtr hKey, string lpValueName, int lpReserved, out uint lpType,
        StringBuilder lpData, ref uint lpcbData);

        [DllImport("advapi32.dll", CharSet = CharSet.Ansi, SetLastError = true, ExactSpelling = true)]
        private static extern int RegCloseKey(IntPtr hKey);

        [DllImport("kernel32.dll")]
        private static extern Int32 GetLastError();

        public struct DeviceInfo
        {
            public string name;
            public string decsription;
        }

        public static List<DeviceInfo> GetAllCOMPorts()
        {
            Guid guidComPorts = new Guid(GUID_DEVINTERFACE_COMPORT);
            IntPtr hDeviceInfoSet = SetupDiGetClassDevs(ref guidComPorts, 0, IntPtr.Zero, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

            if (hDeviceInfoSet == IntPtr.Zero)
            {
                throw new Exception("Failed to get device information set for the COM ports");
            }

            try 
            {
                List<DeviceInfo>  devices = new List< DeviceInfo> ();
                Int32 iMemberIndex = 0;

                while (true)
                {
                    SP_DEVINFO_DATA deviceInfoData = new SP_DEVINFO_DATA();
                    deviceInfoData.cbSize = Marshal.SizeOf(typeof(SP_DEVINFO_DATA));
                    bool success = SetupDiEnumDeviceInfo(hDeviceInfoSet, iMemberIndex, ref deviceInfoData);

                    if (!success)
                    {
                        // No more devices in the device information set
                        break;
                    }

                    DeviceInfo deviceInfo = new DeviceInfo();
                    deviceInfo.name = GetDeviceName(hDeviceInfoSet, deviceInfoData);
                    deviceInfo.decsription = GetDeviceDescription(hDeviceInfoSet, deviceInfoData);
                    devices.Add(deviceInfo);
                    iMemberIndex++;
                }
                return devices;
            }

            finally 
            {
                SetupDiDestroyDeviceInfoList(hDeviceInfoSet);
            }
        }

        private static string GetDeviceName(IntPtr pDevInfoSet, SP_DEVINFO_DATA deviceInfoData)
        {
            IntPtr hDeviceRegistryKey = SetupDiOpenDevRegKey(pDevInfoSet, ref deviceInfoData,
            DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);

            if (hDeviceRegistryKey == IntPtr.Zero)
            {
                throw new Exception("Failed to open a registry key for device-specific configuration information");
            }

            StringBuilder deviceNameBuf = new StringBuilder(256);

            try
            {
                uint lpRegKeyType;
                uint length = (uint)deviceNameBuf.Capacity;
                int result = RegQueryValueEx(hDeviceRegistryKey, "PortName", 0, out lpRegKeyType, deviceNameBuf, ref length);

                if (result != 0)
                {
                    throw new Exception("Can not read registry value PortName for device " + deviceInfoData.ClassGuid);
                }
            }

            finally
            {
                RegCloseKey(hDeviceRegistryKey);
            }

            string deviceName = deviceNameBuf.ToString();
            return deviceName;
        }

        private static string GetDeviceDescription(IntPtr hDeviceInfoSet, SP_DEVINFO_DATA deviceInfoData)
        {
            StringBuilder descriptionBuf = new StringBuilder(256);
            uint propRegDataType;
            uint length = (uint)descriptionBuf.Capacity;
            bool success = SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, ref deviceInfoData, SPDRP_DEVICEDESC,
            out propRegDataType, descriptionBuf, length, out length);

            if (!success)
            {
                throw new Exception("Can not read registry value PortName for device " + deviceInfoData.ClassGuid);
            }

            string deviceDescription = descriptionBuf.ToString();
            return deviceDescription;
        }
    }


    /*====================================================================
     * Example:
     * txt += "MSP430" + msp430Prefix.ToString() + msp430PartNumber.ToString();
     * txt += Environment.NewLine;
     * textBoxLog.AppendText(txt, Color.Green);
     * ==================================================================*/
    public static class RichTextBoxExtensions
    {
        public static void AppendText(this RichTextBox box, string text, Color color)
        {
            box.SelectionStart = box.TextLength;
            box.SelectionLength = 0;

            box.SelectionColor = color;
            box.AppendText(text);
            box.SelectionColor = box.ForeColor;
        }
    }
}
