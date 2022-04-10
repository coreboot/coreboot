/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the SCH5545 SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices and controlling power
 * saving mode on logical devices or the whole chip.
 *
 *   LDN		State
 * 0x0 FDC		Not implemented
 * 0x3 PP		Not implemented
 * 0x4 UARTA		Implemented and tested
 * 0x5 UARTB		Implemented
 * 0x7 KBC		Implemented and tested
 *
 * Controllable through preprocessor defines:
 * SUPERIO_PNP_BASE	I/O address of the first PnP configuration register
 * SCH5545_SHOW_UARTA	If defined, UARTA will be exposed.
 * SCH5545_SHOW_UARTB	If defined, UARTB will be exposed.
 * SCH5545_SHOW_KBC	If defined, the KBC will be exposed.
 * SCH5545_EMI_BASE	If defined, the Embedded Memory Interface resource will be exposed.
 * SCH5545_RUNTIME_BASE	If defined, The Runtime Registers resource will be exposed.
 */

#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME SCH5545
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

/*
* Common helpers will not work on this chip. IO, DMA and IRQ resources.
* These are accessed via LPC interface LDN 0xC.
*/
#undef PNP_READ_IO
#undef PNP_READ_IRQ
#undef PNP_READ_DMA
#undef PNP_WRITE_IO
#undef PNP_WRITE_IRQ
#undef PNP_WRITE_DMA

Device(SIO1) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("SMSC SCH5545 Super I/O"))
	Name (_UID, SUPERIO_UID(SIO1,))

#ifdef SCH5545_EMI_BASE
	Name (IO1B, SCH5545_EMI_BASE)
#endif
#ifdef SCH5545_RUNTIME_BASE
	Name (IO2B, SCH5545_RUNTIME_BASE)
#endif
	Name (IOST, 0x0001) /* IO decoding status */
	Name (MSFG, One) /* Mouse wake config */
	Name (KBFG, One) /* Keyboard wake config */
	Name (PMFG, Zero) /* Wake config */

	/* SuperIO configuration ports */
	OperationRegion (CREG, SystemIO, SUPERIO_PNP_BASE, 0x02)
	Field (CREG, ByteAcc, NoLock, Preserve)
	{
		PNP_ADDR_REG,	8,
		PNP_DATA_REG,	8
	}
	IndexField (PNP_ADDR_REG, PNP_DATA_REG, ByteAcc, NoLock, Preserve)
	{
		Offset (0x07),
		PNP_LOGICAL_DEVICE,	8, /* Logical device selector */

		Offset (0x30),
		PNP_DEVICE_ACTIVE,	1, /* Logical device activation */

		Offset (0x69),
		CR69,	8,	/* UART1 Base address Registers */
		CR6A,	8,
		CR6B,	8,
		Offset (0x6D),
		CR6D,	8,	/* UART2 Base address Registers */
		CR6E,	8,
		CR6F,	8,
		Offset (0x7D),
		CR7D,	8,	/* FD Base address Registers */
		CR7E,	8,
		CR7F,	8,
		Offset (0x81),
		CR81,	8,	/* LPT Base address Registers */
		CR82,	8,
		CR83,	8,
		Offset (0xF0),
		OPT0,	8,	/* MISC registers */
		OPT1,	8,
		OPT2,	8,
		OPT3,	8,
		OPT4,	8,
		OPT5,	8
	}

#ifdef SCH5545_RUNTIME_BASE
	/* Runtime registers */
	OperationRegion (RNTR, SystemIO, SCH5545_RUNTIME_BASE, 0x40)
	Field (RNTR, ByteAcc, NoLock, Preserve)
	{
		PMES,	1,	/* PME Global Status */
		Offset (0x01),
		PMEN,	1,	/* PME Global Enable */
		Offset (0x02),
		PMS1,	8,	/* PME Status 1 for KBD and PS2M */
		PMS2,	8,	/* PME Status 2 for EC, WDT, Bat, Intruder */
		PMS3,	8,	/* PME Status 3 for GPIOs */
		PME1,	8,	/* PME Enable 1 for KBD and PS2M */
		PME2,	8,	/* PME Enable 2 for EC, WDT, Bat, Intruder */
		PME3,	8,	/* PME Enable 3 for GPIOs */
		Offset (0x10),
		SOIS,	1,	/* SMI Global Status*/
		Offset (0x11),
		SOIE,	1,	/* SMI Global Enable */
		Offset (0x12),
		SST1,	8,	/* SMI Status 1 for UARTs, LPT, FD, EC, Bat */
		SST2,	8,	/* SMI Status 2 for KBD, PS2M, WDT, Intruder */
		SST3,	8,	/* SMI Status 3 for GPIOs */
		Offset (0x16),
		SEN1,	8,	/* SMI Enable 1 for UARTs, LPT, FD, EC, Bat */
		SEN2,	8,	/* SMI Enable 2 for KBD, PS2M, WDT, Intruder */
		SEN3,	8,	/* SMI Enable 3 for GPIOs */
		Offset (0x25),
		LED1,	8,	/* LED control register */
		Offset (0x28),
		GPSR,	8,	/* GPIO Select Register  */
		GPRR,	8	/* GPIO Read Register */
	}
#endif
	Name (CRS, ResourceTemplate ()
	{
		IO (Decode16,
			0x0000,
			0x0000,
			0x00,
			0x00,
			_Y11)
#ifdef SCH5545_EMI_BASE
		IO (Decode16,
			0x0000,
			0x0000,
			0x00,
			0x00,
			_Y12)
#endif
#ifdef SCH5545_RUNTIME_BASE
		IO (Decode16,
			0x0000,
			0x0000,
			0x00,
			0x00,
			_Y13)
#endif
	})
	Method (_CRS, 0, NotSerialized)
	{
		If (SUPERIO_PNP_BASE)
		{
			CreateWordField (CRS, \_SB.PCI0.LPCB.SIO1._Y11._MIN, GPI0)
			CreateWordField (CRS, \_SB.PCI0.LPCB.SIO1._Y11._MAX, GPI1)
			CreateByteField (CRS, \_SB.PCI0.LPCB.SIO1._Y11._LEN, GPIL)
			GPI0 = SUPERIO_PNP_BASE
			GPI1 = SUPERIO_PNP_BASE
			GPIL = 0x02
		}
#ifdef SCH5545_EMI_BASE
		If (IO1B)
		{
			CreateWordField (CRS, \_SB.PCI0.LPCB.SIO1._Y12._MIN, GP10)
			CreateWordField (CRS, \_SB.PCI0.LPCB.SIO1._Y12._MAX, GP11)
			CreateByteField (CRS, \_SB.PCI0.LPCB.SIO1._Y12._LEN, GPL1)
			GP10 = SCH5545_EMI_BASE
			GP11 = SCH5545_EMI_BASE
			GPL1 = 0x10
		}
#endif
#ifdef SCH5545_RUNTIME_BASE
		If (IO2B)
		{
			CreateWordField (CRS, \_SB.PCI0.LPCB.SIO1._Y13._MIN, GP20)
			CreateWordField (CRS, \_SB.PCI0.LPCB.SIO1._Y13._MAX, GP21)
			CreateByteField (CRS, \_SB.PCI0.LPCB.SIO1._Y13._LEN, GPL2)
			GP20 = SCH5545_RUNTIME_BASE
			GP21 = SCH5545_RUNTIME_BASE
			GPL2 = 0x40
		}
#endif
		Return (CRS)
	}

	#undef PNP_ENTER_MAGIC_1ST
	#undef PNP_ENTER_MAGIC_2ND
	#undef PNP_ENTER_MAGIC_3RD
	#undef PNP_ENTER_MAGIC_4TH
	#undef PNP_EXIT_MAGIC_1ST
	#undef PNP_EXIT_SPECIAL_REG
	#undef PNP_EXIT_SPECIAL_VAL
	#define PNP_ENTER_MAGIC_1ST	0x55
	#define PNP_EXIT_MAGIC_1ST	0xaa
	#include <superio/acpi/pnp_config.asl>
	#define SUPERIO_LPC_LDN 0x0C
	#include "resource_helpers.asl"

#ifdef SCH5545_SHOW_KBC
	Device (PS2K)
	{
		Name (_HID, EisaId ("PNP0303"))
		Name (_CID, EisaId ("PNP030B"))
		Method (_STA, 0, NotSerialized)
		{
			Return (DSTA (0xa))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IO (Decode16,
				0x0060,
				0x0060,
				0x00,
				0x01,
				)
			IO (Decode16,
				0x0064,
				0x0064,
				0x00,
				0x01,
				)
			IRQ (Edge, ActiveHigh, Exclusive) {1}
		})
		Method (_PSW, 1, NotSerialized)  // _PSW: Power State Wake
		{
			KBFG = Arg0
		}
		Name (_PRW, Package() { 8, 3 })
	}

	Device (PS2M)
	{
		Name (_HID, EisaId ("PNP0F13"))
		Method (_STA, 0, NotSerialized)
		{
			Return (DSTA (0xe))
		}

		Name (_CRS, ResourceTemplate ()
		{
			IRQ (Edge, ActiveHigh, Exclusive) {12}
		})

		Method (_PSW, 1, NotSerialized)
		{
			MSFG = Arg0
		}

		Name (_PRW, Package() { 8, 3 })
	}

	OperationRegion (IOKP, SystemIO, 0x60, 0x05)
	Field (IOKP, ByteAcc, NoLock, Preserve)
	{
		KP60,	8,
		Offset (0x04),
		KP64,	8
	}

	OperationRegion (KB64, SystemIO, 0x64, One)
	Field (KB64, ByteAcc, NoLock, Preserve)
	{
		,	1,
		KRDY,	1,
		Offset (0x01)
	}
#ifdef SCH5545_RUNTIME_BASE
	/* SIO prepare to sleep */
	Method (SIOS, 1, NotSerialized)
	{
		If ((Arg0 == 0x03) | (Arg0 == One))
		{
			ENTER_CONFIG_MODE (One)
			Local0 = OPT0
			OPT0 = (Local0 | 0x60)
			EXIT_CONFIG_MODE ()
			Local0 = PMS1
			PMS1 = (Local0 | 0x18)
			Local0 = PMES
			PMES = (Local0 | One)

			Local0 = PME1
			If (KBFG)
			{
				PME1 = (Local0 | 0x08)
			}
			Else
			{
				PME1 = (Local0 & 0xF7)
			}

			Local0 = PME1
			If (MSFG)
			{
				PME1 = (Local0 | 0x10)
			}
			Else
			{
				PME1 = (Local0 & 0xEF)
			}

			Local0 = PMEN
			PMEN = (Local0 | One)

			While (KRDY) {}
			KP60 = 0xED
			While (KRDY) {}
			KP60 = Zero
			While (KRDY) {}
			KP60 = 0xF4
			Sleep (One)
		}
	}

	Method (GPKM, 0, NotSerialized)
	{
		Local0 = PME1
		PME1 = (Local0 & 0xE7)
		Local0 = PMEN
		PMEN = (Local0 & 0xFE)
		Local0 = PMS1
		PMS1 = (Local0 & 0x18)
		Local0 = PMES
		PMES = (Local0 & One)
	}

	/* SIO wake method */
	Method (SIOW, 1, NotSerialized)
	{
		PMFG = PMS1
		If (Arg0 == One)
		{
			GPKM ()
			ENTER_CONFIG_MODE (One)
			Local0 = OPT0
			OPT0 = (Local0 & 0x9F)
			EXIT_CONFIG_MODE ()
		}

		If (Arg0 == 0x03)
		{
			GPKM ()
			ENTER_CONFIG_MODE (One)
			Local0 = OPT0
			OPT0 = (Local0 & 0x9F)
			OPT2 |= One
			OPT2 &= 0xFE
			EXIT_CONFIG_MODE ()
		}
	}

	Method (SIOH, 0, NotSerialized)
	{
		If (PMFG & 0x08)
		{
			Notify (PS2K, 0x02) // Device Wake
		}

		If (PMFG & 0x10)
		{
			Notify (PS2M, 0x02) // Device Wake
		}
	}
#endif // SCH5545_RUNTIME_BASE
#endif // SCH5545_SHOW_KBC

#ifdef SCH5545_SHOW_UARTA
#define SUPERIO_UARTA_LDN 7
	Device (UAR1)
	{
		Name (_HID, EisaId ("PNP0501"))
		Name (_UID, SUPERIO_UID(SER, SUPERIO_UARTA_LDN))
		Method (_STA, 0, NotSerialized)
		{
			Return (DSTA (Zero))
		}

		Method (_DIS, 0, NotSerialized)
		{
			DCNT (Zero, Zero)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Return (DCRS (Zero, Zero))
		}

		Method (_SRS, 1, NotSerialized)
		{
			CreateWordField (Arg0, 0x02, IO11)
			CreateWordField (Arg0, 0x09, IRQM)
			ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
			STIO (0x6A, IO11)
			SIRQ (Zero, IRQM)
			EXIT_CONFIG_MODE ()
			DCNT (Zero, One)
		}

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x03F8,
					0x03F8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{4}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x02F8,
					0x02F8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{3}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x03E8,
					0x03E8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{4}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x02E8,
					0x02E8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{3}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			EndDependentFn ()
		})

		Name (_PRW, Package() { 8, 3 })
	}
#endif

#ifdef SCH5545_SHOW_UARTB
#define SUPERIO_UARTB_LDN 8
	Device (UAR1)
	{
		Name (_HID, EisaId ("PNP0501"))
		Name (_UID, SUPERIO_UID(SER, SUPERIO_UARTB_LDN))
		Method (_STA, 0, NotSerialized)
		{
			Return (DSTA (One))
		}

		Method (_DIS, 0, NotSerialized)
		{
			DCNT (One, Zero)
		}

		Method (_CRS, 0, NotSerialized)
		{
			Return (DCRS (One, Zero))
		}

		Method (_SRS, 1, NotSerialized)
		{
			CreateWordField (Arg0, 0x02, IO11)
			CreateWordField (Arg0, 0x09, IRQM)
			ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
			STIO (0x6E, IO11)
			SIRQ (One, IRQM)
			EXIT_CONFIG_MODE ()
			DCNT (One, One)
		}

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x03F8,
					0x03F8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{4}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x02F8,
					0x02F8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{3}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x03E8,
					0x03E8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{4}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			StartDependentFn (0x00, 0x00)
			{
				IO (Decode16,
					0x02E8,
					0x02E8,
					0x01,
					0x08,
					)
				IRQNoFlags ()
					{3}
				DMA (Compatibility, NotBusMaster, Transfer8, )
					{}
			}
			EndDependentFn ()
		})

		Name (_PRW, Package() { 8, 3 })
	}

#endif
}
