/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>

// Intel LPC Bus Device  - 0:1f.0

Device (LPCB)
{
	Name(_ADR, 0x001f0000)

	OperationRegion(LPC0, PCI_Config, 0x00, 0x100)
	Field (LPC0, AnyAcc, NoLock, Preserve)
	{
		Offset (0x40),
		PMBS,	16,	// PMBASE
		Offset (0x60),	// Interrupt Routing Registers
		PRTA,	8,
		PRTB,	8,
		PRTC,	8,
		PRTD,	8,
		Offset (0x68),
		PRTE,	8,
		PRTF,	8,
		PRTG,	8,
		PRTH,	8,

		Offset (0x80),	// IO Decode Ranges
		IOD0,	8,
		IOD1,	8,

		Offset (0xb8),	// GPIO Routing Control
		GR00,	 2,
		GR01,	 2,
		GR02,	 2,
		GR03,	 2,
		GR04,	 2,
		GR05,	 2,
		GR06,	 2,
		GR07,	 2,
		GR08,	 2,
		GR09,	 2,
		GR10,	 2,
		GR11,	 2,
		GR12,	 2,
		GR13,	 2,
		GR14,	 2,
		GR15,	 2,
	}

	#include <southbridge/intel/common/acpi/irqlinks.asl>

	#include "acpi/ec.asl"

	Device (DMAC)		// DMA Controller
	{
		Name(_HID, EISAID("PNP0200"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x00, 0x00, 0x01, 0x20)
			IO (Decode16, 0x81, 0x81, 0x01, 0x11)
			IO (Decode16, 0x93, 0x93, 0x01, 0x0d)
			IO (Decode16, 0xc0, 0xc0, 0x01, 0x20)
			DMA (Compatibility, NotBusMaster, Transfer8_16) { 4 }
		})
	}

	Device (FWH)		// Firmware Hub
	{
		Name (_HID, EISAID("INT0800"))
		Name (_CRS, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, 0xff000000, 0x01000000)
		})
	}

	Device (HPET)
	{
		Name (_HID, EISAID("PNP0103"))
		Name (_CID, 0x010CD041)

		Name(BUF0, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, HPET_BASE_ADDRESS, 0x400, FED0)
		})

		Method (_STA, 0)	// Device Status
		{
			Return (\HPTS(HPTE))
		}

		Method (_CRS, 0, Serialized) // Current resources
		{
			If (HPTE) {
				CreateDWordField(BUF0, \_SB.PCI0.LPCB.HPET.FED0._BAS, HPT0)
				If (HPAS == 1) {
					HPT0 = HPET_BASE_ADDRESS + 0x1000
				}

				If (HPAS == 2) {
					HPT0 = HPET_BASE_ADDRESS + 0x2000
				}

				If (HPAS == 3) {
					HPT0 = HPET_BASE_ADDRESS + 0x3000
				}
			}

			Return (BUF0)
		}
	}

	Device(PIC)	// 8259 Interrupt Controller
	{
		Name(_HID,EISAID("PNP0000"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x20, 0x20, 0x01, 0x02)
			IO (Decode16, 0x24, 0x24, 0x01, 0x02)
			IO (Decode16, 0x28, 0x28, 0x01, 0x02)
			IO (Decode16, 0x2c, 0x2c, 0x01, 0x02)
			IO (Decode16, 0x30, 0x30, 0x01, 0x02)
			IO (Decode16, 0x34, 0x34, 0x01, 0x02)
			IO (Decode16, 0x38, 0x38, 0x01, 0x02)
			IO (Decode16, 0x3c, 0x3c, 0x01, 0x02)
			IO (Decode16, 0xa0, 0xa0, 0x01, 0x02)
			IO (Decode16, 0xa4, 0xa4, 0x01, 0x02)
			IO (Decode16, 0xa8, 0xa8, 0x01, 0x02)
			IO (Decode16, 0xac, 0xac, 0x01, 0x02)
			IO (Decode16, 0xb0, 0xb0, 0x01, 0x02)
			IO (Decode16, 0xb4, 0xb4, 0x01, 0x02)
			IO (Decode16, 0xb8, 0xb8, 0x01, 0x02)
			IO (Decode16, 0xbc, 0xbc, 0x01, 0x02)
			IO (Decode16, 0x4d0, 0x4d0, 0x01, 0x02)
			IRQNoFlags () { 2 }
		})
	}

	Device(MATH)	// FPU
	{
		Name (_HID, EISAID("PNP0C04"))
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0xf0, 0xf0, 0x01, 0x01)
			IRQNoFlags() { 13 }
		})
	}

	Device(LDRC)	// LPC device: Resource consumption
	{
		Name (_HID, EISAID("PNP0C02"))
		Name (_UID, 2)
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x2e, 0x2e, 0x1, 0x02)		// First SuperIO
			IO (Decode16, 0x4e, 0x4e, 0x1, 0x02)		// Second SuperIO
			IO (Decode16, 0x61, 0x61, 0x1, 0x01)		// NMI Status
			IO (Decode16, 0x63, 0x63, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x65, 0x65, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x67, 0x67, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x80, 0x80, 0x1, 0x01)		// Port 80 Post
			IO (Decode16, 0x92, 0x92, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0xb2, 0xb2, 0x1, 0x02)		// SWSMI
			IO (Decode16, DEFAULT_PMBASE, DEFAULT_PMBASE, 0x1, 0x80)	// ICH7-M ACPI
			IO (Decode16, DEFAULT_GPIOBASE, DEFAULT_GPIOBASE, 0x1, 0x40)	// ICH7-M GPIO
		})
	}

	Device (RTC)	// Real Time Clock
	{
		Name (_HID, EISAID("PNP0B00"))
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x70, 0x70, 1, 8)
		})
	}

	Device (TIMR)	// Intel 8254 timer
	{
		Name(_HID, EISAID("PNP0100"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x40, 0x40, 0x01, 0x04)
			IO (Decode16, 0x50, 0x50, 0x10, 0x04)
			IRQNoFlags() {0}
		})
	}

	#include "acpi/superio.asl"
}
