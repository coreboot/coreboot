/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a southbridge ASL block and it will
 * expose the W83977TF/EF SuperIO and some of its functionality.
 *
 * Adapted from winbond/w83627dhg.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices and controlling power
 * saving mode on logical devices or the whole chip.
 *
 * Controllable through preprocessor defines:
 * SUPERIO_PNP_BASE	I/O address of the first PnP configuration register
 * SUPERIO_SHOW_UARTA	If defined, UARTA will be exposed.
 * SUPERIO_SHOW_UARTB	If defined, UARTB will be exposed.
 * SUPERIO_SHOW_FDC	If defined, floppy controller will be exposed.
 * SUPERIO_SHOW_LPT	If defined, parallel port will be exposed.
 */

#define SUPERIO_CHIP_NAME W83977TF
#include <superio/acpi/pnp.asl>
#include <superio/winbond/w83977tf/w83977tf.h>

/* Mutex for accesses to the configuration ports */

Mutex(CRMX, 1)

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
	Offset (0x20),
	DID,			8, /* Device ID: TF=0x97, EF=0x52 */
	DREV,			8, /* Device revision */
	FDPW,			1,
	,			2,
	PRPW,			1,
	UAPW,			1, /* UART A Power Down */
	UBPW,			1, /* UART B Power Down */
	Offset (0x23),
	IPD,			1, /* Immediate Chip Power Down */
	Offset (0x30),
	PNP_DEVICE_ACTIVE,	8,
	Offset (0x60),
	PNP_IO0_HIGH_BYTE,	8,
	PNP_IO0_LOW_BYTE,	8,
	PNP_IO1_HIGH_BYTE,	8,
	PNP_IO1_LOW_BYTE,	8,
	PNP_IO2_HIGH_BYTE,	8,
	PNP_IO2_LOW_BYTE,	8,
	Offset (0x70),
	PNP_IRQ0,		8,
	Offset (0x72),
	PNP_IRQ1,		8,
	Offset (0x74),
	PNP_DMA0,		8,
	/* MSWK and KBWK are for ACPI logical device, LDN 0xA */
	Offset (0xE0),
	MSWK,			8,
	Offset (0xE4),
	KBWK,			8,
	Offset (0xF0),
	OPT1,			8,
	OPT2,			8
}

#define PNP_ENTER_MAGIC_1ST	0x87
#define PNP_ENTER_MAGIC_2ND	0x87
#define PNP_EXIT_MAGIC_1ST	0xaa
#include <superio/acpi/pnp_config.asl>

/* PM: indicate IPD (Immediate Power Down) bit state as D0/D3 */
Method (_PSC) {
	ENTER_CONFIG_MODE (PNP_NO_LDN_CHANGE)
	Local0 = IPD
	EXIT_CONFIG_MODE ()
	If (Local0) { Return (3) }
	Else { Return (0) }
}

#ifdef SUPERIO_SHOW_FDC
Device (FDC0)
{
	Name (_HID, EisaId ("PNP0700")) // _HID: Hardware ID
	Method (_STA, 0, NotSerialized) // _STA: Status
	{
		PNP_GENERIC_STA(W83977TF_FDC)
	}

	Method (_DIS, 0, NotSerialized) // _DIS: Disable Device
	{
		PNP_GENERIC_DIS(W83977TF_FDC)
	}

	Method (_CRS, 0)
	{
		Name (BUF0, ResourceTemplate ()
		{
			IO (Decode16, 0x03F2, 0x03F2, 0x00, 0x04, IO0)
			IO (Decode16, 0x03F7, 0x03F7, 0x00, 0x01, IO1)
			IRQ (Edge, ActiveHigh, Exclusive, Y08) {6}
			DMA (Compatibility, NotBusMaster, Transfer8, Y09) {2}
		})
		CreateWordField (BUF0, IO1._MIN, IO1I)
		CreateWordField (BUF0, IO1._MAX, IO1A)

		ENTER_CONFIG_MODE(W83977TF_FDC)
		/* OEM BIOS does not report actual programmed base port */
		/* xx0 is read from superio */
		PNP_READ_IO(PNP_IO0, BUF0, IO0)
		/* Store xx7 range first so the value isn't overwritten
		 * for below */
		IO1I += 7
		IO1A = IO1I
		/* Store xx2 range */
		IO0I += 2
		IO0A = IO0I
		/* End OEM BIOS deficiency */
		PNP_READ_IRQ(PNP_IRQ0, BUF0, Y08)
		PNP_READ_DMA(PNP_DMA0, BUF0, Y09)

		EXIT_CONFIG_MODE()
		Return (BUF0)
	}

	Name (_PRS, ResourceTemplate ()
	{
		IO (Decode16, 0x03F2, 0x03F2, 0x00, 0x04, )
		IO (Decode16, 0x03F7, 0x03F7, 0x00, 0x01, )
		IRQ (Edge, ActiveHigh, Exclusive, ) {6}
		DMA (Compatibility, NotBusMaster, Transfer8, ) {2}
	})

	Method (_SRS, 1, NotSerialized)
	{
		CreateByteField (Arg0, 0x02, IOLO)
		CreateByteField (Arg0, 0x03, IOHI)
		CreateWordField (Arg0, 0x11, IRQW)
		CreateByteField (Arg0, 0x15, DMAV)
		ENTER_CONFIG_MODE(W83977TF_FDC)
		/* FDC base port on 8-byte boundary. */
		PNP_IO0_LOW_BYTE = IOLO & 0xF8
		PNP_IO0_HIGH_BYTE = IOHI
		PNP_IRQ0 = FindSetLeftBit (IRQW) - 1
		PNP_DMA0 = FindSetLeftBit (DMAV) - 1
		PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE()
	}
}
#endif

#ifdef SUPERIO_SHOW_LPT
/* Standard LPT Parallel Port */
Device (LPT)
{
	Name (_HID, EisaId ("PNP0400"))
	Method (_STA, 0, NotSerialized)
	{
		ENTER_CONFIG_MODE(W83977TF_PP)
		Local0 = OPT1 & 0x02
		If (IO0H || IO0L)
		{
			/* Report device not present if ECP is enabled */
			If (Local0 == 0x02)
			{
				EXIT_CONFIG_MODE()
				Return (0x00)
			}
			ElseIf (PNP_DEVICE_ACTIVE)
			{
				EXIT_CONFIG_MODE()
				Return (0x0F)
			}
			Else
			{
				EXIT_CONFIG_MODE()
				Return (0x0D)
			}
		}

		EXIT_CONFIG_MODE()
		Return (0)
	}

	Method (_DIS, 0, NotSerialized)
	{
		PNP_GENERIC_DIS(W83977TF_PP)
	}

	Method (_CRS, 0, NotSerialized)
	{
		Name (BUF5, ResourceTemplate ()
		{
			IO (Decode16,0x0378,0x0378,0x00,0x04,Y0A)
			IRQ (Edge, ActiveHigh, Exclusive, Y0B)
				{7}
		})
		ENTER_CONFIG_MODE(W83977TF_PP)
		PNP_READ_IO(PNP_IO0,BUF5,Y0A)
		PNP_READ_IRQ(PNP_IRQ0,BUF5,Y0B)
		EXIT_CONFIG_MODE()
		Return (BUF5)
	}

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0x01, 0x01)
		{
			IO (Decode16,0x0378,0x0378,0x00,0x08,)
			IRQ (Edge, ActiveHigh, Exclusive, )
				{5,7}
		}
		StartDependentFn (0x01, 0x01)
		{
			IO (Decode16,0x0278,0x0278,0x00,0x08,)
			IRQ (Edge, ActiveHigh, Exclusive, )
				{5,7}
		}
		StartDependentFn (0x01, 0x01)
		{
			IO (Decode16,0x03BC,0x03BC,0x00,0x04,)
			IRQ (Edge, ActiveHigh, Exclusive, )
				{5,7}
		}
		EndDependentFn ()
	})

	Method (_SRS, 1, NotSerialized)
	{
		CreateByteField (Arg0, 0x02, IOLO)
		CreateByteField (Arg0, 0x03, IOHI)
		CreateWordField (Arg0, 0x09, IRQW)
		ENTER_CONFIG_MODE(W83977TF_PP)
		PNP_IO0_LOW_BYTE = IOLO
		PNP_IO0_HIGH_BYTE = IOHI
		PNP_IRQ0 = FindSetLeftBit (IRQW) - 1
		PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE()
	}
}

/* ECP Parallel Port */
Device (ECP)
{
	Name (_HID, EisaId ("PNP0401"))
	Method (_STA, 0, NotSerialized)
	{
		ENTER_CONFIG_MODE(W83977TF_PP)
		Local0 = OPT1 & 0x02
		If (IO0H || IO0L)
		{
			If (Local0 == 0x02)
			{
				If (PNP_DEVICE_ACTIVE)
				{
					EXIT_CONFIG_MODE()
					Return (0x0F)
				}
				Else
				{
					EXIT_CONFIG_MODE()
					Return (0x05)
				}
			}
		}

		EXIT_CONFIG_MODE()
		Return (0x00)
	}

	Method (_DIS, 0, NotSerialized)
	{
		PNP_GENERIC_DIS(W83977TF_PP)
	}

	Method (_CRS, 0, NotSerialized)
	{
		Name (BUF6, ResourceTemplate ()
		{
			IO (Decode16,0x0378,0x0378,0,4,IO0)
			IO (Decode16,0x0778,0x0778,0,4,IO1)
			IRQ (Edge, ActiveHigh, Exclusive, IR1) {7}
			DMA (Compatibility, NotBusMaster, Transfer8, Y0F) {1}
		})
		ENTER_CONFIG_MODE(W83977TF_PP)
		PNP_READ_IO(PNP_IO0, BUF6, IO0)
		PNP_READ_IO(PNP_IO0, BUF6, IO1)
		PNP_READ_IRQ(PNP_IRQ0, BUF6, IR1)
		PNP_READ_DMA(PNP_DMA0, BUF6, Y0F)

		/* Report a second port range that is 0x400 above base port. */
		CreateByteField (BUF6, 0x0B, I2HI)
		CreateByteField (BUF6, 0x0D, I2RH)
		Add (I2HI, 0x04, I2RH)
		Add (I2HI, 0x04, I2HI)
		EXIT_CONFIG_MODE()
		Return (BUF6)
	}

	Name (_PRS, ResourceTemplate () // _PRS: Possible Resource Settings
	{
		StartDependentFn (0x01, 0x01)
		{
			IO (Decode16,0x0378,0x0378,0,4,)
			IO (Decode16,0x0778,0x0778,0,4,)
			IRQ (Edge, ActiveHigh, Exclusive, ) {5,7}
			DMA (Compatibility, NotBusMaster, Transfer8, ) {0,1,3}
		}
		StartDependentFn (0x01, 0x01)
		{
			IO (Decode16,0x0278,0x0278,0,4,)
			IO (Decode16,0x0678,0x0678,0,4,)
			IRQ (Edge, ActiveHigh, Exclusive, ) {5,7}
			DMA (Compatibility, NotBusMaster, Transfer8, ) {0,1,3}
		}
		StartDependentFn (0x01, 0x01)
		{
			IO (Decode16,0x03BC,0x03BC,0,4,)
			IO (Decode16,0x07BC,0x07BC,0,4,)
			IRQ (Edge, ActiveHigh, Exclusive, ) {5,7}
			DMA (Compatibility, NotBusMaster, Transfer8, ) {0,1,3}
		}
		EndDependentFn ()
	})

	Method (_SRS, 1, NotSerialized) // _SRS: Set Resource Settings
	{
		CreateByteField (Arg0, 0x02, IOLO)
		CreateByteField (Arg0, 0x03, IOHI)
		CreateWordField (Arg0, 0x11, IRQW)
		CreateByteField (Arg0, 0x15, DMAC)

		ENTER_CONFIG_MODE(W83977TF_PP)
		PNP_IO0_LOW_BYTE = IOLO
		PNP_IO0_HIGH_BYTE = IOHI
		PNP_IRQ0 = FindSetLeftBit (IRQW) - 1
		PNP_DMA0 = FindSetLeftBit (DMAC) - 1
		PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE()
	}
}
#endif

#define SUPERIO_UART_PM_VAL 0
#define SUPERIO_UART_PM_LDN PNP_NO_LDN_CHANGE

#ifdef SUPERIO_SHOW_UARTA
	#define SUPERIO_UART_LDN W83977TF_SP1
	#define SUPERIO_UART_PM_REG UAPW
	#include <superio/acpi/pnp_uart.asl>
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_PM_REG
#endif

#ifdef SUPERIO_SHOW_UARTB
	#define SUPERIO_UART_LDN W83977TF_SP2
	#define SUPERIO_UART_PM_REG UBPW
	#include <superio/acpi/pnp_uart.asl>
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_PM_REG
#endif

/*
 * TODO: IrDA device;
 * EF=LDN 3 aka UARTB
 * Some revisions of TF=LDN 6
 */

#define SUPERIO_KBC_LDN W83977TF_KBC
#define SUPERIO_KBC_PS2M /* Mouse shares same LDN */
#include <superio/acpi/pnp_kbc.asl>
