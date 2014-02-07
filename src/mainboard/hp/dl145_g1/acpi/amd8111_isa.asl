/*
 * Copyright 2005 AMD
 * Copyright (C) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 */
//AMD8111 isa

Device (ISA) {
	// lpc  0x00040000
	Method (_ADR, 0, NotSerialized) {
		Return (DADD(\_SB.PCI0.SBDN, 0x00010000))
	}
	/*
	OperationRegion (PIRY, PCI_Config, 0x51, 0x02) // LPC Decode Registers
	Field (PIRY, ByteAcc, NoLock, Preserve) {
		Z000,   2,  // Parallel Port Range
				,   1,
		ECP ,   1,  // ECP Enable
		FDC1,   1,  // Floppy Drive Controller 1
		FDC2,   1,  // Floppy Drive Controller 2
						Offset (0x01),
		Z001,   3,  // Serial Port A Range
		SAEN,   1,  // Serial Post A Enabled
		Z002,   3,  // Serial Port B Range
		SBEN,   1   // Serial Post B Enabled
	}
	*/
	Device (PIC) {
		Name (_HID, EisaId ("PNP0000"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0020, 0x0020, 0x01, 0x02)  // Master Interrupt controller
			IO (Decode16, 0x00A0, 0x00A0, 0x01, 0x02)  // Slave Interrupt controller
			IRQ (Edge, ActiveHigh, Exclusive) {2}
		})
	}
	Device (DMA1) {
	Name (_HID, EisaId ("PNP0200"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0000, 0x0000, 0x01, 0x10)  // Slave DMA controller
			IO (Decode16, 0x0080, 0x0080, 0x01, 0x10)  // DMA page registers
			IO (Decode16, 0x00C0, 0x00C0, 0x01, 0x20)  // Master DMA controller
			DMA (Compatibility, NotBusMaster, Transfer16) {4}
		})
	}
	Device (TMR) {
		Name (_HID, EisaId ("PNP0100"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0040, 0x0040, 0x01, 0x04)  // Programmable Interval timer
			IRQ (Edge, ActiveHigh, Exclusive) {0}
		})
	}
	Device (RTC) {
		Name (_HID, EisaId ("PNP0B00"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0070, 0x0070, 0x01, 0x04)  // Realtime Clock and CMOS ram
			IRQ (Edge, ActiveHigh, Exclusive) {8}
		})
	}
	Device (SPKR) {
		Name (_HID, EisaId ("PNP0800"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x0061, 0x0061, 0x01, 0x01)  // PC speaker
		})
	}
	Device (COPR) { // Co-processor
		Name (_HID, EisaId ("PNP0C04"))
		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x00F0, 0x00F0, 0x01, 0x10)  // Floating point Error control
			IRQ (Edge, ActiveHigh, Exclusive) {13}
		})
	}
	Device (SYSR) {  // System control registers (?)
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x00)
		Name (CRS, ResourceTemplate () {
			IO (Decode16, 0x0010, 0x0010, 0x01, 0x10)
			IO (Decode16, 0x0022, 0x0022, 0x01, 0x1E)
			IO (Decode16, 0x0044, 0x0044, 0x01, 0x1C)
			IO (Decode16, 0x0062, 0x0062, 0x01, 0x02)
			IO (Decode16, 0x0065, 0x0065, 0x01, 0x0B)
			IO (Decode16, 0x0074, 0x0074, 0x01, 0x0C)
			IO (Decode16, 0x0080, 0x0080, 0x01, 0x01)
			IO (Decode16, 0x0084, 0x0084, 0x01, 0x03)
			IO (Decode16, 0x0088, 0x0088, 0x01, 0x01)
			IO (Decode16, 0x008C, 0x008C, 0x01, 0x03)
			IO (Decode16, 0x0090, 0x0090, 0x01, 0x10)
			IO (Decode16, 0x00A2, 0x00A2, 0x01, 0x1E)
			IO (Decode16, 0x00E0, 0x00E0, 0x01, 0x10)
			// IO (Decode16, 0x0190, 0x0190, 0x01, 0x04) // Added this to remove ACPI Unrepoted IO Error
			// EISA defined level triggered interrupt control registers
			IO (Decode16, 0x04D0, 0x04D0, 0x01, 0x02)
			// IO (Decode16, 0x0B78, 0x0B78, 0x01, 0x04) // Added this to remove ACPI Unrepoted IO Error
			// IO (Decode16, 0xDE00, 0xDE00, 0x00, 0x80)
			// IO (Decode16, 0xDE80, 0xDE80, 0x00, 0x80)
			IO (Decode16,0xDE00,0xDE00,0x00,0x80)
			IO (Decode16,0xDE80,0xDE80,0x00,0x80)
			// IO (Decode16, 0x1100, 0x117F, 0x01, 0x80) //wrh092302 - added to report Thor NVRAM
			// IO (Decode16, 0x1180, 0x11FF, 0x01, 0x80)
			IO (Decode16, 0x0000, 0x0000, 0x00, 0x00,_Y0D) // PMBS block
			IO (Decode16, 0x0000, 0x0000, 0x00, 0x00,_Y0E) // SMBS block
			IO (Decode16, 0x0000, 0x0000, 0x00, 0x00,_Y0F) // GPBS block
		})
		Method (_CRS, 0, NotSerialized) {
			CreateWordField (CRS, \_SB.PCI0.ISA.SYSR._Y0D._MIN, GP00)
			CreateWordField (CRS, \_SB.PCI0.ISA.SYSR._Y0D._MAX, GP01)
			CreateByteField (CRS, \_SB.PCI0.ISA.SYSR._Y0D._LEN, GP0L)
			Store (PMBS, GP00)
			Store (PMBS, GP01)
			Store (PMLN, GP0L)
			If (SMBS) {
				CreateWordField (CRS, \_SB.PCI0.ISA.SYSR._Y0E._MIN, GP10)
				CreateWordField (CRS, \_SB.PCI0.ISA.SYSR._Y0E._MAX, GP11)
				CreateByteField (CRS, \_SB.PCI0.ISA.SYSR._Y0E._LEN, GP1L)
				Store (SMBS, GP10)
				Store (SMBS, GP11)
				Store (SMBL, GP1L)
			}
			If (GPBS) {
				CreateWordField (CRS, \_SB.PCI0.ISA.SYSR._Y0F._MIN, GP20)
				CreateWordField (CRS, \_SB.PCI0.ISA.SYSR._Y0F._MAX, GP21)
				CreateByteField (CRS, \_SB.PCI0.ISA.SYSR._Y0F._LEN, GP2L)
				Store (GPBS, GP20)
				Store (GPBS, GP21)
				Store (GPLN, GP2L)
			}
			Return (CRS)
		}
	}
	Device (MEM) {
		Name (_HID, EisaId ("PNP0C02"))
		Name (_UID, 0x01)
		Method (_CRS, 0, Serialized) {
			Name (BUF0, ResourceTemplate () {
				Memory32Fixed (ReadWrite, 0x000E0000, 0x00020000) // BIOS E4000-FFFFF
				Memory32Fixed (ReadWrite, 0x000C0000, 0x00010000) // video BIOS c0000-c8404
				Memory32Fixed (ReadWrite, 0xFEC00000, 0x00001000) // I/O APIC
				Memory32Fixed (ReadWrite, 0xFFC00000, 0x00380000) // LPC forwarded, 4 MB w/ROM
				Memory32Fixed (ReadWrite, 0xFEE00000, 0x00001000) // Local APIC
				Memory32Fixed (ReadWrite, 0xFFF80000, 0x00080000) // Overlay BIOS
				Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
				Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
				Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
				Memory32Fixed (ReadWrite, 0x00000000, 0x00000000) // Overlay BIOS
			})
// Read the Video Memory length
			CreateDWordField (BUF0, 0x14, CLEN)
			CreateDWordField (BUF0, 0x10, CBAS)

			ShiftLeft (VGA1, 0x09, Local0)
			Store (Local0, CLEN)

			Return (BUF0)
		}
	}
#include "superio/winbond/w83627hf/acpi/superio.asl"
}
