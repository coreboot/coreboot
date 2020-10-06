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
	Name (_GPE, EC_SCI_GPE)	// GPE for Runtime SCI

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		// EC Name Space Configuration
		    , 1,        // Reserved                              ; 00h.0
		LCDS, 1,        // 1= BACKLIGHT ON , 0= BACKLIGHT OFF    ; 00h.1
		, 6,            // Reserved                              ; 00h.2 ~ 00h.6
		HTBN, 8,        // HOTKEY_BUTTON_NUMBER                  ; 01h For ABO Hot Key Function
		HTBT, 8,        // HOTKEY_BUTTON_TYPE                    ; 02h For ABO Hot Key Function
		LMEN, 1,        // Launch Manager enable .(1=Enable )    ; 03h.0, Lauanch manage
		, 7,            // Reserved                              ; 03h.1 ~ 03h.7
		ADAP, 2,        // Adaptor Type                          ; 04h.0 ~ 1
		                // 0x00 = default(65w)
		                // 0x01 = 65w
		                // 0x02 = 90w
		                // 0x03 = 120w
		, 6,            // Reserved                             ; 04h.2 ~ 04h.7
		Offset(0x08),   // Project Common Name space definition ; 08h - 2Ch
		Offset(0x2D),
		DSPM, 1,        // Display Mode.(0=dGPU, 1=iGPU )       ; 2Dh.0
		Offset(0x2E),
		EFP1, 4,        // Turbo Off P-State                    ; 2Eh.3-0
		Offset(0x40),   // ABO Common Name space definition     ; 2F - 5Ch
		Offset(0x5D),
		ENIB, 16,       // Ext_NameSpace_Index                  ; 5Dh
		                // Ext_NameSpace_Bank                   ; 5Eh
		ENDD, 8,        // Ext_NameSpace_Data                   ; 5Fh
		Offset(0x60),
		SMPR, 8,        // SMBus protocol register              ; 60h
		SMST, 8,        // SMBus status register                ; 61h
		SMAD, 8,        // SMBus address register               ; 62h
		SMCM, 8,        // SMBus command register               ; 63h
		SMD0, 0x100,    // SMBus data regs (32)                 ; 64h - 83h
		BCNT, 8,        // SMBus Block Count                    ; 84h
		SMAA, 24,       // SMBus Alarm Address                  ; 85h - 87h
		Offset(0x90),
		BMFN, 72,       // Battery Manufacture name             ; 90h - 98h
		BATD, 56,       // Battery Device name                  ; 99h - 9fh
		Offset(0xA1),
		, 1,            // Reserve                              ; A1h.0
		VIDO, 1,        // Video Out Button (1=Pressed)         ; A1h.1
		TOUP, 1,        // Touch Pad Button (0=On, 1=Off)       ; A1h.2
		Offset(0xA2),
		ODTS, 8,        // OS Shutdown Temp2 (DTS)              ; A2h
		Offset(0xA3),
		OSTY, 3,        // OSTYPE :   000- XP                   ; A3h.0-2
		                //            001- Vista
		                //            010- Linux
		                //            011- Win7
		, 4,            // Reserve                              ; A3h.3-6
		ADPT, 1,        // AC Adapter (0=OffLine, 1=OnLine)     ; A3h.7
		Offset(0xA4),
		PWAK, 1,        // PME Wake Enable(0=Disable, 1=Enable) ; A4h.0
		MWAK, 1,        // Modem Wake Enable(0/1=Disable/Enable); A4h.1
		LWAK, 1,        // LAN Wake Enable (0=Disable, 1=Enable); A4h.2
		RWAK, 1,        // RTC Wake Enable(0=DIsable,1=Enable)  ; A4h.3
		WWAK, 1,        // WLAN wake Enable (0=Disable,1=Enable); A4h.4
		UWAK, 1,        // USB WAKE(0=Disable, 1=Enable)        ; A4h.5
		KWAK, 1,        // Keyboard WAKE(0=Disable,1=Enable)    ; A4h.6
		TWAK, 1,        // TouchPad WAKE(0=Disable,1=Enable)    ; A4h.7
		Offset(0xA5),
		CCAC, 1,        // Charge Current (1=AC OFF)            ; A5h.0
		AOAC, 1,        // Adapter OVP (1=AC OFF)               ; A5h.1
		BLAC, 1,        // Batt learning (1=AC OFF)             ; A5h.2
		PSRC, 1,        // Command (1=AC OFF)                   ; A5h.3
		BOAC, 1,        // Batt OVP (1=AC OFF)                  ; A5h.4
		LCAC, 1,        // Leak Current (1=AC OFF)              ; A5h.5
		AAAC, 1,        // Air Adapter (1=AC OFF)               ; A5h.6
		ACAC, 1,        // AC Off (1=AC OFF)                    ; A5h.7
		Offset(0xA6),
		S3ST, 1,        // System entry S3 State                : A6.0
		S3RM, 1,        // System resume from S3 State          : A6.1
		S4ST, 1,        // System entry S4 State                : A6.2
		S4RM, 1,        // System resume from S4 State          : A6.3
		S5ST, 1,        // System entry S4 State                : A6.4
		S5RM, 1,        // System resume from S4 State          : A6.5
		, 2,            // Reserve                              ; A6h.6-7
		Offset(0xA7),
		OSTT, 8,        // OS Throttling Temp                   ; A7h
		OSST, 8,        // OS Shutdown Temp                     ; A8h
		THLT, 8,        // Throttling Temp                      ; A9h
		TCNL, 8,        // Tcontrol Temp                        ; AAh
		MODE, 1,        // Mode(0=Local, 1=Remote)              ; ABh.0
		, 2,            // Reserve                              ; ABh.1-2
		INIT, 1,        // INITOK(0/1=Controlled by OS/EC)      ; ABh.3
		FAN1, 1,        // FAN1 Active                          ; ABh.4
		FAN2, 1,        // FAN2 Active                          ; ABh.5
		FANT, 1,        // FAN Speed Time OK                    ; ABh.6
		SKNM, 1,        // Skin Mode (0/1=Skin Address 90/92)   ; ABh.7
		SDTM, 8,        // Shutdown Thermal Temperature         ; ACh
		FSSN, 4,        // Fan Speed Step Number                ; ADh.0-3
		                // 00 : Fan Off
		                // 01 : Fan On Speed 1
		                // 10 : Fan On Speed 2
		                // 11 : Fan On Speed 3
		FANU, 4,        // Machine Fan's number                 ; ADh.4-7
		PCVL, 4,        // Throttling Level                     ; AEh.0-3
		                // 0000 : Nothing
		                // 0001 : Clock throttling 12.5%
		                // 0010 : Clock throttling 25%
		                // 0011 : Clock throttling 37.5%
		                // 0100 : Clock throttling 50%
		, 2,            // Reserved                             ; AEh.4-5
		SWTO, 1,        // SW Throttling (1=Active)             ; AEh.6
		TTHR, 1,        // HW (THRM#) Throttling (1=Active)     ; AEh.7
		TTHM, 1,        // TS_THERMAL(1:Throttling for thermal) ; AFh.0
		THTL, 1,        // THROTTLING(1:Ctrl H/W throtting act) ; AFh.1
		, 2,            // Reserved                             ; AFh.2-3
		NPST, 4,        // Number of P-State level              ; AFh.4-7
		CTMP, 8,        // Current CPU Temperature              ; B0h
		CTML, 8,        // CPU local temperature                ; B1h
		SKTA, 8,        // Skin Temperature A                   ; B2h
		SKTB, 8,        // GPU Temperature                      ; B3h
		SKTC, 8,        // Skin Temperature C                   ; B4h
		, 8,            // Reserved                             ; B5h
		NTMP, 8,        // North Bridge Diode Temp              ; B6h
		Offset(0xB7),
		, 1,            // Reserved                             ; B7h.0
		SKEY, 1,        // Security key event                   ; B7h.1
		DIGM, 1,        // Digital Mode (1=Selected)            ; B7h.2
		CDLE, 1,        // CD lock mode enable                  ; B7h.3
		, 4,            // Reserved                             ; B7h.4-7
		, 1,            // Reserved                             ; B8h.0
		LIDF, 1,        // LID flag (1=Closed, 0=Opened)        ; B8h.1
		PMEE, 1,        // PME event (0=off, 1=On)              ; B8h.2
		PWBE, 1,        // Power button event (0=off, 1=On)     ; B8h.3
		RNGE, 1,        // Ring in event (0=off, 1=On)          ; B8h.4
		BTWE, 1,        // Bluetooth wake event (0=off, 1=On)   ; B8h.5
		, 2,            // Reserved                             ; B8h.6-7
		Offset(0xB9),
		BRTS, 8,        // Brightness Value                     ; B9h
		S35M, 1,        // S3.5 HotKey test mode                ; BAh.0
		S35S, 1,        // S3.5 function status                 ; BAh.1
		, 2,            // Reserved                             ; BAh.2-3
		FFEN, 1,        // IRST support bit (1= Support)        ; BAh.4
		FFST, 1,        // IRST status flag                     ; BAh.5
		, 2,            // Reserved                             ; BAh.6-7
		WLAT, 1,        // Wireless LAN (0=Inactive, 1=Active)  ; BBh.0
		BTAT, 1,        // BlueTooth (0=Inactive, 1=Active)     ; BBh.1
		WLEX, 1,        // Wireless LAN (0=Not Exist, 1=Exist)  ; BBh.2
		BTEX, 1,        // BlueTooth (0=Not Exist, 1=Exist)     ; BBh.3
		KLSW, 1,        // Kill Switch (0=Off, 1=On)            ; BBh.4
		WLOK, 1,        // Wireless LAN Initialize OK           ; BBh.5
		AT3G, 1,        // 3G (0=non-active, 1=active)          ; BBh.6
		EX3G, 1,        // 3G (0=Not Exist, 1=Exist)            ; BBh.7
		PJID, 8,        // Project ID                           ; BCh
		CPUJ, 3,        // CPU Type                             ; BDh.0-2
		                // 00, Tj85,
		                // 01, Tj90,
		                // 02, Tj100,
		                // 03, Tj105,
		                // 04 - 07, Reserved
		CPNM, 3,        // CPU Core number                      ; BDh.3-5
		                // 00, Single Core
		                // 01, Dual Core
		                // 02, Quad Core
		                // 03 - 07, Reserved
		GATY, 2,        // VGA Type                             ; BDh.6-7
		                // 00, UMA
		                // 01. Discrete
		                // 02 - 03, Reserved
		Offset(0xBE),
		BOL0, 1,        // Batt0 (0=OffLine, 1=OnLine)          ; BEh.0
		BOL1, 1,        // Batt1 (0=OffLine, 1=OnLine)          ; BEh.1
		, 2,            // Reserved                             ; BEh.2-3
		BCC0, 1,        // Batt0 be charging (1=Charging)       ; BEh.4
		BCC1, 1,        // Batt1 be charging (1=Charging)       ; BEh.5
		, 2,            // Reserved                             ; BEh.6-7
		BPU0, 1,        // Batt0 (1=PowerUp)                    ; BFh.0
		BPU1, 1,        // Batt1 (1=PowerUp)                    ; BFh.1
		, 2,            // Reserved                             ; BFh.2-3
		BOS0, 1,        // Batt0 (1=OnSMBUS)                    ; BFh.4
		BOS1, 1,        // Batt1 (1=OnSMBUS)                    ; BFh.5
		, 2,            // Reserved                             ; BFh.6-7
		Offset(0xC0),
		BTY0, 1,        // Batt Type (0=NiMh, 1=LiIon)          ; C0h.0
		BAM0, 1,        // Battery mode (0=mA, 1=mW)            ; C0h.1
		BAL0, 1,        // Low Battery                          ; C0h.2
		, 1,            // Reserved                             ; C0h.3
		BMF0, 3,        // Battery Manufacturer                 ; C0h.4-6
		                // 001 : Sanyo
		                // 010 : Sony
		                // 100 : Pansonic
		                // 101 : CPT
		, 1,            // Reserved                             ; C0h.7
		BST0, 8,        // Battery Status                       ; C1h
		                // Bit0 : Discharging
		                // Bit1 : Charging
		                // Bit2 : Discharg and Critical Low
		                // Bit3-7 : Reserved
		BRC0, 16,       // Batt Remaining Capacity              ; C2h, C3h
		BSN0, 16,       // Batt Serial Number                   ; C4h, C5h
		BPV0, 16,       // Batt Present Voltage                 ; C6h, C7h
		BDV0, 16,       // Batt Design Voltage                  ; C8h, C9h
		BDC0, 16,       // Batt Design Capacity                 ; CAh, CBh
		BFC0, 16,       // Batt Last Full Charge Capacity       ; CCh, CDh
		GAU0, 8,        // Batt Gasgauge                        ; CEh
		CYC0, 8,        // Batt Cycle Counter                   ; CFh
		BPC0, 16,       // Batt Current                         ; D0h, D1h
		BAC0, 16,       // Batt Average Current                 ; D2h, D3h
		BTW0, 8,        // Batt Comsuption                      ; D4h
		BVL0, 8,        // Batt Battery Volt                    ; D5h
		BTM0, 8,        // Batt Battery Temp                    ; D6h
		BAT0, 8,        // Batt Average Temp (Degree C)         ; D7h
		BCG0, 16,       // Batt charge current                  ; D8h, D9h
		BCT0, 8,        // Batt Current Temp Semple counter     ; DAh
		BCI0, 8,        // BATT CMD Index for read BATT(SMB)    ; DBh
		BCM0, 8,        // Count up to Communicate BATT         ; DCh
		BOT0, 8,        // Count up if BATT over Temp           ; DDh
		BSSB, 16,       // BATT Battery Status SMB              ; DEh, DFh
		BOV0, 8,        // BATT Over Voltage Count              ; E0h
		BCF0, 8,        // BATT Communication Fail Counter      ; E1h
		BAD0, 8,        // Battery Voltage of ADC               ; E2h
		BCV1, 16,       // Cell Voltage 1 (mV)                  ; E3h, E4h
		BCV2, 16,       // Cell Voltage 2 (mV)                  ; E5h, E6h
		BCV3, 16,       // Cell Voltage 3 (mV)                  ; E7h, E8h
		BCV4, 16,       // Cell Voltage 4 (mV)                  ; E9h, EAh
		Offset(0xF4),
		BMD0, 16,       // Manufacture Date                     ; F4h, F5h
		                // Batt Day	                            ; BIT[4:0] (Day)
		                // Batt Month                           ; BIT[9:5] (Month)
		                // Batt Year                            ; BIT[15:10] (Year)
		BACV, 16,       // Charging Voltage                     ; F6h, F7h
		BDN0, 8,        // Battery Cell Number                  ; F8h
		, 8             // Last byte for Reserved.
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
		\PWRS = ADPT

		// Force a read of CPU temperature
		Local0 = CTML
		/* So that we don't get a warning that Local0 is unused.  */
		Local0++
	}


/*
 * EC Query Responses
 *
 * DTS temperature update    10h
 * Decrease brightness event 11h
 * Increase brightness event 12h
 * Cover lid open            15h
 * Cover lid close           16h
 * External device plugged   17h
 * External device removed   18h
 * Bluetooth wake up event   19h
 * Scr expand event          1Bh
 * Display toggle            1Ch
 * CPU  fast/slow event      1Dh
 * Battery in critical low   22h
 * Battery in low state      23h
 * Battery pack plug in      25h
 * Docking in                2Ah
 * Undock                    2Bh
 * Power button pressed      32h
 * AC plug in                37h
 * AC removed                38h
 * Modem ring in             3Ah
 * PME signal active         3Eh
 * Hotkey make Function      45h
 * Hotkey break Function     46h
 */

	/* Decrease brightness hotkey */
	Method (_Q11, 0, NotSerialized)
	{
		\_SB.MB.BRTD()
	}

	/* Increase brightness hotkey */
	Method (_Q12, 0, NotSerialized)
	{
		\_SB.MB.BRTU()
	}

	/* Lid opened */
	Method (_Q15, 0, NotSerialized)
	{
		\_SB.MB.LIDO()
	}

	/* Lid closed */
	Method (_Q16, 0, NotSerialized)
	{
		\_SB.MB.LIDC()
	}

	/* Switch display hotkey */
	Method (_Q1C, 0, NotSerialized)
	{
		\_SB.MB.DSPS()
	}

	// AC Power Connected
	Method (_Q37, 0, NotSerialized)
	{
		\PWRS = 1
		Notify (AC, 0x80)
		Notify (BATX, 0x80)
		// TODO ADD CPU power profile
		\PNOT ()
	}

	// AC Power Removed
	Method (_Q38, 0, NotSerialized)
	{
		\PWRS = 0
		Notify (AC, 0x80)
		Notify (BATX, 0x80)
		// TODO ADD CPU power profile
		\PNOT ()
	}

	/* Wireless toggle hotkey */
	Method (_Q40, 0, NotSerialized)
	{
		\_SB.MB.WLTG()
	}

	// Battery at critical low state
	Method (_Q22, 0, NotSerialized)
	{
		Notify (BATX, 0x80)
	}

	// Battery insert
	Method(_Q25, 0, NotSerialized)
	{
		Notify(BATX, 0x81)
		Notify(BATX, 0x80)
	}

	// Commuation Device Disable/Enable Event
	Method(_Q60, 0, NotSerialized)
	{
		IF(WLEX) //if Wlan exist
		{
			//TODO LANE = WLAT
		}
	}

	#include "ac.asl"
	#include "battery.asl"
}
