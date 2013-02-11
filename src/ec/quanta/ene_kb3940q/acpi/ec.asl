/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * The mainboard must define a PNOT method to handle power
 * state notifications and Notify CPU device objects to
 * re-evaluate their _PPC and _CST tables.
 */

External (\_PR.CPU0._PPC, IntObj)

Device (EC0)
{
	Name (_HID, EISAID ("PNP0C09"))
	Name (_UID, 1)
	Name (_GPE, Add(EC_SCI_GPI, 16))	// GPE for Runtime SCI

	OperationRegion (ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, Lock, Preserve)
	{
		// EC Name Space Configuration

		Offset(0x40),
		BDC0, 16,       // Batt Design Capacity                 ; 40h, 41h
		BDV0, 16,       // Batt Design Voltage                  ; 42h, 43h
		BFC0, 16,       // Batt Last Full Charge Capacity       ; 44h, 45h
		BPC0, 16,       // Batt Current                         ; 46h, 47h
		BRC0, 16,       // Batt Remaining Capacity              ; 48h, 49h
		BPV0, 16,       // Batt Present Voltage                 ; 4Ah, 4Bh
		BCG0, 16,       // Batt Charge current                  ; 4Ch, 4Dh
		BACV, 16,       // Batt Charging Voltage                ; 4Eh, 4Fh
		BTM0, 16,       // Batt Battery Temp                    ; 50h, 51h
		BSN0, 16,       // Batt Serial Number                   ; 52h, 53h
		BPCT, 16,       // Batt Percentage of full charge       ; 54h, 55h
		BSSB, 16,       // BATT Battery Status SMB              ; 56h, 57h
		CYC0, 16,       // Batt Cycle Counter                   ; 58h, 59h
		BMD0, 16,       // Manufacture Date                     ; 5Ah, 5Bh
		                // Batt Day                             ; BIT[4:0] (Day)
		                // Batt Month                           ; BIT[9:5] (Month)
		                // Batt Year                            ; BIT[15:10] (Year)


		Offset(0x60),
		BCHM, 32,       // Battery Chemistry                    ; 60h - 64h
		BATD, 56,       // Battery Device name                  ; 64h - 6Ah

		Offset(0x70),
		ADPT, 1,        // AC Adapter Status for OS             ; 70h.0
		ADPN, 1,        // AC Adapter H/W status                ; 70h.1
		BTIN, 1,        // Battery Present                      ; 70h.2
		BTBD, 1,        // Battery Malfunction                  ; 70h.3
		ACMD, 1,        // ACPI Mode                            ; 70h.4
		    , 1,        // Reserved                             ; 70h.5
		SSBS, 1,        // 1=Standard BIOS, 0=Coreboot          ; 70h.6
		PSTH, 1,        // Passive Thermal Policy               ; 70h.7
		BST0, 8,        // Battery Status                       ; 71h
		                // Bit0 : Discharging
		                // Bit1 : Charging
		                // Bit2 : Discharging and Critical Low
		                // Bit3-7 : Reserved
		LIDF, 1,        // Lid is open                          ; 72h.0
		GPRC, 1,        // Recovery GPI Status                  ; 72h.1
		    , 6,        // Reserved                             ; 72h.2-7
		TPLD, 1,        // TouchPad LED Activation              ; 73h.0
		TPST, 1,        // Touchpad LED Status                  ; 73h.1
		    , 6,        // Reserved                             ; 73h.2-7

		Offset(0x78),
		CTMP, 8,        // Current CPU Temperature              ; 78h
		SKTB, 8,        // GPU Temperature                      ; 79h
		LTM1, 8,        // Local Temp 1                         ; 7Ah
		LTM2, 8,        // Local Temp 2                         ; 7Bh
		FTCH, 16,       // Fan Tachometer value                 ; 7Ch - 7Dh
		FDBG, 16,       // Fan Debug - Override Fan Tach value  ; 7Eh - 7Fh
		    , 1,        // Reserved                             ; 80h.0
		KBID, 1,        // 0=EN KBD, 1=JP KBD                   ; 80h.1
		    , 6,        // Reserved                             ; 80h.2-7
		NPST, 8,        // Number of P-State level              ; 81h
		MPST, 8,        // Maxumum P-State                      ; 82h
		KWAK, 1,        // Keyboard WAKE(0=Disable,1=Enable)    ; 83h.0
		TWAK, 1,        // TouchPad WAKE(0=Disable,1=Enable)    ; 83h.1
		    , 1,        // Reserved                             ; 83h.2
		LWAK, 1,        // LAN Wake Enable (0=Disable, 1=Enable); 83h.3
		RWAK, 1,        // RTC Wake Enable(0=DIsable,1=Enable)  ; 83h.4
		    , 3,        // Reserved                             ; 83h.5-7
		KBEV, 1,        // Keyboard Wake Event                  ; 84h.0
		TPEV, 1,        // TouchPad Wake Event                  ; 84h.1
		LDEV, 1,        // Lid Wake Event                       ; 84h.2
		    , 4,        // Reserved                             ; 84h.3-6
		PBEV, 1,        // Power Button Wake Event              ; 84h.7

		ECCD, 8,        // EC Code State                        ; 85h
		ROFW, 8,        // RO FW Reason ID                      ; 86h

		Offset(0xBA),
		FWVR, 48,       // EC Firmware Version                  ; BAh-BFh

		Offset(0xC0),
		SMPR, 8,        // SMBus protocol register              ; C0h
		SMST, 8,        // SMBus status register                ; C1h
		SMAD, 8,        // SMBus address register               ; C2h
		SMCM, 8,        // SMBus command register               ; C3h
		SMD0, 0x100,    // SMBus data regs (32)                 ; C4h - E3h
		BCNT, 8,        // SMBus Block Count                    ; E4h
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

	Method(_STA)
	{
		Return(0x0F)
	}


	Method (_REG, 2, NotSerialized)
	{
		// Initialize AC power state
		Store (ADPT, \PWRS)

		// Initialize LID switch state
		Store (LIDF, \LIDS)

		// Force a read of CPU temperature
		Store (CTMP, Local0)

		// Find and program number of P-States
		Store (SizeOf (\_PR.CPU0._PSS), MPST)
		Store ("Programming number of P-states: ", Debug)
		Store (MPST, Debug)

		// Find and program the current P-State
		Store(\_PR.CPU0._PPC, NPST)
		Store ("Programming Current P-state: ", Debug)
		Store (NPST, Debug)
	}

/*
 * EC Query Responses
 *
 * Lid Status Change         06h
 * Wifi Button Event (F12)   07h
 * TZ Event Update CPU Temp  08h
 * CPU P-State Down          0Eh
 * CPU P-State UP            0Fh

 * AC plug in                10h
 * AC removed                11h
 * Battery Plugged in        12h
 * Battery Removed           13h
 * Battery State Change      14h
 */

	// Wifi Button Event
	Method (_Q07)
	{
		Store ("Wifi Button Event 0x07", Debug)
	}

	// Thermal Event
	Method (_Q08)
	{
		Store ("Thermal Event 0x08", Debug)
		Notify(\_TZ.THRM, 0x80)
	}

	// Pstate Down
	Method (_Q0E)
	{
		Store ("Pstate Event 0x0E", Debug)

		Store(\_PR.CPU0._PPC, Local0)
		Subtract(PPCM, 0x01, Local1)

		If(LLess(Local0, Local1)) {
			Increment(Local0)
			\PPCN ()
		}

		Store(Local0, NPST)
	}

	// Pstate Up
	Method (_Q0F)
	{
		Store ("Pstate Event 0x0F", Debug)
		Store(\_PR.CPU0._PPC, Local0)

		If(Local0) {
			Decrement(Local0)
			\PPCN ()
		}

		Store(Local0, NPST)
	}

	// AC Power Connected
	Method (_Q10, 0, NotSerialized)
	{
		Store ("AC Insertion Event 0x10", Debug)
		Store (One, \PWRS)
		Notify (AC, 0x80)
		Notify (BATX, 0x80)
		\PNOT ()
	}

	// AC Power Removed
	Method (_Q11, 0, NotSerialized)
	{
		Store ("AC Detach Event 0x11", Debug)
		Store (Zero, \PWRS)
		Notify (AC, 0x80)
		Notify (BATX, 0x80)
		\PNOT ()
	}

	// Battery State Change - Attach Event
	Method (_Q12, 0, NotSerialized)
	{
		Store ("Battery Insertion Event 0x12", Debug)

		Notify (BATX, 0x81)
		Notify (BATX, 0x80)
		\PNOT ()
	}

	// Battery State Change - Detach Event
	Method (_Q13, 0, NotSerialized)
	{
		Store ("Battery Detach Event 0x13", Debug)

		Notify (BATX, 0x81)
		Notify (BATX, 0x80)
		\PNOT ()
	}


	// Battery State Change Event
	Method (_Q14, 0, NotSerialized)
	{
		Store ("Battery State Change Event 0x14", Debug)

		Notify (BATX, 0x80)
	}

	// Lid Switch Event
	Method (_Q06)
	{
		Store ("Lid Switch Event 0x06", Debug)
		sleep(20)
		Store (LIDF, \LIDS)
		Notify (\_SB.LID0, 0x80)
	}

	#include "ac.asl"
	#include "battery.asl"
}
