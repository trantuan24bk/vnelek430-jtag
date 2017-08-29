/*
 * MSP430 USB Firmware Upgrade Example
 *
 * An example software to field firmware upgrade a MSP430 USB based device
 *
 * Copyright (C) {2010} Texas Instruments Incorporated - http://www.ti.com/ 
 * 
*/
#pragma once

#include "BSL430_dll.h"
#include "CRC.h"
#include <windows.h>
#include <Winuser.h>
#include <Dbt.h>
#include <string.h>
#include "Device430.h"

#using <mscorlib.dll>				// Invoke external process
#using <System.dll>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Data::SqlTypes;
using namespace System::Reflection;
using namespace System::Resources;
using namespace System::Diagnostics;
using namespace Microsoft::Win32;


namespace BSL_USB_GUI {

  /// <summary>
  /// Summary for DownloadView
  ///
  /// WARNING: If you change the name of this class, you will need to change the
  ///          'Resource File Name' property for the managed resource compiler tool
  ///          associated with all .resx files this class depends on.  Otherwise,
  ///          the designers will not be able to interact properly with localized
  ///          resources associated with this form.
  /// </summary>
  public ref class DownloadView : public System::Windows::Forms::Form
  {
  public:
    DownloadView(void)
    {
		InitializeComponent();
		InitializeVidPidFirmware();

		// Construct Software version for title
		Assembly^ myAsm = Assembly::GetExecutingAssembly();
		AssemblyName^ assemName = myAsm->GetName();
		int majorRevision = assemName->Version->Major;
		int minorRevision = assemName->Version->Minor;
		int buildNumber = assemName->Version->Build;
		this->Text = this->Text + " " + majorRevision + "." + minorRevision + "." + buildNumber;

		InitializeBackgroundWorker();

		// Update recent files in menu.
		loadRecentFiles();
    }
  
  protected: static String^ myVID;
  protected: static String^ myPID;
  protected: static String^ myDeviceIndex = "0";
  protected: static DWORD numberOfUSBDevicesConnected = 0;
  protected: static bool scanDeviceAgain = false;
			 			
  protected: static String^ originHexFilePath = "";					// Store original hex file path
  protected: static String^ txtFileName = "";						// Store *.txt file name only
  protected: static String^ txtTIHex = "";							// Contains code data
  protected: static int errorCode = STATUS_SUCCESS;

  protected: static int testAddress = 0x00;
  protected: static int testLength = 0x00;
			 

#pragma region Windows Forms Tools

  private: System::Windows::Forms::MenuStrip^  menuMSP430;
  private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  newToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator;
  private: System::Windows::Forms::ToolStripMenuItem^  saveToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
  private: System::Windows::Forms::ToolStripMenuItem^  printPreviewToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
  private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  editToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  undoToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  redoToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
  private: System::Windows::Forms::ToolStripMenuItem^  cutToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  copyToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  pasteToolStripMenuItem;


  private: System::Windows::Forms::ToolStripMenuItem^  toolsToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  customizeToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  optionsToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator6;
  private: System::Windows::Forms::ToolStripMenuItem^  socketInfoToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  deviceInfoToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  contentsToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  indexToolStripMenuItem;

  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator5;
  private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
  private: System::Windows::Forms::ToolStrip^  toolBar;
  private: System::Windows::Forms::ToolStripSplitButton^  btnOpenRecentFiles;
  private: System::Windows::Forms::ToolStripButton^  btnReload;
  private: System::Windows::Forms::ToolStripButton^  btnSave;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator8;
  private: System::Windows::Forms::ToolStripButton^  btnHardwareSetting;
  private: System::Windows::Forms::ToolStripButton^  btnSoftwareSetting;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator7;
  private: System::Windows::Forms::ToolStripLabel^  toolStripLabel1;
  private: System::Windows::Forms::ToolStripButton^  btnProgram;
  private: System::Windows::Forms::ToolStripButton^  btnRead;
  private: System::Windows::Forms::ToolStripButton^  btnVerify;
  private: System::Windows::Forms::ToolStripButton^  btnEraseAll;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator9;
  private: System::Windows::Forms::ToolStripLabel^  toolStripLabel2;
  private: System::Windows::Forms::ToolStripButton^  btnHardwareTest;
  private: System::Windows::Forms::ToolStripButton^  btnDetectDevice;
  private: System::Windows::Forms::ToolStripButton^  btnSocket;
  private: System::Windows::Forms::ToolStripButton^  btnDeviceInfo;
  private: System::Windows::Forms::ToolStripComboBox^  cboMSP430Device;
  private: System::Windows::Forms::ToolStripComboBox^  cboMSP430Family;
  private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;
  private: System::Windows::Forms::RichTextBox^  textBoxHex;
  private: System::Windows::Forms::RichTextBox^  textBoxLog;
  private: System::Windows::Forms::StatusStrip^  statusBar;
  private: System::Windows::Forms::ToolStripProgressBar^  progresBar;
  private: System::ComponentModel::BackgroundWorker^  bgWorkerProgramTarget;
  private: System::Windows::Forms::OpenFileDialog^  openHexDialog;
  private: System::Windows::Forms::ContextMenuStrip^  menuMessageLog;
  private: System::Windows::Forms::ToolStripMenuItem^  menuMessageLogClear;
  private: System::Windows::Forms::ToolStripMenuItem^  menuMessageLogSave;
  private: System::Windows::Forms::ToolStripMenuItem^  menuMessageLogCopy;
private: System::Windows::Forms::SaveFileDialog^  saveLogFileDialog;


  private: System::Windows::Forms::ToolStripStatusLabel^  statusLabel;
  
#pragma endregion

  protected:
    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    ~DownloadView()
    {
      // Unregister USB devices before quiting program.
      UnRegisterUSBDevice();
      if (components)
      {
        delete components;
      }
    }

  private: BYTE RegisterUSBDevice(HWND hWnd);
  private: BYTE UnRegisterUSBDevice();
  private: void StringToChar(String^ str , BYTE * character);
    
  private: System::ComponentModel::BackgroundWorker^  backgroundWorkerCheckDevice;
  private: System::ComponentModel::IContainer^  components;

  private:
    /// <summary>
    /// Required designer variable.
    /// </summary>


#pragma region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    void InitializeComponent(void)
    {
		this->components = (gcnew System::ComponentModel::Container());
		System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(DownloadView::typeid));
		this->backgroundWorkerCheckDevice = (gcnew System::ComponentModel::BackgroundWorker());
		this->menuMSP430 = (gcnew System::Windows::Forms::MenuStrip());
		this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->newToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripSeparator = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->printPreviewToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->editToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->undoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->redoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->cutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->copyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->pasteToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->customizeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->optionsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripSeparator6 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->socketInfoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->deviceInfoToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->contentsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->indexToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolStripSeparator5 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->toolBar = (gcnew System::Windows::Forms::ToolStrip());
		this->btnOpenRecentFiles = (gcnew System::Windows::Forms::ToolStripSplitButton());
		this->btnReload = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnSave = (gcnew System::Windows::Forms::ToolStripButton());
		this->toolStripSeparator8 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->btnHardwareSetting = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnSoftwareSetting = (gcnew System::Windows::Forms::ToolStripButton());
		this->toolStripSeparator7 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->toolStripLabel1 = (gcnew System::Windows::Forms::ToolStripLabel());
		this->btnProgram = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnRead = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnVerify = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnEraseAll = (gcnew System::Windows::Forms::ToolStripButton());
		this->toolStripSeparator9 = (gcnew System::Windows::Forms::ToolStripSeparator());
		this->toolStripLabel2 = (gcnew System::Windows::Forms::ToolStripLabel());
		this->btnHardwareTest = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnDetectDevice = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnSocket = (gcnew System::Windows::Forms::ToolStripButton());
		this->btnDeviceInfo = (gcnew System::Windows::Forms::ToolStripButton());
		this->cboMSP430Device = (gcnew System::Windows::Forms::ToolStripComboBox());
		this->cboMSP430Family = (gcnew System::Windows::Forms::ToolStripComboBox());
		this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
		this->textBoxHex = (gcnew System::Windows::Forms::RichTextBox());
		this->textBoxLog = (gcnew System::Windows::Forms::RichTextBox());
		this->menuMessageLog = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
		this->menuMessageLogCopy = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->menuMessageLogClear = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->menuMessageLogSave = (gcnew System::Windows::Forms::ToolStripMenuItem());
		this->statusBar = (gcnew System::Windows::Forms::StatusStrip());
		this->progresBar = (gcnew System::Windows::Forms::ToolStripProgressBar());
		this->statusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
		this->bgWorkerProgramTarget = (gcnew System::ComponentModel::BackgroundWorker());
		this->openHexDialog = (gcnew System::Windows::Forms::OpenFileDialog());
		this->saveLogFileDialog = (gcnew System::Windows::Forms::SaveFileDialog());
		this->menuMSP430->SuspendLayout();
		this->toolBar->SuspendLayout();
		this->tableLayoutPanel1->SuspendLayout();
		this->menuMessageLog->SuspendLayout();
		this->statusBar->SuspendLayout();
		this->SuspendLayout();
		// 
		// menuMSP430
		// 
		this->menuMSP430->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->fileToolStripMenuItem, 
			this->editToolStripMenuItem, this->toolsToolStripMenuItem, this->helpToolStripMenuItem});
		this->menuMSP430->Location = System::Drawing::Point(0, 0);
		this->menuMSP430->Name = L"menuMSP430";
		this->menuMSP430->Size = System::Drawing::Size(744, 25);
		this->menuMSP430->TabIndex = 11;
		this->menuMSP430->Text = L"menuStrip1";
		// 
		// fileToolStripMenuItem
		// 
		this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(8) {this->newToolStripMenuItem, 
			this->openToolStripMenuItem, this->toolStripSeparator, this->saveToolStripMenuItem, this->toolStripSeparator1, this->printPreviewToolStripMenuItem, 
			this->toolStripSeparator2, this->exitToolStripMenuItem});
		this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
		this->fileToolStripMenuItem->Size = System::Drawing::Size(39, 21);
		this->fileToolStripMenuItem->Text = L"&File";
		// 
		// newToolStripMenuItem
		// 
		this->newToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"newToolStripMenuItem.Image")));
		this->newToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->newToolStripMenuItem->Name = L"newToolStripMenuItem";
		this->newToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::N));
		this->newToolStripMenuItem->Size = System::Drawing::Size(164, 22);
		this->newToolStripMenuItem->Text = L"&Open";
		// 
		// openToolStripMenuItem
		// 
		this->openToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"openToolStripMenuItem.Image")));
		this->openToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
		this->openToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
		this->openToolStripMenuItem->Size = System::Drawing::Size(164, 22);
		this->openToolStripMenuItem->Text = L"&Reload";
		// 
		// toolStripSeparator
		// 
		this->toolStripSeparator->Name = L"toolStripSeparator";
		this->toolStripSeparator->Size = System::Drawing::Size(161, 6);
		// 
		// saveToolStripMenuItem
		// 
		this->saveToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"saveToolStripMenuItem.Image")));
		this->saveToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
		this->saveToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
		this->saveToolStripMenuItem->Size = System::Drawing::Size(164, 22);
		this->saveToolStripMenuItem->Text = L"&Save";
		// 
		// toolStripSeparator1
		// 
		this->toolStripSeparator1->Name = L"toolStripSeparator1";
		this->toolStripSeparator1->Size = System::Drawing::Size(161, 6);
		// 
		// printPreviewToolStripMenuItem
		// 
		this->printPreviewToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"printPreviewToolStripMenuItem.Image")));
		this->printPreviewToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->printPreviewToolStripMenuItem->Name = L"printPreviewToolStripMenuItem";
		this->printPreviewToolStripMenuItem->Size = System::Drawing::Size(164, 22);
		this->printPreviewToolStripMenuItem->Text = L"Recent files";
		// 
		// toolStripSeparator2
		// 
		this->toolStripSeparator2->Name = L"toolStripSeparator2";
		this->toolStripSeparator2->Size = System::Drawing::Size(161, 6);
		// 
		// exitToolStripMenuItem
		// 
		this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
		this->exitToolStripMenuItem->Size = System::Drawing::Size(164, 22);
		this->exitToolStripMenuItem->Text = L"E&xit";
		// 
		// editToolStripMenuItem
		// 
		this->editToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->undoToolStripMenuItem, 
			this->redoToolStripMenuItem, this->toolStripSeparator3, this->cutToolStripMenuItem, this->copyToolStripMenuItem, this->pasteToolStripMenuItem});
		this->editToolStripMenuItem->Name = L"editToolStripMenuItem";
		this->editToolStripMenuItem->Size = System::Drawing::Size(52, 21);
		this->editToolStripMenuItem->Text = L"&Tools";
		// 
		// undoToolStripMenuItem
		// 
		this->undoToolStripMenuItem->Name = L"undoToolStripMenuItem";
		this->undoToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Z));
		this->undoToolStripMenuItem->Size = System::Drawing::Size(206, 22);
		this->undoToolStripMenuItem->Text = L"Program";
		// 
		// redoToolStripMenuItem
		// 
		this->redoToolStripMenuItem->Name = L"redoToolStripMenuItem";
		this->redoToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Y));
		this->redoToolStripMenuItem->Size = System::Drawing::Size(206, 22);
		this->redoToolStripMenuItem->Text = L"Erase";
		// 
		// toolStripSeparator3
		// 
		this->toolStripSeparator3->Name = L"toolStripSeparator3";
		this->toolStripSeparator3->Size = System::Drawing::Size(203, 6);
		// 
		// cutToolStripMenuItem
		// 
		this->cutToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"cutToolStripMenuItem.Image")));
		this->cutToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->cutToolStripMenuItem->Name = L"cutToolStripMenuItem";
		this->cutToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::X));
		this->cutToolStripMenuItem->Size = System::Drawing::Size(206, 22);
		this->cutToolStripMenuItem->Text = L"Detect Device";
		// 
		// copyToolStripMenuItem
		// 
		this->copyToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"copyToolStripMenuItem.Image")));
		this->copyToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
		this->copyToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::C));
		this->copyToolStripMenuItem->Size = System::Drawing::Size(206, 22);
		this->copyToolStripMenuItem->Text = L"Power Target";
		// 
		// pasteToolStripMenuItem
		// 
		this->pasteToolStripMenuItem->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pasteToolStripMenuItem.Image")));
		this->pasteToolStripMenuItem->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->pasteToolStripMenuItem->Name = L"pasteToolStripMenuItem";
		this->pasteToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::V));
		this->pasteToolStripMenuItem->Size = System::Drawing::Size(206, 22);
		this->pasteToolStripMenuItem->Text = L"Hardware Test";
		// 
		// toolsToolStripMenuItem
		// 
		this->toolsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->customizeToolStripMenuItem, 
			this->optionsToolStripMenuItem, this->toolStripSeparator6, this->socketInfoToolStripMenuItem, this->deviceInfoToolStripMenuItem});
		this->toolsToolStripMenuItem->Name = L"toolsToolStripMenuItem";
		this->toolsToolStripMenuItem->Size = System::Drawing::Size(66, 21);
		this->toolsToolStripMenuItem->Text = L"&Settings";
		// 
		// customizeToolStripMenuItem
		// 
		this->customizeToolStripMenuItem->Name = L"customizeToolStripMenuItem";
		this->customizeToolStripMenuItem->Size = System::Drawing::Size(140, 22);
		this->customizeToolStripMenuItem->Text = L"Hardware";
		// 
		// optionsToolStripMenuItem
		// 
		this->optionsToolStripMenuItem->Name = L"optionsToolStripMenuItem";
		this->optionsToolStripMenuItem->Size = System::Drawing::Size(140, 22);
		this->optionsToolStripMenuItem->Text = L"Software";
		// 
		// toolStripSeparator6
		// 
		this->toolStripSeparator6->Name = L"toolStripSeparator6";
		this->toolStripSeparator6->Size = System::Drawing::Size(137, 6);
		// 
		// socketInfoToolStripMenuItem
		// 
		this->socketInfoToolStripMenuItem->Name = L"socketInfoToolStripMenuItem";
		this->socketInfoToolStripMenuItem->Size = System::Drawing::Size(140, 22);
		this->socketInfoToolStripMenuItem->Text = L"Socket Info";
		// 
		// deviceInfoToolStripMenuItem
		// 
		this->deviceInfoToolStripMenuItem->Name = L"deviceInfoToolStripMenuItem";
		this->deviceInfoToolStripMenuItem->Size = System::Drawing::Size(140, 22);
		this->deviceInfoToolStripMenuItem->Text = L"Device Info";
		// 
		// helpToolStripMenuItem
		// 
		this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->contentsToolStripMenuItem, 
			this->indexToolStripMenuItem, this->toolStripSeparator5, this->aboutToolStripMenuItem});
		this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
		this->helpToolStripMenuItem->Size = System::Drawing::Size(47, 21);
		this->helpToolStripMenuItem->Text = L"&Help";
		// 
		// contentsToolStripMenuItem
		// 
		this->contentsToolStripMenuItem->Name = L"contentsToolStripMenuItem";
		this->contentsToolStripMenuItem->Size = System::Drawing::Size(182, 22);
		this->contentsToolStripMenuItem->Text = L"User\'s Guide";
		// 
		// indexToolStripMenuItem
		// 
		this->indexToolStripMenuItem->Name = L"indexToolStripMenuItem";
		this->indexToolStripMenuItem->Size = System::Drawing::Size(182, 22);
		this->indexToolStripMenuItem->Text = L"Check for updates";
		// 
		// toolStripSeparator5
		// 
		this->toolStripSeparator5->Name = L"toolStripSeparator5";
		this->toolStripSeparator5->Size = System::Drawing::Size(179, 6);
		// 
		// aboutToolStripMenuItem
		// 
		this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
		this->aboutToolStripMenuItem->Size = System::Drawing::Size(182, 22);
		this->aboutToolStripMenuItem->Text = L"&About...";
		// 
		// toolBar
		// 
		this->toolBar->AutoSize = false;
		this->toolBar->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(20) {this->btnOpenRecentFiles, 
			this->btnReload, this->btnSave, this->toolStripSeparator8, this->btnHardwareSetting, this->btnSoftwareSetting, this->toolStripSeparator7, 
			this->toolStripLabel1, this->btnProgram, this->btnRead, this->btnVerify, this->btnEraseAll, this->toolStripSeparator9, this->toolStripLabel2, 
			this->btnHardwareTest, this->btnDetectDevice, this->btnSocket, this->btnDeviceInfo, this->cboMSP430Device, this->cboMSP430Family});
		this->toolBar->Location = System::Drawing::Point(0, 25);
		this->toolBar->Name = L"toolBar";
		this->toolBar->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
		this->toolBar->Size = System::Drawing::Size(744, 30);
		this->toolBar->TabIndex = 12;
		this->toolBar->Text = L"toolStrip1";
		// 
		// btnOpenRecentFiles
		// 
		this->btnOpenRecentFiles->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnOpenRecentFiles->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnOpenRecentFiles.Image")));
		this->btnOpenRecentFiles->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnOpenRecentFiles->Name = L"btnOpenRecentFiles";
		this->btnOpenRecentFiles->Size = System::Drawing::Size(32, 27);
		this->btnOpenRecentFiles->Text = L"Open..";
		this->btnOpenRecentFiles->ButtonClick += gcnew System::EventHandler(this, &DownloadView::btnOpenRecentFiles_ButtonClick);
		this->btnOpenRecentFiles->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &DownloadView::btnOpenRecentFiles_DropDownItemClicked);
		// 
		// btnReload
		// 
		this->btnReload->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnReload->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnReload.Image")));
		this->btnReload->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnReload->Name = L"btnReload";
		this->btnReload->Size = System::Drawing::Size(23, 27);
		this->btnReload->Text = L"Reload..";
		this->btnReload->Click += gcnew System::EventHandler(this, &DownloadView::btnReload_Click);
		// 
		// btnSave
		// 
		this->btnSave->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnSave->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnSave.Image")));
		this->btnSave->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnSave->Name = L"btnSave";
		this->btnSave->Size = System::Drawing::Size(23, 27);
		this->btnSave->Text = L"Save";
		// 
		// toolStripSeparator8
		// 
		this->toolStripSeparator8->Name = L"toolStripSeparator8";
		this->toolStripSeparator8->Size = System::Drawing::Size(6, 30);
		// 
		// btnHardwareSetting
		// 
		this->btnHardwareSetting->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnHardwareSetting->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnHardwareSetting.Image")));
		this->btnHardwareSetting->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnHardwareSetting->Name = L"btnHardwareSetting";
		this->btnHardwareSetting->Size = System::Drawing::Size(23, 27);
		this->btnHardwareSetting->Text = L"Hardware";
		// 
		// btnSoftwareSetting
		// 
		this->btnSoftwareSetting->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnSoftwareSetting->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnSoftwareSetting.Image")));
		this->btnSoftwareSetting->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnSoftwareSetting->Name = L"btnSoftwareSetting";
		this->btnSoftwareSetting->Size = System::Drawing::Size(23, 27);
		this->btnSoftwareSetting->Text = L"Software";
		// 
		// toolStripSeparator7
		// 
		this->toolStripSeparator7->Name = L"toolStripSeparator7";
		this->toolStripSeparator7->Size = System::Drawing::Size(6, 30);
		// 
		// toolStripLabel1
		// 
		this->toolStripLabel1->Name = L"toolStripLabel1";
		this->toolStripLabel1->Size = System::Drawing::Size(12, 27);
		this->toolStripLabel1->Text = L" ";
		// 
		// btnProgram
		// 
		this->btnProgram->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnProgram->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnProgram.Image")));
		this->btnProgram->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnProgram->Name = L"btnProgram";
		this->btnProgram->Size = System::Drawing::Size(23, 27);
		this->btnProgram->Text = L"Program";
		this->btnProgram->Click += gcnew System::EventHandler(this, &DownloadView::btnProgram_Click);
		// 
		// btnRead
		// 
		this->btnRead->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnRead->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnRead.Image")));
		this->btnRead->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnRead->Name = L"btnRead";
		this->btnRead->Size = System::Drawing::Size(23, 27);
		this->btnRead->Text = L"Read";
		// 
		// btnVerify
		// 
		this->btnVerify->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnVerify->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnVerify.Image")));
		this->btnVerify->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnVerify->Name = L"btnVerify";
		this->btnVerify->Size = System::Drawing::Size(23, 27);
		this->btnVerify->Text = L"Verify";
		// 
		// btnEraseAll
		// 
		this->btnEraseAll->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnEraseAll->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnEraseAll.Image")));
		this->btnEraseAll->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnEraseAll->Name = L"btnEraseAll";
		this->btnEraseAll->Size = System::Drawing::Size(23, 27);
		this->btnEraseAll->Text = L"Erase All";
		this->btnEraseAll->Click += gcnew System::EventHandler(this, &DownloadView::btnEraseAll_Click);
		// 
		// toolStripSeparator9
		// 
		this->toolStripSeparator9->Name = L"toolStripSeparator9";
		this->toolStripSeparator9->Size = System::Drawing::Size(6, 30);
		// 
		// toolStripLabel2
		// 
		this->toolStripLabel2->Name = L"toolStripLabel2";
		this->toolStripLabel2->Size = System::Drawing::Size(12, 27);
		this->toolStripLabel2->Text = L" ";
		// 
		// btnHardwareTest
		// 
		this->btnHardwareTest->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnHardwareTest->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnHardwareTest.Image")));
		this->btnHardwareTest->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnHardwareTest->Name = L"btnHardwareTest";
		this->btnHardwareTest->Size = System::Drawing::Size(23, 27);
		this->btnHardwareTest->Text = L"Hardware Test";
		this->btnHardwareTest->Click += gcnew System::EventHandler(this, &DownloadView::btnHardwareTest_Click);
		// 
		// btnDetectDevice
		// 
		this->btnDetectDevice->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnDetectDevice->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnDetectDevice.Image")));
		this->btnDetectDevice->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnDetectDevice->Name = L"btnDetectDevice";
		this->btnDetectDevice->Size = System::Drawing::Size(23, 27);
		this->btnDetectDevice->Text = L"Detect Device";
		this->btnDetectDevice->Click += gcnew System::EventHandler(this, &DownloadView::btnDetectDevice_Click);
		// 
		// btnSocket
		// 
		this->btnSocket->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnSocket->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnSocket.Image")));
		this->btnSocket->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnSocket->Name = L"btnSocket";
		this->btnSocket->Size = System::Drawing::Size(23, 27);
		this->btnSocket->Text = L"Socket Connection";
		// 
		// btnDeviceInfo
		// 
		this->btnDeviceInfo->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Image;
		this->btnDeviceInfo->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btnDeviceInfo.Image")));
		this->btnDeviceInfo->ImageTransparentColor = System::Drawing::Color::Magenta;
		this->btnDeviceInfo->Name = L"btnDeviceInfo";
		this->btnDeviceInfo->Size = System::Drawing::Size(23, 27);
		this->btnDeviceInfo->Text = L"Device information";
		// 
		// cboMSP430Device
		// 
		this->cboMSP430Device->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
		this->cboMSP430Device->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
		this->cboMSP430Device->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
		this->cboMSP430Device->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		this->cboMSP430Device->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"MSP430G2553", L"MSP430G2231", L"MSP430F5510"});
		this->cboMSP430Device->MaxDropDownItems = 12;
		this->cboMSP430Device->Name = L"cboMSP430Device";
		this->cboMSP430Device->Size = System::Drawing::Size(118, 30);
		// 
		// cboMSP430Family
		// 
		this->cboMSP430Family->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
		this->cboMSP430Family->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::SuggestAppend;
		this->cboMSP430Family->AutoCompleteSource = System::Windows::Forms::AutoCompleteSource::ListItems;
		this->cboMSP430Family->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		this->cboMSP430Family->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"MSP430x1xx Family", L"MSP430x2xx Family", 
			L"MSP430x3xx Family", L"MSP430x4xx Family", L"MSP430x5xx Family", L"MSP430x6xx Family"});
		this->cboMSP430Family->Name = L"cboMSP430Family";
		this->cboMSP430Family->Size = System::Drawing::Size(138, 30);
		// 
		// tableLayoutPanel1
		// 
		this->tableLayoutPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
			| System::Windows::Forms::AnchorStyles::Left) 
			| System::Windows::Forms::AnchorStyles::Right));
		this->tableLayoutPanel1->ColumnCount = 2;
		this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
			57.5F)));
		this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
			42.5F)));
		this->tableLayoutPanel1->Controls->Add(this->textBoxHex, 0, 0);
		this->tableLayoutPanel1->Controls->Add(this->textBoxLog, 1, 0);
		this->tableLayoutPanel1->Location = System::Drawing::Point(12, 69);
		this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
		this->tableLayoutPanel1->RowCount = 1;
		this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
		this->tableLayoutPanel1->Size = System::Drawing::Size(720, 344);
		this->tableLayoutPanel1->TabIndex = 13;
		// 
		// textBoxHex
		// 
		this->textBoxHex->BackColor = System::Drawing::SystemColors::Control;
		this->textBoxHex->Dock = System::Windows::Forms::DockStyle::Fill;
		this->textBoxHex->Font = (gcnew System::Drawing::Font(L"Courier New", 9.267326F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->textBoxHex->Location = System::Drawing::Point(3, 3);
		this->textBoxHex->Name = L"textBoxHex";
		this->textBoxHex->Size = System::Drawing::Size(408, 338);
		this->textBoxHex->TabIndex = 0;
		this->textBoxHex->Text = L"";
		// 
		// textBoxLog
		// 
		this->textBoxLog->BackColor = System::Drawing::SystemColors::Control;
		this->textBoxLog->ContextMenuStrip = this->menuMessageLog;
		this->textBoxLog->Cursor = System::Windows::Forms::Cursors::Default;
		this->textBoxLog->Dock = System::Windows::Forms::DockStyle::Fill;
		this->textBoxLog->Font = (gcnew System::Drawing::Font(L"Courier New", 9.267326F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
			static_cast<System::Byte>(0)));
		this->textBoxLog->Location = System::Drawing::Point(417, 3);
		this->textBoxLog->Name = L"textBoxLog";
		this->textBoxLog->Size = System::Drawing::Size(300, 338);
		this->textBoxLog->TabIndex = 1;
		this->textBoxLog->Text = L"";
		// 
		// menuMessageLog
		// 
		this->menuMessageLog->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->menuMessageLogCopy, 
			this->menuMessageLogClear, this->menuMessageLogSave});
		this->menuMessageLog->Name = L"menuMessageLog";
		this->menuMessageLog->Size = System::Drawing::Size(150, 70);
		// 
		// menuMessageLogCopy
		// 
		this->menuMessageLogCopy->Name = L"menuMessageLogCopy";
		this->menuMessageLogCopy->Size = System::Drawing::Size(149, 22);
		this->menuMessageLogCopy->Text = L"Copy";
		this->menuMessageLogCopy->Click += gcnew System::EventHandler(this, &DownloadView::menuMessageLogCopy_Click);
		// 
		// menuMessageLogClear
		// 
		this->menuMessageLogClear->Name = L"menuMessageLogClear";
		this->menuMessageLogClear->Size = System::Drawing::Size(149, 22);
		this->menuMessageLogClear->Text = L"Clear log";
		this->menuMessageLogClear->Click += gcnew System::EventHandler(this, &DownloadView::menuMessageLogClear_Click);
		// 
		// menuMessageLogSave
		// 
		this->menuMessageLogSave->Name = L"menuMessageLogSave";
		this->menuMessageLogSave->Size = System::Drawing::Size(149, 22);
		this->menuMessageLogSave->Text = L"Save to file...";
		this->menuMessageLogSave->Click += gcnew System::EventHandler(this, &DownloadView::menuMessageLogSave_Click);
		// 
		// statusBar
		// 
		this->statusBar->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->progresBar, this->statusLabel});
		this->statusBar->Location = System::Drawing::Point(0, 416);
		this->statusBar->Name = L"statusBar";
		this->statusBar->Size = System::Drawing::Size(744, 22);
		this->statusBar->TabIndex = 14;
		this->statusBar->Text = L"statusStrip1";
		// 
		// progresBar
		// 
		this->progresBar->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
		this->progresBar->Name = L"progresBar";
		this->progresBar->RightToLeft = System::Windows::Forms::RightToLeft::No;
		this->progresBar->Size = System::Drawing::Size(150, 16);
		this->progresBar->Style = System::Windows::Forms::ProgressBarStyle::Continuous;
		// 
		// statusLabel
		// 
		this->statusLabel->Name = L"statusLabel";
		this->statusLabel->Size = System::Drawing::Size(136, 17);
		this->statusLabel->Text = L"Searching for device...";
		// 
		// bgWorkerProgramTarget
		// 
		this->bgWorkerProgramTarget->WorkerReportsProgress = true;
		this->bgWorkerProgramTarget->WorkerSupportsCancellation = true;
		this->bgWorkerProgramTarget->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &DownloadView::bgWorkerProgramTarget_DoWork);
		this->bgWorkerProgramTarget->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &DownloadView::bgWorkerProgramTarget_RunWorkerCompleted);
		this->bgWorkerProgramTarget->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &DownloadView::bgWorkerProgramTarget_ProgressChanged);
		// 
		// openHexDialog
		// 
		this->openHexDialog->Filter = L"TI-Hex files|*.txt; *.out;";
		this->openHexDialog->Title = L"Open TI-Hex file...";
		// 
		// saveLogFileDialog
		// 
		this->saveLogFileDialog->DefaultExt = L"*.txt";
		this->saveLogFileDialog->Filter = L"TI-Hex files|*.txt";
		this->saveLogFileDialog->Title = L"Save log to file...";
		// 
		// DownloadView
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->BackColor = System::Drawing::SystemColors::Control;
		this->ClientSize = System::Drawing::Size(744, 438);
		this->Controls->Add(this->statusBar);
		this->Controls->Add(this->tableLayoutPanel1);
		this->Controls->Add(this->toolBar);
		this->Controls->Add(this->menuMSP430);
		this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
		this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
		this->MaximizeBox = false;
		this->Name = L"DownloadView";
		this->Text = L"Vnelek430 - Programmer for MSP430";
		this->menuMSP430->ResumeLayout(false);
		this->menuMSP430->PerformLayout();
		this->toolBar->ResumeLayout(false);
		this->toolBar->PerformLayout();
		this->tableLayoutPanel1->ResumeLayout(false);
		this->menuMessageLog->ResumeLayout(false);
		this->statusBar->ResumeLayout(false);
		this->statusBar->PerformLayout();
		this->ResumeLayout(false);
		this->PerformLayout();

	}

    void InitializeVidPidFirmware(void)
    {
	  this->myVID = "04D8";
	  this->myPID = "000F";
    }

    void InitializeBackgroundWorker(void)
    {
		backgroundWorkerCheckDevice->DoWork += gcnew DoWorkEventHandler( this, &DownloadView::backgroundWorkerCheckDevice_DoWork);
		backgroundWorkerCheckDevice->RunWorkerCompleted += gcnew RunWorkerCompletedEventHandler( this, &DownloadView::backgroundWorkerCheckDevice_RunWorkerCompleted );
    }

#pragma endregion

	
/* =========================================================================
 * Function     : backgroundWorkerCheckDevice_DoWork
 * Description  : Event handler thread for Checking # of devices
 * Agruments    :
 *					sender		Not in used.
 *					e			Contains number of devices connected to computer.
 * Return       : None.
 * ========================================================================= */
void backgroundWorkerCheckDevice_DoWork( Object^ sender, DoWorkEventArgs^ e )
{
	// Scan # of USB HID devices based on VID and PID
	DWORD numOfDevices = BSL_NumOfUSBDevices(System::Convert::ToUInt16(myVID->Replace("0x",""),16),
										     System::Convert::ToUInt16(myPID->Replace("0x",""),16));

	// Set # of USB devices into results inside this thread (safely)
	e->Result = numOfDevices; 
}

/* =========================================================================
 * Function     : backgroundWorkerCheckDevice_RunWorkerCompleted
 * Description  : This event handler deals with the results of the
 *				  background operation for checking the number of USB devices.
 *				  Before exit this function, run background check device again.
 * Agruments    :
 *					sender		Not in used.
 *					e			Contains number of devices connected to computer,
 *								cancelled message or error message.
 * Return       : None.
 * ========================================================================= */
void backgroundWorkerCheckDevice_RunWorkerCompleted( Object^ /*sender*/, RunWorkerCompletedEventArgs^ e )
{
	if ( e->Error != nullptr )
	{

	}
	else if ( e->Cancelled )
	{

	}
	else
	{
		// Read passed value from backgroundWorkerCheckDevice thread
		numberOfUSBDevicesConnected = System::Convert::ToUInt32(e->Result->ToString());

		String^ structText;

		if(numberOfUSBDevicesConnected > 0)
		{
			structText = "Connected!";
			// Update label
			updateStatusText(structText, COLOR_GREEN);
		}
		else if(numberOfUSBDevicesConnected == 0)
		{
			structText = "Disconnected!";	  
			// Update label
			updateStatusText(structText, COLOR_RED);
		}
	}

	if(scanDeviceAgain)
	{
		scanDeviceAgain = false;
		backgroundWorkerCheckDevice->RunWorkerAsync();
	}
}

/* =========================================================================
 * Function     : bgWorkerProgramTarget_DoWork
 * Description  : This function process the work for writting 
 *				  data to the MSP430 Programmer.
 * Agruments    :
 *					sender		Not in used.
 *					e			
 * Return       : None.
 * ========================================================================= */
private: System::Void bgWorkerProgramTarget_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) 
		 {	
			String^ line;
			int usbIndex         = 0x00;
			int i, indexStart	 = 0x00;
			int indexEnd		 = 0x00;
			int progressIndex	 = 0x00;
			byte usbDataFrame[START_INDEX + TX_MAXLEN];
			BYTE receiveBuffer[USB_HID_BUFFER];
			
			// Progress is in starting point...
			bgWorkerProgramTarget->ReportProgress(0x00);

			while (true)
			{
				// Read a line of hex-txt
				indexEnd = txtTIHex->IndexOf('\n', indexStart);
				line = txtTIHex->Substring(indexStart, indexEnd - indexStart);
		        
				// Beginning of a new address
				if (line->Substring(0, 1) == "@")
				{
					// Is data available before? Send it!!!
					if (usbIndex > START_INDEX)
					{
						// Update LENGTH of sequence of data
						usbIndex = usbIndex - START_INDEX;

						usbDataFrame[1] = (byte)(usbIndex >> 8);
						usbDataFrame[2] = (byte)(usbIndex & 0xFF);

						// Transmit block of data to host
						if(USB_SendToTarget(usbDataFrame, receiveBuffer, CMD_WRITE) != STATUS_SUCCESS)
						{
							return;
						}

						// Update progress bar
						progressIndex += 10;
						bgWorkerProgramTarget->ReportProgress(progressIndex);
					}

					// Index of first byte of data sequence
					usbIndex = START_INDEX;

					// Start address of block
					usbDataFrame[3] = Convert::ToByte(line->Substring(1, 2), 16);   // High-byte
					usbDataFrame[4] = Convert::ToByte(line->Substring(3, 2), 16);   // Low-byte
				}
				else if (line == "q")
				{
					// Process when reach end of source code
					
					// Update length of sequence of data
					usbIndex = usbIndex - START_INDEX;
					usbDataFrame[1] = (byte)(usbIndex >> 8);
					usbDataFrame[2] = (byte)(usbIndex & 0xFF);

					// Transmit block of data to programmer
					if(USB_SendToTarget(usbDataFrame, receiveBuffer, CMD_WRITE_END) != STATUS_SUCCESS)
					{						
						return ;
					}
					
					//bgWorkerProgramTarget->ReportProgress(100);
					return;
				}
				else
				{
					// Remove last character: " "
					if (line->Substring(line->Length - 1, 1) == " ")
						line = line->Substring(0, line->Length - 1);

					// Parsing contents of a line to bytes
					for (i = 0; i < line->Length; i += 3)
					{
						// Continue update block of data
						usbDataFrame[usbIndex++] = Convert::ToByte(line->Substring(i, 2), 16);
					}

					// The index has exceeded block of data
					if (usbIndex >= (TX_MAXLEN + START_INDEX))
					{
						// Update length of sequence of data
						usbIndex = usbIndex - START_INDEX;
						usbDataFrame[1] = (byte)(usbIndex >> 8);
						usbDataFrame[2] = (byte)(usbIndex & 0xFF);

						// Transmit block of data to programmer
  						if(USB_SendToTarget(usbDataFrame, receiveBuffer, CMD_WRITE) != STATUS_SUCCESS)
						{
							return ;
						}

						// Update progress bar
						progressIndex += 10;
						bgWorkerProgramTarget->ReportProgress(progressIndex);

						// Start address of new block
						usbIndex = (usbDataFrame[3] << 8) + usbDataFrame[4] + usbIndex;
	
						usbDataFrame[3] = (byte)(usbIndex >> 8);		// High-byte
						usbDataFrame[4] = (byte)(usbIndex & 0xFF);		// Low-byte

						// Reset code sequence index
						usbIndex = START_INDEX;
					}
				}

				// Increase read-line index in TI-TXT
				indexStart = indexEnd + 1;
			}

			//// Complete!!!
			//bgWorkerProgramTarget->ReportProgress(100);
			//return;
		 }


/* =========================================================================
 * Function     : bgWorkerProgramTarget_ProgressChanged
 * Description  : This event handler orccur when progress of writting 
 *				  data to MSP430 programmer has changed.
 *				  Update working status on UI interface.
 * Agruments    :
 *					sender		Not in used.
 *					e			
 * Return       : None.
 * ========================================================================= */
private: System::Void bgWorkerProgramTarget_ProgressChanged(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e) 
		 {
			 // Update pecentage to Progres bar
			 progresBar->Value = e->ProgressPercentage;
			
			 // For debug only
			 //if(e->ProgressPercentage != 100)
			 {
				updateMessageLog("Sent sequence:", COLOR_GREEN);
				updateMessageLog("Address: " + testAddress.ToString("X4"), COLOR_BLACK);
				updateMessageLog("Length: " + testLength.ToString(), COLOR_BLACK);
			 }
		 }

/* =========================================================================
 * Function     : bgWorkerProgramTarget_RunWorkerCompleted
 * Description  : This event handler orrcur when the task writting to MSP430 
 *				  programmer has done.
 * Agruments    :
 *					sender		Not in used.
 *					e			
 * Return       : None.
 * ========================================================================= */
private: System::Void bgWorkerProgramTarget_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) 
{
	if ( e->Error != nullptr )
	{
	}
	else if ( e->Cancelled )
	{
	}
	else
	{
		// Enable "Program" button
		btnProgram->Enabled = true;

		// Close BSL connection
		BSL_close_BSL();

		// Display error message
		displayErrorMessage(errorCode, "Programming target done!");
	}
}

/* =========================================================================
*Function:    displayErrorMessage
*Description: 
*             
*Parameters: 
*             
*Returns:
*             void
* ========================================================================= */
void displayErrorMessage(int code, String^ successMessage)
{
	// Display error message
	if(code != STATUS_SUCCESS)
	{
		updateMessageLog("Error Code: 0x" + errorCode.ToString("X2"), COLOR_BROWN);

		switch(code)
		{
			case STATUS_CRC_FAILED:					
				updateMessageLog("CRC checking failed!", COLOR_RED);
				break;

			case STATUS_WRITE_FAILED:
				updateMessageLog("Trying write to target failed!", COLOR_RED);
				break;

			case STATUS_DETECT_FAILED:
				updateMessageLog("Detect device failed!", COLOR_RED);
				break;

			case STATUS_NOT_RESPONDING:
				updateMessageLog("The programmer is not responding!", COLOR_RED);
				break;

			default:
				updateMessageLog("Unknown error!", COLOR_RED);
				break;
		}
	}
	else
	{
		updateMessageLog(successMessage, COLOR_GREEN);
	}
}

/* =========================================================================
*Function:    OnHandleCreated
*Description: Function that overrides OnHandleCreated for the purpose of 
*             registering USB notification to windows
*Parameters: 
*             EventArgs^ e          Events arguments
*Returns:
*             void
* ========================================================================= */
protected: virtual void OnHandleCreated(System::EventArgs^ e) override
{
	// Runs the original constructor of OnHandleCreated
	Form::OnHandleCreated(e);

	// Register USB devices
	BYTE status = RegisterUSBDevice((HWND) Handle.ToPointer());

	if(status == 0x00)
	{
		// If registration is good, scan for # of USB devices
		backgroundWorkerCheckDevice->RunWorkerAsync();
	}
	else
	{
		updateMessageLog("Unable to register device!", COLOR_RED);
	}
}

/* =========================================================================
*Function:    WndProc
*Description: Overrides windows form processor function
*Parameters: 
*             Message% m            Takes windows message
*Returns:
*             void
 * ========================================================================= */
protected: virtual void WndProc(Message% m) override
{
  ParseMessages(m);
  Form::WndProc(m);	                        // pass message on to base form
}

/* =========================================================================
*Function:    ParseMessages
*Description: Parses message from Windows to identify if device was removed or
*             inserted
*Parameters: 
*             Message% m            Takes windows message
*Returns:
*             void
 * ========================================================================= */
private: void ParseMessages(Message% m)
{
	if (m.Msg == WM_DEVICECHANGE)	          // Received a device change message.
	{                                       // A USB device was either inserted or removed.
		switch (m.WParam.ToInt32())	          // Check the W parameter to see if a device was inserted or removed
		{
			case DBT_DEVICEARRIVAL:	            // Device was inserted
				if(!(backgroundWorkerCheckDevice->IsBusy))
				{
					// Run check number of devices thread
					backgroundWorkerCheckDevice->RunWorkerAsync(); 
				}
				else
				{
					scanDeviceAgain = true;
				}
				break;
			case DBT_DEVICEREMOVECOMPLETE:	    // Device was removed
				if(!(backgroundWorkerCheckDevice->IsBusy))
				{
					// Run check number of devices thread
					backgroundWorkerCheckDevice->RunWorkerAsync();
				}
				else
				{
					scanDeviceAgain = true;
				}
				break;
		}
	}
}


private: System::Void btnProgram_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			if(BSL_initialize_BSL() == BSL_DEVICE_NOT_FOUND)
			{
				// Update to message log
				updateMessageLog("Device initial failed!", COLOR_RED);
			}
			else
			{
				if (textBoxHex->Text->Length > 0)
				{
					// Disable "Program" button
					btnProgram->Enabled = false;

					// Copy hex data to global variable
					txtTIHex = textBoxHex->Text;

					// Start programming target device
					bgWorkerProgramTarget->RunWorkerAsync();
				}
				else
				{
					// Update message log
					updateMessageLog("Please! Choose your TI-Hex file!", COLOR_GREEN);
				}
			}
		 }

		 
		 /* =========================================================================
         * Function     : updateStatusText
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        void updateStatusText(String^ txt, int color)
        {			
			statusLabel->Text = txt;
			
            switch (color)
            {
                case COLOR_BLACK:
					statusLabel->ForeColor = Color::Black;
                    break;
                case COLOR_RED:                    
					statusLabel->ForeColor = Color::Red;
                    break;
                case COLOR_GREEN:                    
					statusLabel->ForeColor = Color::Green;
                    break;
                case COLOR_BROWN:                    
					statusLabel->ForeColor = Color::Brown;
                    break;
                default:
                    break;
            }
        }

		/* =========================================================================
         * Function     : updateMessageLog
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
        void updateMessageLog(String^ txt, int color)
        {			
			textBoxLog->SelectionStart = textBoxLog->TextLength;
			textBoxLog->SelectionLength = 0;
			
            switch (color)
            {
                case COLOR_BLACK:
					textBoxLog->SelectionColor = Color::Black;
                    break;
                case COLOR_RED:                    
					textBoxLog->SelectionColor = Color::Red;
                    break;
                case COLOR_GREEN:                    
					textBoxLog->SelectionColor = Color::Green;
                    break;
                case COLOR_BROWN:                    
					textBoxLog->SelectionColor = Color::Brown;
                    break;
                default:
                    break;
            }    
			textBoxLog->AppendText(txt + "\n");
			textBoxLog->SelectionColor = textBoxLog->ForeColor;
        }

		/* =========================================================================
         * Function     : storeRecentFiles
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
		void storeRecentFiles(String^ filePath)
		{
			int i, k;
			int NumOfRecentFiles;
			const int maxRecentFiles = 7;
			RegistryKey^ softwareKey;
			Object^ objValue;
			
			softwareKey = Registry::CurrentUser->CreateSubKey("Software\\Vnelek430");

			// Retrieve number of recent files
			NumOfRecentFiles = 0x00;
			objValue = softwareKey->GetValue("NumOfRecentFiles", RegistryValueKind::DWord);

			if(Convert::ToString(objValue) == "DWord")
			{
				NumOfRecentFiles = 0x00;
			}
			else
			{
				NumOfRecentFiles = Convert::ToInt32(objValue);
			}
			
			// Check if file path is exist?
			for( i = 1; i <= NumOfRecentFiles; i++)
			{
				objValue = softwareKey->GetValue("HexFiles" + i.ToString(), RegistryValueKind::String);

				String^ txt = Convert::ToString(objValue);

				// Exit if match path
				if(!txt->CompareTo(filePath))
				{
					// Close registry
					softwareKey->Close();
					return;
				}
			}

			// Reach maximum of files?
			if(NumOfRecentFiles >= maxRecentFiles)
			{
				for( i = 2; i <= maxRecentFiles; i++)
				{
					objValue = softwareKey->GetValue("HexFiles" + i.ToString(), RegistryValueKind::String);
					
					k = i -1;
					softwareKey->SetValue("HexFiles" + k.ToString(), Convert::ToString(objValue), RegistryValueKind::String);	
				}

				// Overwrite new file path
				softwareKey->SetValue("HexFiles" + maxRecentFiles.ToString(), filePath, RegistryValueKind::String);

				// Store new number of file in registry
				softwareKey->SetValue("NumOfRecentFiles", maxRecentFiles, RegistryValueKind::DWord);
			}
			else
			{
				// Store new file path
				NumOfRecentFiles++;
				softwareKey->SetValue("HexFiles" + NumOfRecentFiles.ToString(), filePath, RegistryValueKind::String);

				// Store new number of file in registry
				softwareKey->SetValue("NumOfRecentFiles", NumOfRecentFiles, RegistryValueKind::DWord);
			}

			// Close registry
			softwareKey->Close();
		}

		/* =========================================================================
         * Function     : loadRecentFiles
         * Description  : 
         * Agruments    :
         * Return       : None.
         * ========================================================================= */
		void loadRecentFiles(void)
		{
			int i;
			int NumOfRecentFiles;
			const int maxRecentFiles = 7;
			RegistryKey^ softwareKey;			
			Object^ objValue;
			
			softwareKey = Registry::CurrentUser->CreateSubKey("Software\\Vnelek430");

			// Retrieve number of recent files
			NumOfRecentFiles = 0x00;
			objValue = softwareKey->GetValue("NumOfRecentFiles", RegistryValueKind::DWord);

			if(Convert::ToString(objValue) == "DWord")
			{
				NumOfRecentFiles = 0x00;
			}
			else
			{
				NumOfRecentFiles = Convert::ToInt32(objValue);
			}
			
			// Clear all dropdown items
			btnOpenRecentFiles->DropDownItems->Clear();

			for( i = 1; i <= NumOfRecentFiles; i++)
			{
				objValue = softwareKey->GetValue("HexFiles" + i.ToString(), RegistryValueKind::String);
			
				// Add new recent files
				btnOpenRecentFiles->DropDownItems->Add(Convert::ToString(objValue));
			}

			// Close registry
			softwareKey->Close();
		}

		/* =========================================================================
         * Function     : USB_SendToTarget
         * Description  : 
         * Agruments    : 
         * Return       : None.
         * ========================================================================= */
        int USB_SendToTarget(byte data[], BYTE receiveBuffer[], byte command)
        {
            int length, address;
			unsigned int i;
			unsigned int numOfBytes;
			const int MAX_WAIT_TIMES = 10;
			
			errorCode = STATUS_SUCCESS;								// Initial status code
            data[0] = command;										// Assign command            
            length = (data[1] << 8) + data[2] + START_INDEX;		// Get length of sequence of data            
            address = (data[3] << 8) + data[4];						// Retrieve address

			// For debug only
			testAddress = address;
			testLength = length - START_INDEX;

			// Send data to device
			BSL_USB_SendData(data, length);

			for(i = 0; i < MAX_WAIT_TIMES; i++)
			{
				// Sleep & wait for data in 10ms...
				Thread::Sleep(10);

				// Read data from USB buffer
				numOfBytes = BSL_USB_ReceiveData(receiveBuffer);

				if(numOfBytes >= START_INDEX)
				{
					// Exit for loop
					break;
				}
			}

			// Clear receive buffer
			BSL_FlushBuffer();

			// There is no data. Return error!!!
			if(i == MAX_WAIT_TIMES)
			{
				errorCode = STATUS_NOT_RESPONDING;
				return STATUS_ERROR;
			}
			
			switch(receiveBuffer[0])
			{
				case STATUS_WRITE_FAILED:
					// The programmer is trying to write to target.
					// But, it's not working right.
					errorCode = STATUS_WRITE_FAILED;
					return STATUS_ERROR;
					break;

				case STATUS_CRC_FAILED:
					// Re-send data to device
					//BSL_USB_SendData(data, length);
					errorCode = STATUS_CRC_FAILED;
					return STATUS_ERROR;
					break;

				case STATUS_DETECT_FAILED:
					errorCode = STATUS_DETECT_FAILED;
					return STATUS_ERROR;
					break;

				case STATUS_ERASE_FAILED:
					errorCode = STATUS_ERASE_FAILED;
					return STATUS_ERROR;
					break;
				/*------------------------------------------------------------------------*/
				case GET_DEVICE_RAM_START:							// Return RAM's params
					data[0] = GET_DEVICE_RAM_START;
					data[1] = (BYTE)(Device_RamStart(globalDeviceIdx)>>8);
					data[2] = (BYTE)(Device_RamStart(globalDeviceIdx));
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_RAM_END:							// Return RAM's params
					data[0] = GET_DEVICE_RAM_END;
					data[1] = (BYTE)(Device_RamEnd(globalDeviceIdx)>>8);
					data[2] = (BYTE)(Device_RamEnd(globalDeviceIdx));
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_INFO_START:							// Return INFO's params
					data[0] = GET_DEVICE_INFO_START;
					data[1] = (BYTE)(Device_InfoStart(globalDeviceIdx)>>8);
					data[2] = (BYTE)(Device_InfoStart(globalDeviceIdx));
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_INFO_END:							// Return INFO's params
					data[0] = GET_DEVICE_INFO_END;
					data[1] = (BYTE)(Device_InfoEnd(globalDeviceIdx)>>8);
					data[2] = (BYTE)(Device_InfoEnd(globalDeviceIdx));
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_FLASH_START:						// Return FLASH's params
					data[0] = GET_DEVICE_FLASH_START;
					data[1] = (BYTE)(Device_FlashStart(globalDeviceIdx)>>8);
					data[2] = (BYTE)(Device_FlashStart(globalDeviceIdx));
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_FLASH_END:							// Return FLASH's params
					data[0] = GET_DEVICE_FLASH_END;
					data[1] = (BYTE)(Device_FlashEnd(globalDeviceIdx)>>8);
					data[2] = (BYTE)(Device_FlashEnd(globalDeviceIdx));
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_HAS_CPUX:							// Return CPU's params
					data[0] = GET_DEVICE_HAS_CPUX;
					data[1] = DeviceHas_CpuX(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_HAS_DATAQUICK:						// Return DATA QUICK's params
					data[0] = GET_DEVICE_HAS_DATAQUICK;
					data[1] = DeviceHas_DataQuick(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_ENH_VERIFY:							// Return EnhVerify params
					data[0] = GET_DEVICE_ENH_VERIFY;
					data[1] = DeviceHas_EnhVerify(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_FAST_FLASH:							// Return FAST FLASH's params
					data[0] = GET_DEVICE_FAST_FLASH;
					data[1] = DeviceHas_FastFlash(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_JTAG:								// Return JTAG's params
					data[0] = GET_DEVICE_JTAG;
					data[1] = DeviceHas_JTAG(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_SPYBIWIRE:							// Return SPYBIWIRE's params
					data[0] = GET_DEVICE_SPYBIWIRE;
					data[1] = DeviceHas_SpyBiWire(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				case GET_DEVICE_HAS_TESTPIN:						// Return TEST PIN's params
					data[0] = GET_DEVICE_HAS_TESTPIN;
					data[1] = DeviceHas_TestPin(globalDeviceIdx);
					BSL_USB_SendData(data, START_INDEX);
					break;

				default: break;
			}

			// Exit function
			return STATUS_SUCCESS;
        }


private: System::Void btnOpenRecentFiles_ButtonClick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(openHexDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 // Open TI-Hex file...
				 openHexFiles(openHexDialog->FileName);

				 // Update recent files in menu.
				 loadRecentFiles();
			 }
		 }


private: System::Void btnReload_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			if(originHexFilePath != "")
			{
				// Reload hex file...
				openHexFiles(originHexFilePath);
			}
			else
			{
				updateMessageLog("Please! Choose your TI-hex file first!", COLOR_GREEN);
			}
		 }

private: System::Void btnOpenRecentFiles_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) 
		 {
			// Reload hex file...
			openHexFiles(e->ClickedItem->Text);
		 }

		 /* =========================================================================
         * Function     : openHexFiles
         * Description  : 
         * Agruments    : 
         * Return       : None.
         * ========================================================================= */
		 void openHexFiles(String ^ fullFileName)
		 {
			int i;
			char charBuffer[512];
			LPTSTR lpChar;
			String ^ext, ^txtFilePath;
			String^ ExecutablePath;

			// Check if file is exist?
			if(IO::File::Exists(fullFileName) == false)
			{
				updateMessageLog("File not found!", COLOR_BROWN);
				updateMessageLog(fullFileName, COLOR_BLACK);
				return;
			}

			// Store in global variable
			originHexFilePath = fullFileName;

			// Store recent openned file in registry
			storeRecentFiles(fullFileName);

			// Get file name ONLY
			i = fullFileName->LastIndexOf('\\');
			txtFileName = fullFileName->Substring(i + 1, fullFileName->Length - i - 1);

			updateMessageLog("Opened hex file...", COLOR_GREEN);
			updateMessageLog(txtFileName, COLOR_BLACK);

			// Get temporary folder in Windows				
			lpChar = charBuffer;
			GetTempPath(512, lpChar);

			// Temporary folder + file.txt
			txtFilePath = gcnew String(lpChar) + txtFileName->Substring(0, txtFileName->Length-3) + "txt";

            // Get file extension
			i = txtFileName->LastIndexOf(".");
			ext = txtFileName->Substring(i + 1, txtFileName->Length - i - 1);
			
            String^ agrument = "--ti_txt " + "\"" + fullFileName + "\"" + " -o " + "\"" + txtFilePath + 
                               "\"" + " -order MS -romwidth 16";

			if (!ext->CompareTo("out"))
            {
                // Convert *.out -> *.txt
				ExecutablePath = System::Windows::Forms::Application::ExecutablePath;
				i = ExecutablePath->LastIndexOf("\\");
				ExecutablePath = ExecutablePath->Substring(0, i+1);

				ProcessStartInfo^ startInfo = gcnew ProcessStartInfo(ExecutablePath +"hex430.exe");
				startInfo->WindowStyle = ProcessWindowStyle::Hidden;
				startInfo->WorkingDirectory = ExecutablePath;					
				startInfo->Arguments = agrument;
				startInfo->UseShellExecute = false;
				Process::Start(startInfo);					
            }
            else
            {
                // Get *.txt path
                txtFilePath = fullFileName;
            }
			
            // Read file + Update textbox hex
			System::IO::StreamReader^ fHexFile = System::IO::File::OpenText(txtFilePath);
			textBoxHex->Text = fHexFile->ReadToEnd();
			fHexFile->Close();
		 }


private: System::Void btnEraseAll_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			byte usbData[64];
			BYTE receiveBuffer[USB_HID_BUFFER];
			int length;

			if(BSL_initialize_BSL() == BSL_DEVICE_NOT_FOUND)
			{
				// Update to message log
				updateMessageLog("Initialize to device failed!", COLOR_RED);
				return;
			}

			// Disable "Erase All" button
			btnEraseAll->Enabled = false;

			length = START_INDEX;
			usbData[1] = (length>>8);
			usbData[2] = (length & 0xFF);

			// Send erase target command to programmer
			USB_SendToTarget(usbData, receiveBuffer, CMD_ERASE);

			// Close BSL connection
			BSL_close_BSL();

			// Display error message
			displayErrorMessage(errorCode, "Erase target success!");

			// Enable "Erase All" button
			btnEraseAll->Enabled = true;
		 }

private: System::Void btnDetectDevice_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			byte usbData[64];
			BYTE receiveBuffer[USB_HID_BUFFER];
			int length;

			if(BSL_initialize_BSL() == BSL_DEVICE_NOT_FOUND)
			{
				// Update to message log
				updateMessageLog("Initialize to device failed!", COLOR_RED);
				return;
			}

			// Disable "Detect device" button
			btnDetectDevice->Enabled = false;

			length = START_INDEX;
			usbData[1] = (length>>8);
			usbData[2] = (length & 0xFF);

			// Send erase target command to programmer
			USB_SendToTarget(usbData, receiveBuffer, CMD_DETECT);

			// Close BSL connection
			BSL_close_BSL();

			// Display error message
			displayErrorMessage(errorCode, "Device detected:");

			if(receiveBuffer[0] == STATUS_DETECT_SUCCESS)
			{
				String^ txt = receiveBuffer[1].ToString("X2") + receiveBuffer[2].ToString("X2") + 
							  "-" + receiveBuffer[3].ToString("X2");

				updateMessageLog(txt, COLOR_BROWN);
			}

			// Enable "Erase All" button
			btnDetectDevice->Enabled = true;
		 }

private: System::Void btnHardwareTest_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 
		 }


private: System::Void menuMessageLogCopy_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 // Copy message log to clipboard
			 if(textBoxLog->Text != "")
			 {
				Clipboard::SetText(textBoxLog->Text);
			 }
			 else
			 {
				 Clipboard::Clear();
			 }
		 }

private: System::Void menuMessageLogClear_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 // Clear log textbox
			 textBoxLog->Text = "";
		 }

private: System::Void menuMessageLogSave_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(saveLogFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 // Save log to file...
			 }
		 }

		 //
}; // End of class DownloadView

}  // End of namespace