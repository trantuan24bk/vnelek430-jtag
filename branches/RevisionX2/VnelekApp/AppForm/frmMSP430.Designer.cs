namespace AppForm
{
    partial class frmMSP430
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMSP430));
            this.menuMSP430 = new System.Windows.Forms.MenuStrip();
            this.menuFile = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileOpen = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileReload = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.menuFileSave = new System.Windows.Forms.ToolStripMenuItem();
            this.menuFileSaveAs = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.menuFileRecent = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.menuFileExit = new System.Windows.Forms.ToolStripMenuItem();
            this.menuTools = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsProgram = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsErase = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.menuToolsRead = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsVerify = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsDetectDevice = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.menuToolsHardwareTest = new System.Windows.Forms.ToolStripMenuItem();
            this.menuToolsPowerDevice = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSettings = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSettingsHardware = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSettingsSoftware = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator6 = new System.Windows.Forms.ToolStripSeparator();
            this.menuSettingsConnection = new System.Windows.Forms.ToolStripMenuItem();
            this.menuSettingsDeviceInfo = new System.Windows.Forms.ToolStripMenuItem();
            this.menuHelp = new System.Windows.Forms.ToolStripMenuItem();
            this.menuHelpUserGuide = new System.Windows.Forms.ToolStripMenuItem();
            this.menuHelpUpdate = new System.Windows.Forms.ToolStripMenuItem();
            this.searchToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator5 = new System.Windows.Forms.ToolStripSeparator();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.statusBar = new System.Windows.Forms.StatusStrip();
            this.progresBar = new System.Windows.Forms.ToolStripProgressBar();
            this.statusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolBar = new System.Windows.Forms.ToolStrip();
            this.btnOpenRecentFiles = new System.Windows.Forms.ToolStripSplitButton();
            this.btnReload = new System.Windows.Forms.ToolStripButton();
            this.btnSave = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator8 = new System.Windows.Forms.ToolStripSeparator();
            this.btnHardwareSetting = new System.Windows.Forms.ToolStripButton();
            this.btnSoftwareSetting = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator7 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.btnProgram = new System.Windows.Forms.ToolStripButton();
            this.btnRead = new System.Windows.Forms.ToolStripButton();
            this.btnVerify = new System.Windows.Forms.ToolStripButton();
            this.btnEraseAll = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator9 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripLabel2 = new System.Windows.Forms.ToolStripLabel();
            this.btnHardwareTest = new System.Windows.Forms.ToolStripButton();
            this.btnDetectDevice = new System.Windows.Forms.ToolStripButton();
            this.btnSocket = new System.Windows.Forms.ToolStripButton();
            this.btnDeviceInfo = new System.Windows.Forms.ToolStripButton();
            this.cboMSP430Device = new System.Windows.Forms.ToolStripComboBox();
            this.cboMSP430Family = new System.Windows.Forms.ToolStripComboBox();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.textBoxHex = new System.Windows.Forms.RichTextBox();
            this.textBoxLog = new System.Windows.Forms.RichTextBox();
            this.menuMSP430.SuspendLayout();
            this.statusBar.SuspendLayout();
            this.toolBar.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuMSP430
            // 
            this.menuMSP430.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuMSP430.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuFile,
            this.menuTools,
            this.menuSettings,
            this.menuHelp});
            this.menuMSP430.Location = new System.Drawing.Point(0, 0);
            this.menuMSP430.Name = "menuMSP430";
            this.menuMSP430.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.menuMSP430.Size = new System.Drawing.Size(683, 25);
            this.menuMSP430.TabIndex = 0;
            this.menuMSP430.Text = "menuStrip1";
            // 
            // menuFile
            // 
            this.menuFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuFileOpen,
            this.menuFileReload,
            this.toolStripSeparator,
            this.menuFileSave,
            this.menuFileSaveAs,
            this.toolStripSeparator1,
            this.menuFileRecent,
            this.toolStripSeparator2,
            this.menuFileExit});
            this.menuFile.Name = "menuFile";
            this.menuFile.Size = new System.Drawing.Size(39, 21);
            this.menuFile.Text = "&File";
            // 
            // menuFileOpen
            // 
            this.menuFileOpen.Image = ((System.Drawing.Image)(resources.GetObject("menuFileOpen.Image")));
            this.menuFileOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.menuFileOpen.Name = "menuFileOpen";
            this.menuFileOpen.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.menuFileOpen.Size = new System.Drawing.Size(162, 22);
            this.menuFileOpen.Text = "&Open";
            // 
            // menuFileReload
            // 
            this.menuFileReload.Image = ((System.Drawing.Image)(resources.GetObject("menuFileReload.Image")));
            this.menuFileReload.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.menuFileReload.Name = "menuFileReload";
            this.menuFileReload.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.R)));
            this.menuFileReload.Size = new System.Drawing.Size(162, 22);
            this.menuFileReload.Text = "&Reload";
            // 
            // toolStripSeparator
            // 
            this.toolStripSeparator.Name = "toolStripSeparator";
            this.toolStripSeparator.Size = new System.Drawing.Size(159, 6);
            // 
            // menuFileSave
            // 
            this.menuFileSave.Image = ((System.Drawing.Image)(resources.GetObject("menuFileSave.Image")));
            this.menuFileSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.menuFileSave.Name = "menuFileSave";
            this.menuFileSave.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
            this.menuFileSave.Size = new System.Drawing.Size(162, 22);
            this.menuFileSave.Text = "&Save";
            // 
            // menuFileSaveAs
            // 
            this.menuFileSaveAs.Name = "menuFileSaveAs";
            this.menuFileSaveAs.Size = new System.Drawing.Size(162, 22);
            this.menuFileSaveAs.Text = "Save &As";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(159, 6);
            // 
            // menuFileRecent
            // 
            this.menuFileRecent.Image = ((System.Drawing.Image)(resources.GetObject("menuFileRecent.Image")));
            this.menuFileRecent.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.menuFileRecent.Name = "menuFileRecent";
            this.menuFileRecent.Size = new System.Drawing.Size(162, 22);
            this.menuFileRecent.Text = "Recent files";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(159, 6);
            // 
            // menuFileExit
            // 
            this.menuFileExit.Name = "menuFileExit";
            this.menuFileExit.Size = new System.Drawing.Size(162, 22);
            this.menuFileExit.Text = "E&xit";
            // 
            // menuTools
            // 
            this.menuTools.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuToolsProgram,
            this.menuToolsErase,
            this.toolStripSeparator3,
            this.menuToolsRead,
            this.menuToolsVerify,
            this.menuToolsDetectDevice,
            this.toolStripSeparator4,
            this.menuToolsHardwareTest,
            this.menuToolsPowerDevice});
            this.menuTools.Name = "menuTools";
            this.menuTools.Size = new System.Drawing.Size(52, 21);
            this.menuTools.Text = "&Tools";
            // 
            // menuToolsProgram
            // 
            this.menuToolsProgram.Name = "menuToolsProgram";
            this.menuToolsProgram.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.menuToolsProgram.Size = new System.Drawing.Size(171, 22);
            this.menuToolsProgram.Text = "&Program";
            // 
            // menuToolsErase
            // 
            this.menuToolsErase.Name = "menuToolsErase";
            this.menuToolsErase.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
            this.menuToolsErase.Size = new System.Drawing.Size(171, 22);
            this.menuToolsErase.Text = "&Erase";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(168, 6);
            // 
            // menuToolsRead
            // 
            this.menuToolsRead.Image = ((System.Drawing.Image)(resources.GetObject("menuToolsRead.Image")));
            this.menuToolsRead.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.menuToolsRead.Name = "menuToolsRead";
            this.menuToolsRead.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
            this.menuToolsRead.Size = new System.Drawing.Size(171, 22);
            this.menuToolsRead.Text = "&Read";
            // 
            // menuToolsVerify
            // 
            this.menuToolsVerify.Image = ((System.Drawing.Image)(resources.GetObject("menuToolsVerify.Image")));
            this.menuToolsVerify.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.menuToolsVerify.Name = "menuToolsVerify";
            this.menuToolsVerify.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
            this.menuToolsVerify.Size = new System.Drawing.Size(171, 22);
            this.menuToolsVerify.Text = "&Verify";
            // 
            // menuToolsDetectDevice
            // 
            this.menuToolsDetectDevice.Name = "menuToolsDetectDevice";
            this.menuToolsDetectDevice.Size = new System.Drawing.Size(171, 22);
            this.menuToolsDetectDevice.Text = "&Detect Device";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(168, 6);
            // 
            // menuToolsHardwareTest
            // 
            this.menuToolsHardwareTest.Name = "menuToolsHardwareTest";
            this.menuToolsHardwareTest.Size = new System.Drawing.Size(171, 22);
            this.menuToolsHardwareTest.Text = "Hardware Test";
            // 
            // menuToolsPowerDevice
            // 
            this.menuToolsPowerDevice.Name = "menuToolsPowerDevice";
            this.menuToolsPowerDevice.Size = new System.Drawing.Size(171, 22);
            this.menuToolsPowerDevice.Text = "Power Device";
            // 
            // menuSettings
            // 
            this.menuSettings.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuSettingsHardware,
            this.menuSettingsSoftware,
            this.toolStripSeparator6,
            this.menuSettingsConnection,
            this.menuSettingsDeviceInfo});
            this.menuSettings.Name = "menuSettings";
            this.menuSettings.Size = new System.Drawing.Size(66, 21);
            this.menuSettings.Text = "&Settings";
            // 
            // menuSettingsHardware
            // 
            this.menuSettingsHardware.Name = "menuSettingsHardware";
            this.menuSettingsHardware.Size = new System.Drawing.Size(183, 22);
            this.menuSettingsHardware.Text = "&Hardware";
            // 
            // menuSettingsSoftware
            // 
            this.menuSettingsSoftware.Name = "menuSettingsSoftware";
            this.menuSettingsSoftware.Size = new System.Drawing.Size(183, 22);
            this.menuSettingsSoftware.Text = "&Software";
            // 
            // toolStripSeparator6
            // 
            this.toolStripSeparator6.Name = "toolStripSeparator6";
            this.toolStripSeparator6.Size = new System.Drawing.Size(180, 6);
            // 
            // menuSettingsConnection
            // 
            this.menuSettingsConnection.Name = "menuSettingsConnection";
            this.menuSettingsConnection.Size = new System.Drawing.Size(183, 22);
            this.menuSettingsConnection.Text = "Socket &Connection";
            // 
            // menuSettingsDeviceInfo
            // 
            this.menuSettingsDeviceInfo.Name = "menuSettingsDeviceInfo";
            this.menuSettingsDeviceInfo.Size = new System.Drawing.Size(183, 22);
            this.menuSettingsDeviceInfo.Text = "&Device Infomation";
            // 
            // menuHelp
            // 
            this.menuHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuHelpUserGuide,
            this.menuHelpUpdate,
            this.searchToolStripMenuItem,
            this.toolStripSeparator5,
            this.aboutToolStripMenuItem});
            this.menuHelp.Name = "menuHelp";
            this.menuHelp.Size = new System.Drawing.Size(47, 21);
            this.menuHelp.Text = "&Help";
            // 
            // menuHelpUserGuide
            // 
            this.menuHelpUserGuide.Name = "menuHelpUserGuide";
            this.menuHelpUserGuide.Size = new System.Drawing.Size(185, 22);
            this.menuHelpUserGuide.Text = "&User\'s Guide";
            // 
            // menuHelpUpdate
            // 
            this.menuHelpUpdate.Name = "menuHelpUpdate";
            this.menuHelpUpdate.Size = new System.Drawing.Size(185, 22);
            this.menuHelpUpdate.Text = "&Check for update";
            // 
            // searchToolStripMenuItem
            // 
            this.searchToolStripMenuItem.Name = "searchToolStripMenuItem";
            this.searchToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.searchToolStripMenuItem.Text = "&Device Information";
            // 
            // toolStripSeparator5
            // 
            this.toolStripSeparator5.Name = "toolStripSeparator5";
            this.toolStripSeparator5.Size = new System.Drawing.Size(182, 6);
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(185, 22);
            this.aboutToolStripMenuItem.Text = "&About...";
            // 
            // statusBar
            // 
            this.statusBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.progresBar,
            this.statusLabel});
            this.statusBar.Location = new System.Drawing.Point(0, 398);
            this.statusBar.Name = "statusBar";
            this.statusBar.Size = new System.Drawing.Size(683, 22);
            this.statusBar.TabIndex = 1;
            this.statusBar.Text = "statusStrip1";
            // 
            // progresBar
            // 
            this.progresBar.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.progresBar.Name = "progresBar";
            this.progresBar.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.progresBar.Size = new System.Drawing.Size(150, 16);
            this.progresBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            // 
            // statusLabel
            // 
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(122, 17);
            this.statusLabel.Text = "Application started!";
            // 
            // toolBar
            // 
            this.toolBar.AutoSize = false;
            this.toolBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnOpenRecentFiles,
            this.btnReload,
            this.btnSave,
            this.toolStripSeparator8,
            this.btnHardwareSetting,
            this.btnSoftwareSetting,
            this.toolStripSeparator7,
            this.toolStripLabel1,
            this.btnProgram,
            this.btnRead,
            this.btnVerify,
            this.btnEraseAll,
            this.toolStripSeparator9,
            this.toolStripLabel2,
            this.btnHardwareTest,
            this.btnDetectDevice,
            this.btnSocket,
            this.btnDeviceInfo,
            this.cboMSP430Device,
            this.cboMSP430Family});
            this.toolBar.Location = new System.Drawing.Point(0, 25);
            this.toolBar.Name = "toolBar";
            this.toolBar.RenderMode = System.Windows.Forms.ToolStripRenderMode.Professional;
            this.toolBar.Size = new System.Drawing.Size(683, 30);
            this.toolBar.TabIndex = 2;
            this.toolBar.Text = "toolStrip1";
            // 
            // btnOpenRecentFiles
            // 
            this.btnOpenRecentFiles.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnOpenRecentFiles.Image = ((System.Drawing.Image)(resources.GetObject("btnOpenRecentFiles.Image")));
            this.btnOpenRecentFiles.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnOpenRecentFiles.Name = "btnOpenRecentFiles";
            this.btnOpenRecentFiles.Size = new System.Drawing.Size(32, 27);
            this.btnOpenRecentFiles.Text = "Open..";
            this.btnOpenRecentFiles.ButtonClick += new System.EventHandler(this.btnOpenRecentFiles_ButtonClick);
            // 
            // btnReload
            // 
            this.btnReload.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnReload.Image = ((System.Drawing.Image)(resources.GetObject("btnReload.Image")));
            this.btnReload.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnReload.Name = "btnReload";
            this.btnReload.Size = new System.Drawing.Size(23, 27);
            this.btnReload.Text = "Reload..";
            this.btnReload.Click += new System.EventHandler(this.btnReload_Click);
            // 
            // btnSave
            // 
            this.btnSave.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSave.Image = ((System.Drawing.Image)(resources.GetObject("btnSave.Image")));
            this.btnSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(23, 27);
            this.btnSave.Text = "Save";
            // 
            // toolStripSeparator8
            // 
            this.toolStripSeparator8.Name = "toolStripSeparator8";
            this.toolStripSeparator8.Size = new System.Drawing.Size(6, 30);
            // 
            // btnHardwareSetting
            // 
            this.btnHardwareSetting.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnHardwareSetting.Image = ((System.Drawing.Image)(resources.GetObject("btnHardwareSetting.Image")));
            this.btnHardwareSetting.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnHardwareSetting.Name = "btnHardwareSetting";
            this.btnHardwareSetting.Size = new System.Drawing.Size(23, 27);
            this.btnHardwareSetting.Text = "Hardware";
            // 
            // btnSoftwareSetting
            // 
            this.btnSoftwareSetting.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSoftwareSetting.Image = ((System.Drawing.Image)(resources.GetObject("btnSoftwareSetting.Image")));
            this.btnSoftwareSetting.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSoftwareSetting.Name = "btnSoftwareSetting";
            this.btnSoftwareSetting.Size = new System.Drawing.Size(23, 27);
            this.btnSoftwareSetting.Text = "Software";
            // 
            // toolStripSeparator7
            // 
            this.toolStripSeparator7.Name = "toolStripSeparator7";
            this.toolStripSeparator7.Size = new System.Drawing.Size(6, 30);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(12, 27);
            this.toolStripLabel1.Text = " ";
            // 
            // btnProgram
            // 
            this.btnProgram.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnProgram.Image = ((System.Drawing.Image)(resources.GetObject("btnProgram.Image")));
            this.btnProgram.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnProgram.Name = "btnProgram";
            this.btnProgram.Size = new System.Drawing.Size(23, 27);
            this.btnProgram.Text = "Program";
            this.btnProgram.Click += new System.EventHandler(this.btnProgram_Click);
            // 
            // btnRead
            // 
            this.btnRead.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnRead.Image = ((System.Drawing.Image)(resources.GetObject("btnRead.Image")));
            this.btnRead.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnRead.Name = "btnRead";
            this.btnRead.Size = new System.Drawing.Size(23, 27);
            this.btnRead.Text = "Read";
            // 
            // btnVerify
            // 
            this.btnVerify.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnVerify.Image = ((System.Drawing.Image)(resources.GetObject("btnVerify.Image")));
            this.btnVerify.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnVerify.Name = "btnVerify";
            this.btnVerify.Size = new System.Drawing.Size(23, 27);
            this.btnVerify.Text = "Verify";
            // 
            // btnEraseAll
            // 
            this.btnEraseAll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnEraseAll.Image = ((System.Drawing.Image)(resources.GetObject("btnEraseAll.Image")));
            this.btnEraseAll.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnEraseAll.Name = "btnEraseAll";
            this.btnEraseAll.Size = new System.Drawing.Size(23, 27);
            this.btnEraseAll.Text = "Erase All";
            this.btnEraseAll.Click += new System.EventHandler(this.btnEraseAll_Click);
            // 
            // toolStripSeparator9
            // 
            this.toolStripSeparator9.Name = "toolStripSeparator9";
            this.toolStripSeparator9.Size = new System.Drawing.Size(6, 30);
            // 
            // toolStripLabel2
            // 
            this.toolStripLabel2.Name = "toolStripLabel2";
            this.toolStripLabel2.Size = new System.Drawing.Size(12, 27);
            this.toolStripLabel2.Text = " ";
            // 
            // btnHardwareTest
            // 
            this.btnHardwareTest.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnHardwareTest.Image = ((System.Drawing.Image)(resources.GetObject("btnHardwareTest.Image")));
            this.btnHardwareTest.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnHardwareTest.Name = "btnHardwareTest";
            this.btnHardwareTest.Size = new System.Drawing.Size(23, 27);
            this.btnHardwareTest.Text = "Hardware Test";
            this.btnHardwareTest.Click += new System.EventHandler(this.btnHardwareTest_Click);
            // 
            // btnDetectDevice
            // 
            this.btnDetectDevice.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDetectDevice.Image = ((System.Drawing.Image)(resources.GetObject("btnDetectDevice.Image")));
            this.btnDetectDevice.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDetectDevice.Name = "btnDetectDevice";
            this.btnDetectDevice.Size = new System.Drawing.Size(23, 27);
            this.btnDetectDevice.Text = "Detect Device";
            this.btnDetectDevice.Click += new System.EventHandler(this.btnDetectDevice_Click);
            // 
            // btnSocket
            // 
            this.btnSocket.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnSocket.Image = ((System.Drawing.Image)(resources.GetObject("btnSocket.Image")));
            this.btnSocket.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnSocket.Name = "btnSocket";
            this.btnSocket.Size = new System.Drawing.Size(23, 27);
            this.btnSocket.Text = "Socket Connection";
            // 
            // btnDeviceInfo
            // 
            this.btnDeviceInfo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnDeviceInfo.Image = ((System.Drawing.Image)(resources.GetObject("btnDeviceInfo.Image")));
            this.btnDeviceInfo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDeviceInfo.Name = "btnDeviceInfo";
            this.btnDeviceInfo.Size = new System.Drawing.Size(23, 27);
            this.btnDeviceInfo.Text = "Device information";
            // 
            // cboMSP430Device
            // 
            this.cboMSP430Device.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.cboMSP430Device.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
            this.cboMSP430Device.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.cboMSP430Device.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.cboMSP430Device.Items.AddRange(new object[] {
            "MSP430G2553",
            "MSP430G2231",
            "MSP430F5510"});
            this.cboMSP430Device.MaxDropDownItems = 12;
            this.cboMSP430Device.Name = "cboMSP430Device";
            this.cboMSP430Device.Size = new System.Drawing.Size(118, 30);
            // 
            // cboMSP430Family
            // 
            this.cboMSP430Family.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.cboMSP430Family.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
            this.cboMSP430Family.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.ListItems;
            this.cboMSP430Family.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.cboMSP430Family.Items.AddRange(new object[] {
            "MSP430x1xx Family",
            "MSP430x2xx Family",
            "MSP430x3xx Family",
            "MSP430x4xx Family",
            "MSP430x5xx Family",
            "MSP430x6xx Family"});
            this.cboMSP430Family.Name = "cboMSP430Family";
            this.cboMSP430Family.Size = new System.Drawing.Size(138, 30);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 244F));
            this.tableLayoutPanel1.Controls.Add(this.textBoxHex, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.textBoxLog, 1, 0);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 63);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(678, 332);
            this.tableLayoutPanel1.TabIndex = 3;
            // 
            // textBoxHex
            // 
            this.textBoxHex.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBoxHex.Font = new System.Drawing.Font("Courier New", 9.980198F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxHex.Location = new System.Drawing.Point(3, 3);
            this.textBoxHex.Name = "textBoxHex";
            this.textBoxHex.ReadOnly = true;
            this.textBoxHex.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.textBoxHex.ShowSelectionMargin = true;
            this.textBoxHex.Size = new System.Drawing.Size(428, 326);
            this.textBoxHex.TabIndex = 0;
            this.textBoxHex.Text = "";
            // 
            // textBoxLog
            // 
            this.textBoxLog.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBoxLog.Font = new System.Drawing.Font("Consolas", 9.267326F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxLog.Location = new System.Drawing.Point(437, 3);
            this.textBoxLog.Name = "textBoxLog";
            this.textBoxLog.ReadOnly = true;
            this.textBoxLog.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.textBoxLog.Size = new System.Drawing.Size(238, 326);
            this.textBoxLog.TabIndex = 1;
            this.textBoxLog.Text = "";
            // 
            // frmMSP430
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(683, 420);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this.toolBar);
            this.Controls.Add(this.statusBar);
            this.Controls.Add(this.menuMSP430);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.Name = "frmMSP430";
            this.Text = "Vnelek430 - v1.0";
            this.menuMSP430.ResumeLayout(false);
            this.menuMSP430.PerformLayout();
            this.statusBar.ResumeLayout(false);
            this.statusBar.PerformLayout();
            this.toolBar.ResumeLayout(false);
            this.toolBar.PerformLayout();
            this.tableLayoutPanel1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuMSP430;
        private System.Windows.Forms.ToolStripMenuItem menuFile;
        private System.Windows.Forms.ToolStripMenuItem menuFileReload;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator;
        private System.Windows.Forms.ToolStripMenuItem menuFileSave;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem menuFileExit;
        private System.Windows.Forms.ToolStripMenuItem menuTools;
        private System.Windows.Forms.ToolStripMenuItem menuToolsProgram;
        private System.Windows.Forms.ToolStripMenuItem menuToolsErase;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem menuToolsRead;
        private System.Windows.Forms.ToolStripMenuItem menuToolsVerify;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem menuToolsHardwareTest;
        private System.Windows.Forms.ToolStripMenuItem menuSettings;
        private System.Windows.Forms.ToolStripMenuItem menuSettingsHardware;
        private System.Windows.Forms.ToolStripMenuItem menuSettingsSoftware;
        private System.Windows.Forms.ToolStripMenuItem menuHelp;
        private System.Windows.Forms.ToolStripMenuItem menuHelpUserGuide;
        private System.Windows.Forms.ToolStripMenuItem menuHelpUpdate;
        private System.Windows.Forms.ToolStripMenuItem searchToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator5;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.StatusStrip statusBar;
        private System.Windows.Forms.ToolStripProgressBar progresBar;
        private System.Windows.Forms.ToolStripStatusLabel statusLabel;
        private System.Windows.Forms.ToolStrip toolBar;
        private System.Windows.Forms.ToolStripSplitButton btnOpenRecentFiles;
        private System.Windows.Forms.ToolStripButton btnReload;
        private System.Windows.Forms.ToolStripButton btnSave;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator8;
        private System.Windows.Forms.ToolStripButton btnHardwareSetting;
        private System.Windows.Forms.ToolStripButton btnSoftwareSetting;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator7;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripButton btnProgram;
        private System.Windows.Forms.ToolStripButton btnRead;
        private System.Windows.Forms.ToolStripButton btnVerify;
        private System.Windows.Forms.ToolStripButton btnEraseAll;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator9;
        private System.Windows.Forms.ToolStripLabel toolStripLabel2;
        private System.Windows.Forms.ToolStripButton btnHardwareTest;
        private System.Windows.Forms.ToolStripButton btnDetectDevice;
        private System.Windows.Forms.ToolStripButton btnSocket;
        private System.Windows.Forms.ToolStripButton btnDeviceInfo;
        private System.Windows.Forms.ToolStripComboBox cboMSP430Family;
        private System.Windows.Forms.ToolStripComboBox cboMSP430Device;
        private System.Windows.Forms.ToolStripMenuItem menuFileSaveAs;
        private System.Windows.Forms.ToolStripMenuItem menuFileRecent;
        private System.Windows.Forms.ToolStripMenuItem menuFileOpen;
        private System.Windows.Forms.ToolStripMenuItem menuToolsPowerDevice;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.RichTextBox textBoxHex;
        private System.Windows.Forms.RichTextBox textBoxLog;
        private System.Windows.Forms.ToolStripMenuItem menuToolsDetectDevice;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator6;
        private System.Windows.Forms.ToolStripMenuItem menuSettingsConnection;
        private System.Windows.Forms.ToolStripMenuItem menuSettingsDeviceInfo;
    }
}

