/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * The mainboard must define a PNOT method to handle power
 * state notifications and Notify CPU device objects to
 * re-evaluate their _PPC and _CST tables.
 */

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, EC_SCI_GPI + 16) // GPE for Runtime SCI

	// EC RAM fields
	OperationRegion(ERAM, EmbeddedControl, 0, 0xFF)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
				//   [Configuration Space 0]
		    , 1,        //   Reserved bit[0]
		ENGA, 1,        //   Enable Global attention
		ENHY, 1,        //   Enable Hotkey function
		HFNE, 1,        //   Enable Sticky Fn Key
		DSEM, 1,        //   Disable embedded NumPad Emulation
		EN3R, 1,        //   Enable 3rd Host interface and TWR registers
		    , 1,        //   Reserved bit[6]
		ENTM, 1,        //   Enable Thermal monitoring

		Offset(0x01),   // [Configuration Space 1]
		ENBK, 1,        //   Enable Panel backlight on/ off synchronized with LID state
		    , 3,        //   Reserved bits[1:3]
		WPSW, 1,        //   Warning if the power switch is depressed
		    , 2,        //   Reserved bits[5:6]
		NTKY, 1,        //   do not ignore any key while Fn key is held down

		Offset(0x02),   // [Configuration Space 2]
		    , 5,        //   Reserved bits[0:4]
		SNLC, 1,        //   Smart NumLock Enable. 1:Enable 0:Disable
		    , 1,        //   Reserved bit[6]
		TNKB, 1,        //   ThinkVantage button function bit
		                //	   0: Scan code (Enter key)
		                //	   1: SMI/SCI attention

		Offset(0x03),   // [Configuration Space 3]
		DSHP, 1,        //   Disable to synchronize headphone state with the speaker mute
		IGPK, 1,        //   Ignore phantom keys
		    , 2,        //   Reserved bits[2:3]
		CBAT, 1,        //   Change discharge/charge preference to discharge primary battery first and charge secondary battery first if possible
		ADO0, 1,        //   Audio button behavior mode 0
		ADO1, 1,        //   Audio button behavior mode 1
		    , 1,        //   Enable to use PMH fan functions to get the fan speed

		Offset(0x04),   // [Sound Mask 0]
		    , 1,        //   reserved
		CLBA, 1,        //   critical low battery alarm
		LWBA, 1,        //   low battery alarm (YELLOW -> RED)
		SUBE, 1,        //   suspend beep
		PUBE, 1,        //   vm_suspend beep
		RSBE, 1,        //   resume beep
		DCBE, 1,        //   DC in/out beep
		PFBE, 1,        //   power off beep

		Offset(0x05),   // [Sound Mask 1]
		HSPA, 1,        //   power off alarm
		NHDD, 1,        //   no HDD alarm
		DEAD, 1,        //   dead
		B440, 1,        //   440 hz beep
		B315, 1,        //   315 hz beep
		T315, 1,        //   two 315 hz beep
		R315, 1,        //   three 315 hz beep
		BYAM, 1,        //   inhibit swap

		Offset(0x06),   // [Sound ID (Write only)]
		HSUN, 8,        //   Sound ID (Write Only)

		Offset(0x07),   // [Sound Repeat Interval (unit time 125ms)]
		HSRP, 8,        //   Sound Repeat Interval (Unit time : 125ms )

		Offset(0x08),   // [Sound Mask 2]
		Offset(0x09),   // [KBD ID]
		Offset(0x0A),   // [KMC command control]
		                // Offset 0xOB : reserved

		Offset(0x0C),   // [LED On/Off/ Blinking Control (Write only)]
		HLCL, 4,        //   0: power LED
		                //   1: battery status 0
		                //   2: battery status 1
		                //   3: additional Bay LED (Venice) / reserved (Toronto-4) / Slicer LED (Tokyo)
		                //   4-6: reserved
		                //   7: suspend LED
		                //   8: dock LED 1
		                //   9: dock LED 2
		                //   10-13: reserved
		                //   14: microphone mute
		                //   15: reserved
		    , 1,        //   Reserved bit[4]
		    , 1,        //   Blink gradually
		BLIK, 1,        //   Blinking
		TONF, 1,        //   Turn on/off

		Offset(0x0D),   // [Peripheral control 3]
		UONE, 1,        //   Bit0   - Enable to supply power through USB in S3, S4 and S5 state.
		    , 1,        //   Reserved bit[1]
		    , 1,        //   set USB_AO_SEL0 signal in S3, S4, and S5 state
		    , 1,        //   set USB_AO_SEL1 signal in S3, S4, and S5 state
		    , 4,        //   Reserved bits[4:7]

		Offset(0x0E),   // [Peripheral Status 4]
		HFNS, 2,        //   Bit[1, 0] : Fn Key Status
		                //      [0, 0] ... Unlock
		                //      [0, 1] ... Sticky
		                //      [1, 0] ... Lock
		                //      [1, 1] ... Reserved
		GSER, 1,        //   G-Sensor Diag Error. 1:Error / 0: No error
		PSCS, 1,        //   Inhibit charging
		    , 1,        //   External power saving mode
		GSUD, 1,        //   G-Sensor under Diag. 1: Diag is running/0: Diag comp(not running)
		GSID, 2,        //   Bit[7,6] G-Sensor type ID
		                //      [0,0] ... None
		                //      [0,1] ... STMicro
		                //      [1,0] ... Memsic
		                //      [1,1] ... ADI

		Offset(0x0F),   // [Peripheral status 5 (read only)]
		    , 4,        //   Reserved bits[0:3]
		HDSU, 1,        //   HDD was detached in suspend (this bit is cleared when the system enters into suspend)
		BYSU, 1,        //   Bay HDD was detached in suspend (this bit is cleared when the system enters into suspend)
		    , 1,        //   NumLock state of the embedded keyboard
		TMOD, 1,        //   Tablet mode switch

		Offset(0x10),   // [Attention Mask (00-127)]
		HAM0, 8,        // 10 : Attention Mask (00-07)
		HAM1, 8,        // 11 : Attention Mask (08-0F)
		HAM2, 8,        // 12 : Attention Mask (10-17)
		HAM3, 8,        // 13 : Attention Mask (18-1F)
		HAM4, 8,        // 14 : Attention Mask (20-27)
		HAM5, 8,        // 15 : Attention Mask (28-2F)
		HAM6, 8,        // 16 : Attention Mask (30-37)
		HAM7, 8,        // 17 : Attention Mask (38-3F)
		HAM8, 8,        // 18 : Attention Mask (40-47)
		HAM9, 8,        // 19 : Attention Mask (48-4F)
		HAMA, 8,        // 1A : Attention Mask (50-57)
		HAMB, 8,        // 1B : Attention Mask (58-5F)
		HAMC, 8,        // 1C : Attention Mask (60-67)
		HAMD, 8,        // 1D : Attention Mask (68-6F)
		HAME, 8,        // 1E : Attention Mask (70-77)
		HAMF, 8,        // 1F : Attention Mask (78-7F)

		                // Offset 0x20 : reserved
		Offset(0x21),   // [Inhibit Charge timer byte High (00h)] (timer is kicked when high byte is written)

		Offset(0x23),   // [Misc. control]
		                //   01h Reset charge inhibit
		                //   02h Inhibit to charge
		                //   03h Reset external power saving mode
		                //   04h Set to external power saving mode
		                //   09h Emulate pressing an eject button of optical device in the bay
		                //   0Ah Reset thermal state to control the fan
		                //   10h Start gravity sensor diagnostic program

		                // Offset 24-26 : reserved
		Offset(0x27),   // [Passward Scan Code]
		                // Offset 28-29 : reserved

		Offset(0x2A),   // [Attention Request]
		HATR, 8,        // 2A : Attention request

		Offset(0x2B),   // [Trip point of battery capacity]
		HT0H, 8,        // 2B : MSB of Trip Point Capacity for Battery 0
		HT0L, 8,        // 2C : LSB of Trip Point Capacity for Battery 0
		HT1H, 8,        // 2D : MSB of Trip Point Capacity for Battery 1
		HT1L, 8,        // 2E : LSB of Trip Point Capacity for Battery 1

		Offset(0x2F),   // [Fan Speed Control]
		HFSP, 8,        //  bit 2-0: speed (0: stop, 7:highest speed)
		                //  bit 5-3: reserved (should be 0)
		                //  bit 6: max. speed
		                //  bit 7: Automatic mode (fan speed controlled by thermal level)

		Offset(0x30),   // [Audio mute control]
		    , 7,        //  Reserved bits[0:6]
		SMUT, 1,        //  Mute

		Offset(0x31),   // [Peripheral Control 2]
		FANS, 2,        //   bit 0,1 Fan selector
		                //	   00: Fan 1, 01: Fan 2
		HUWB, 1,        //   UWB on
		ENS4, 1,        //   Reserved bit[3]
		DSEX, 1,        //   Disable Express Card
		AYID, 1,        //   Always-on Card identified
		    , 1,        //   Select USB to Always On card
		    , 1,        //   Assert the express card slot power control standby signal

		Offset(0x32),   // [EC Event Mask 0]
		HWPM, 1,        //   PME : Not used. PME# is connected to GPE directly.
		HWLB, 1,        //   Critical Low Bat
		HWLO, 1,        //   Lid Open
		    , 1,        //   Eject button
		HWFN, 1,        //   FN key
		    , 1,        //   Portfino wake up
		HWRI, 1,        //   Ring Indicator (UART)
		HWBU, 1,        //   Bay Unlock

		Offset(0x33),   // [EC Event Mask 1]

		Offset(0x34),   // [Peripheral status 2 (read only)]
		    , 1,        //   Reserved bit[0]
		    , 1,        //   Beep is being sounded now
		    , 1,        //   SMBus is busy
		    , 1,        //   Reserved bit[3]
		    , 1,        //   Fan exists
		    , 1,        //   Gravity sensors exist
		    , 1,        //   Reserved bit[6]
		HPLO, 1,        //   Power consumption warning

		Offset(0x35),   // [Peripheral status 3 (Read only)]
		    , 1,        //   Input devices (keyboard and mouse) are locked by password
		    , 1,        //   Input devices are frozen (input from devices are inhibited)
		    , 1,        //   Fan power on Reset is done
		    , 1,        //   Attention is disabled temporarily
		    , 1,        //   Fan error
		    , 2,        //   Reserved bit[5:6]
		    , 1,        //   Thermal sensor error

		Offset(0x36),   // [Copy register of EC Event Status (0x32)]
		Offset(0x37),   // [Copy register of EC Event Status (0x33)]

		Offset(0x38),   // [Battery 0 status (read only)]
		HB0S, 7,        //   bit 3-0 level
		                //     F: Unknown
		                //     2-n: battery level
		                //     1: low level
		                //     0: (critical low battery, suspend/ hibernate)
		                //   bit 4 error
		                //   bit 5 charge
		                //   bit 6 discharge
		MBTS, 1,        //   bit 7 battery attached

		Offset(0x39),   // [Battery 1 status (read only)]
		                //    bit definition is the same as offset(0x38)

		Offset(0x3A),   // [Peripheral control 0]
		MUTE, 1,        //   Mute
		I2CS, 1,        //   I2C select ( 0:Dock EEPROM etc, 1:Thermal sensor etc )(Tokyo-2)
		PWRF, 1,        //   Power off
		WANO, 1,        //   H/W Override bit
		                //	(enable to control wireless devices even if the global WAN disable switch is ON)
		DCBD, 1,        //   Bluetooth On
		DCWL, 1,        //   Wireless Lan On
		DCWW, 1,        //   Wireless Wan On
		    , 1,        //   2nd Battery Inhibit (Tokyo)

		Offset(0x3B),   // [Peripheral control 1]
		SPKM, 1,        //   Speaker Mute
		KBLH, 1,        //   Keyboard Light
		    , 1,        //   Reserved bit[2]
		BTDH, 1,        //   Bluetooth detach
		USBN, 1,        //   USB On
		    , 1,        //   Inhibit communication with battery 0
		    , 1,        //   Inhibit communication with battery 1
		S3FG, 1,        //   Reserved bit[7]

		Offset(0x3C),   // [Resume reason (Read only)]
		Offset(0x3D),   // [Password Control byte]
		Offset(0x3E),   // [Password data (8 byte)~ offset:45h]

		Offset(0x46),   // [sense status 0]
		FNKY, 1,        //   Fn key
		    , 1,        //   Reserved bit[1]
		HPLD, 1,        //   LID open
		PROF, 1,        //   Power off
		ACPW, 1,        //   External power (AC status)
		    , 2,        //   Reserved bits[5:6]
		CALR, 1,        //   LP mode (power consumption alert)

		Offset(0x47),   // [sense status 1]
		HPBU, 1,        //   Bay Unlock
		DKEV, 1,        //   Dock event
		BYNO, 1,        //   Bay is not Attached
		HDIB, 1,        //   HDD in the bay
		    , 4,        //   Reserved bits[4:7]

		Offset(0x48),   // [sense status 2]
		HPHI, 1,        //   Head Phone In
		GSTS, 1,        //   Global Wan Enable Switch
		    , 2,        //   Reserved bits[2:3]
		EXGC, 1,        //   External Graphic Chip
		DOKI, 1,        //   Dock attached
		HDDT, 1,        //   HDD detect
		    , 1,        //   Reserved bit[7]

		Offset(0x49),   // [sense status 3]
		                // Offset 0x4A : reserved

		Offset(0x4C),   // [MSB of Event Timer]
		ETHB, 8,        //   bit[14:0]=timer counter, bit[15], 0:ms, 1:sec

		Offset(0x4D),   // [LSB of Event Timer]
		ETLB, 8,        //

		Offset(0x4E),   // [EC Event Status 0]
		Offset(0x4F),   // [EC Event Status 1]

		Offset(0x50),   // [SMB_PRTCL (protocol register)]
		SMPR, 8,        //	00: Controller Not In use
		                //	01: reserved
		                //	02: Write Quick command
		                //	03: Read Quick command
		                //	04: Send Quick command
		                //	05: Receive Byte
		                //	06: Write Byte
		                //	07: Read Byte
		                //	08: Write Word
		                //	09: Read Word
		                //	0A: Write block

		Offset(0x51),   // [SMB_STS (status register)]
		SMST, 8,        //    bits[0:4] Status
		                //    bit[5] Reserved
		                //    bit[6] Alarm Received
		                //    bit[7] Done

		Offset(0x52),   // [SMB_ADDR (address register)]
		SMAD, 8,        //

		Offset(0x53),   // [SMB_CMD (Command register)]
		SMCM, 8,        //

		Offset(0x54),   // [SMB_DATA (Data Register (32 bytes))~ offset:73h]
		SMD0, 100,      //

		Offset(0x74),   // [SMB_BCNT (Block count register)]
		BCNT, 8,        //

		Offset(0x75),   // [SMB_ALRM_ADDR (Alarm address register)]
		SMAA, 8,        //

		Offset(0x76),   // [SMB_ALRM_DATA (Alarm data register (2 bytes))]
		BATD, 16,       //

		Offset(0x78),   // [Temperature of thermal sensor 0 (centigrade)]
		TMP0, 8,        // 78 : Temperature of thermal sensor 0
		TMP1, 8,        // 79 : Temperature of thermal sensor 1
		TMP2, 8,        // 7A : Temperature of thermal sensor 2
		TMP3, 8,        // 7B : Temperature of thermal sensor 3
		TMP4, 8,        // 7C : Temperature of thermal sensor 4
		TMP5, 8,        // 7D : Temperature of thermal sensor 5
		TMP6, 8,        // 7E : Temperature of thermal sensor 6
		TMP7, 8,        // 7F : Temperature of thermal sensor 7

		                // Offset 79-7F : reserved
		Offset(0x80),   // [Attention control byte]

		Offset(0x81),   // [Battery information ID for 0xA0-0xAF]
		HIID, 8,        //   (this byte is depend on the interface, 62&66 and 1600&1604)

		Offset(0x82),   // [Fn Dual function - make time out time (100ms unit)]

		Offset(0x83),   // [Fn Dual function ID]
		HFNI, 8,        //	0: none
		                //	1-3: Reserved
		                //	4: ACPI Power
		                //	5: ACPI Sleep
		                //	6: ACPI Wake
		                //  7: Left Ctrl key

		Offset(0x84),   // [Fan Speed]
		    , 16,       //
		                //	(I/F Offset 3Bh bit5 => 0:Main Fan , 1:Second Fan)

		Offset(0x86),   // [password 0 - 7 status]
		Offset(0x87),   // [password 8 - 15 status]
		Offset(0x88),   // [Thermal Status of Level 0 (low)]
		Offset(0x89),   // [Thermal Status of Level 1 (middle)]
		Offset(0x8A),   // [Thermal Status of Level 2 (middle high)]
		Offset(0x8B),   // [Thermal Status of Level 3 (high)]
		                // Offset 0x8C : reserved

		Offset(0x8D),   // [Interval of polling Always-on cards in half minute]
		HDAA, 3,        //   Warning Delay Period
		HDAB, 3,        //   Stolen Delay Period
		HDAC, 2,        //   Sensitivity

		Offset(0x8E),   // [Key-number assigned to the ThinkVantage button]
		Offset(0x8F),   // [EC Internal Use for Fan Duty Table Creation]
		Offset(0x90),   // [EC internal use]
		Offset(0xA0),   // [Battery Information Area]~ offset:0AFh
		Offset(0xB0),   // [Battery 0 charge start capacity]
		Offset(0xB1),   // [Battery 0 charge stop capacity]
		Offset(0xB2),   // [Battery 1 charge start capacity]
		Offset(0xB3),   // [Battery 1 charge stop capacity]

		Offset(0xB4),   // [Battery 0 control]
		                //    01h Stop refreshing the battery
		                //    02h Refresh the battery
		                //    07h Set long life span mode
		                //    08h Set long run time mode

		Offset(0xB5),   // [Battery 1 control]
		                // Offset B6-C7 : reserved

		Offset(0xC8),   // [Adaptive Thermal Management (ATM)]
		ATMX, 8,        //  bit 7-4 - Thermal Table & bit 3-0 - Fan Speed Table

		Offset(0xC9),   // [Wattage of AC/DC]
		AC65, 8,        //

		Offset(0xCA),   // Reserved - but should be 0

		Offset(0xCB),   // [ATM configuration]
		BFUD, 1,        //   bit 0 - Battery FUD Flag
		    , 7,        //   bit 1~7 - Reserved

		Offset(0xCC),   //
		PWMH, 8,        // CC : AC Power Consumption (MSB)
		PWML, 8,        // CD : AC Power Consumption (LSB) - unit: 100mW

		Offset(0xCE),   // [Configuration Space 4]
		    , 2,        //   Windows key mode
		    , 2,        //   Application key mode
		    , 1,        //   Swap the Fn key and the left Ctrl key
		    , 3,        //   Reserved bits[5:7]

		Offset(0xCF),   // [Configuration Space 5]
		HSID, 8,        //   Hand shaking byte of system information ID

		Offset(0xD0),   // [EC Type]
		                // D1-DF : reserved

		Offset(0xE0),   // @@ Mapping to old EC RAM for battery information
		ECRC, 16,       // BAT1 Sys command [0] RC
		ECAC, 16,       // BAT1 Sys command [0] AV_I
		ECVO, 16,       // BAT1 Sys command [0] Voltage

		                // E1-E7 : reserved
		Offset(0xE8),   // [Version 0]
		Offset(0xE9),   // [Version 1]
		Offset(0xEA),   // [Machine ID]
		Offset(0xEB),   // [Function Specification Minor Version]
		Offset(0xEC),   // [EC capability 0]
		Offset(0xED),   // [EC capability 1]

		Offset(0xEE),   // [Highest battery level]
		MBTH, 4,        //   bit 3-0: battery 0 highest level
		SBTH, 4,        //   bit 7-4: battery 1 highest level
		                //   note: if highest level is 0 or 0xF, it means not defined
		                //         (in this case, use default highest level, it is 6)

		Offset(0xEF),   // [EC Function Specification Major Version]
		Offset(0xF0),   // [Build ID]~ offset:0F7h

		Offset(0xF8),   // [Build Date (F8: MSB, F9:LSB)]
		    , 4,        //   bit 3-0: day(1-31)
		    , 4,        //   bit 7-4: month(1-12)
		    , 8,        //   bit 15-8: year(2000 base)

		Offset(0xFA),   // [Build Time (in 2seconds)]
		                //   ex: when index FAh=5Ah and index FBh=ADh, Build Time is 5AADh .. 12:53:46

		Offset(0xFC),   // [reserved]~ offset:0FFh
	} // End of ERAM

	//
	// Battery Information ID : 00/10
	//
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		SBRC, 16,    // Remaining Capacity
		SBFC, 16,    // Fully Charged Capacity
		SBAE, 16,    // Average Time To Empty
		SBRS, 16,    // Relative State Of Charge
		SBAC, 16,    // Average Current
		SBVO, 16,    // Voltage
		SBAF, 16,    // Average Time To Full
		SBBS, 16,    // Battery State
	}

	//
	// Battery Information ID : 01/11
	//
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		             // Battery Mode(w)
		    , 15,
		SBCM, 1,     //  bit 15 - CAPACITY_MODE
		             //        0: Report in mA/mAh ; 1: Enabled
		SBMD, 16,    // Manufacture Data
		SBCC, 16,    // Cycle Count
	}

	//
	// Battery Information ID : 02/12
	//
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		SBDC, 16,    // Design Capacity
		SBDV, 16,    // Design Voltage
		SBOM, 16,    // Optional Mfg Function 1
		SBSI, 16,    // Specification Info
		SBDT, 16,    // Manufacture Date
		SBSN, 16,    // Serial Number
	}

	//
	// Battery Information ID : 04/14
	//
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		SBCH, 32,    // Device Checmistory (string)
	}

	//
	// Battery Information ID : 05/15
	//
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		SBMN, 128,   // Manufacture Name (s)
	}

	//
	// Battery Information ID : 06/16
	//
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0xA0),
		SBDN, 128,   // Device Name (s)
	}

	Method (_CRS, 0, NotSerialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 0, 1)
			IO (Decode16, 0x66, 0x66, 0, 1)
		})
		Return (ECMD)
	}

	Method (_REG, 2, NotSerialized)
	{
		// Initialize AC power state
		\PWRS = ACPW

		// Initialize LID switch state
		\LIDS = NOT(HPLD)

		// Enable OS control of fan speed
		// TODO FCOS = One

		// Force a read of CPU temperature
		// TODO Which temperature corresponds to the CPU?
		Local0 = TMP0
		/* So that we don't get a warning that Local0 is unused.  */
		Local0++
	}

/* Attention Codes
 *  00h     No Event
 *  01h-0Fh Reserved for ACPI Events
 *  10h-1Fh Hotkey Events
 *  22h     Critical Low Battery
 *  23h     Battery Empty
 *  24h     Trip Point Capacity Event for Battery 0
 *  26h     AC Adapter Attached
 *  27h     AC Adapter Detached
 *  28h     Power Off Switch Pressed
 *  29h     Power Off Switch Released
 *  2Ah     LID Open
 *  2Bh     LID Close
 *  3Eh     PME Event
 *  40h     Thermal Event
 *  41h     Global Wireless Enable Switch
 *  43h     Mute State Change
 *  4Ah     Battery 0 Attach/Detach
 *  4Bh     Battery 0 State Change
 *  66h     Mute Button
 */

	// Battery at critical low state
	Method (_Q22)
	{
		If (MBTS)
		{
			Notify (BATX, 0x80)
		}
	}

	// AC Power Connected
	Method (_Q26, 0, NotSerialized)
	{
		\PWRS = One
		Notify (AC, 0x80)
		Notify (BATX, 0x80)
		\PNOT ()
	}

	// AC Power Removed
	Method (_Q27, 0, NotSerialized)
	{
		\PWRS = Zero
		Notify (AC, 0x80)
		\PNOT ()
	}

	// LID Open
	Method (_Q2A)
	{
		\LIDS = One
		Notify (\_SB.LID0, 0x80)
	}

	// LID Close (Suspend Trigger)
	Method (_Q2B)
	{
		\LIDS = Zero
		Notify (\_SB.LID0, 0x80)
	}

	// Battery Attach/Detach Event
	Method (_Q4A)
	{
		Notify (BATX, 0x80)
		Notify (BATX, 0x81)
		\PNOT ()
	}

	// Battery State Change Event
	Method (_Q4B)
	{
		Notify (BATX, 0x80)
	}

	// Global Wireless Disable/Enable Event
	Method (_Q41, 0, NotSerialized)
	{
		IF (DCWL) //if Wlan exist
		{
			//TODO LANE = WLAT
		}
	}

	#include "ac.asl"
	#include "battery.asl"
	// TODO #include "fan.asl"
}
