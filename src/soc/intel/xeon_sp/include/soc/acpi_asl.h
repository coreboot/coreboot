/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_ACPI_ASL_H_
#define _SOC_ACPI_ASL_H_

#define GEN_PCIE_LEGACY_IRQ()				\
	Package () { 0x0000FFFF, 0x00, LNKA, 0x00 },	\
	Package () { 0x0001FFFF, 0x01, LNKB, 0x00 },	\
	Package () { 0x0002FFFF, 0x02, LNKC, 0x00 },	\
	Package () { 0x0003FFFF, 0x03, LNKD, 0x00 }

#define GEN_UNCORE_LEGACY_IRQ(dev)			\
	Package () { ##dev, 0x00, LNKA, 0x00 },		\
	Package () { ##dev, 0x01, LNKB, 0x00 },		\
	Package () { ##dev, 0x02, LNKC, 0x00 },		\
	Package () { ##dev, 0x03, LNKD, 0x00 }

#define GEN_PCIE_IOAPIC_IRQ(irq1, irq2, irq3, irq4)	\
	Package () { 0x0000FFFF, 0x00, 0x00, ##irq1 },	\
	Package () { 0x0001FFFF, 0x01, 0x00, ##irq2 },	\
	Package () { 0x0002FFFF, 0x02, 0x00, ##irq3 },	\
	Package () { 0x0003FFFF, 0x03, 0x00, ##irq4 }

#define GEN_UNCORE_IOAPIC_IRQ(dev,irq1,irq2,irq3,irq4)	\
	Package () { ##dev, 0x00, 0x00, ##irq1 },	\
	Package () { ##dev, 0x01, 0x00, ##irq2 },	\
	Package () { ##dev, 0x02, 0x00, ##irq3 },	\
	Package () { ##dev, 0x03, 0x00, ##irq4 }


#define MAKE_LINK_DEV(id,uid)						\
	Device (LNK##id)						\
	{								\
		Name (_HID, EISAID ("PNP0C0F"))				\
		Name (_UID, ##uid)					\
		Method (_PRS, 0, NotSerialized)				\
		{							\
			Return (PRS##id)				\
		}							\
		Method (_CRS, 0, Serialized)				\
		{							\
			Name (RTLA, ResourceTemplate ()			\
			{						\
			  IRQ (Level, ActiveLow, Shared) {}		\
			})						\
			CreateWordField (RTLA, 1, IRQ0)			\
			IRQ0 = 0					\
									\
			/* Set the bit from PIRQ Routing Register */	\
			IRQ0 = 1 << (^^PIR##id & ^^IREM)		\
			Return (RTLA)					\
		}							\
		Method (_SRS, 1, Serialized)				\
		{							\
			CreateWordField (Arg0, 1, IRQ0)			\
			FindSetRightBit (IRQ0, Local0)			\
			Local0--					\
			^^PIR##id = Local0				\
		}							\
		Method (_STA, 0, Serialized)				\
		{							\
			If (^^PIR##id & ^^IREN) {			\
				Return (0x9)				\
			} Else {					\
				Return (0xb)				\
			}						\
		}							\
		Method (_DIS, 0, Serialized)				\
		{							\
			^^PIR##id |= ^^IREN				\
		}							\
	}

#endif
