/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include this file into a mainboard's DSDT _SB device tree and it will
 * expose the F81803A SuperIO and some of its functionality.
 *
 * It allows the change of IO ports, IRQs and DMA settings on logical
 * devices, disabling and reenabling logical devices and controlling power
 * saving mode on logical devices or the whole chip.
 *
 * LDN					State
 * 0x1 UARTA			Implemented, partially tested
 * 0x2 UARTB			Implemented, partially tested
 * 0x4 HWM			Not implemented
 * 0x5 KBC			Not implemented
 * 0x6 GPIO6			Not implemented
 * 0x7 WDT0&PLED		Not implemented
 * 0xa ACPI/PME/ERP		Partially implemented
 *
 * Controllable through preprocessor defines:
 * SUPERIO_DEV		Device identifier for this SIO (e.g. SIO0)
 * SUPERIO_PNP_BASE	I/o address of the first PnP configuration register
 * F81803A_SHOW_UARTA	If defined, UARTA will be exposed.
 * F81803A_SHOW_UARTB	If defined, UARTB will be exposed.
 * F81803A_SHOW_HWMON	If defined, the hardware monitor will be exposed.
 * F81803A_SHOW_PME	If defined, the PME/EARP/ACPI  will be exposed.
 *
 * Known issue:
 * Do not enable UARTA and UARTB simultaneously, Linux boot will crash.
 * Select one or the other.
 */
#undef SUPERIO_CHIP_NAME
#define SUPERIO_CHIP_NAME F81803A
#include <superio/acpi/pnp.asl>

#undef PNP_DEFAULT_PSC
#define PNP_DEFAULT_PSC Return (0) /* no power management */

Device(SUPERIO_DEV) {
	Name (_HID, EisaId("PNP0A05"))
	Name (_STR, Unicode("Fintek F81803A Super I/O"))
	Name (_UID, SUPERIO_UID(SUPERIO_DEV,))

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
		Offset (0x30),
		PNP_DEVICE_ACTIVE,	1, /* Logical device activation */
		Offset (0x60),
		PNP_IO0_HIGH_BYTE,	8, /* First I/O port base - high byte */
		PNP_IO0_LOW_BYTE,	8, /* First I/O port base - low byte */
		Offset (0x62),
		PNP_IO1_HIGH_BYTE,	8, /* Second I/O port base - high byte */
		PNP_IO1_LOW_BYTE,	8, /* Second I/O port base - low byte */
		Offset (0x70),
		PNP_IRQ0,			8, /* First IRQ */
		offset(0xFB),
		APC5,				8, /* PME ACPI Control Register 5 */
	}

	Method(_CRS)
	{
		/* Announce the used i/o ports to the OS */
		Return (ResourceTemplate () {
			IO (Decode16, SUPERIO_PNP_BASE, SUPERIO_PNP_BASE, 0x01, 0x02)
		})
	}

	#undef PNP_ENTER_MAGIC_1ST
	#undef PNP_ENTER_MAGIC_2ND
	#undef PNP_ENTER_MAGIC_3RD
	#undef PNP_ENTER_MAGIC_4TH
	#undef PNP_EXIT_MAGIC_1ST
	#undef PNP_EXIT_SPECIAL_REG
	#undef PNP_EXIT_SPECIAL_VAL
	#define PNP_ENTER_MAGIC_1ST	0x87
	#define PNP_ENTER_MAGIC_2ND	0x87
	#define PNP_EXIT_MAGIC_1ST	0xaa
	#include <superio/acpi/pnp_config.asl>

#ifdef F81803A_SHOW_UARTA
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 1

	Device (SUPERIO_ID(SER, SUPERIO_UART_LDN)) {
		Name (_HID, EisaId ("PNP0501"))
		Name (_UID, SUPERIO_UID(SER, SUPERIO_UART_LDN))

		Method (_STA)
		{
			PNP_GENERIC_STA(SUPERIO_UART_LDN)
		}

		Method (_CRS, 0, Serialized)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x08, 0x08, IO0)
				IRQNoFlags (IR0) {}
			})
			ENTER_CONFIG_MODE (SUPERIO_UART_LDN)
			  PNP_READ_IO(PNP_IO0, CRS, IO0)
			  PNP_READ_IRQ(PNP_IRQ0, CRS, IR0)
			EXIT_CONFIG_MODE ()
			Return (CRS)
		}

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0,0) {
				IO (Decode16, 0x03f8, 0x03f8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (0,0) {
				IO (Decode16, 0x02f8, 0x02f8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (1,0) {
				IO (Decode16, 0x03e8, 0x03e8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (1,0) {
				IO (Decode16, 0x02e8, 0x02e8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (2,0) {
				IO (Decode16, 0x0100, 0x0ff8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			EndDependentFn()
		})

		Method (_SRS, 1, Serialized)
		{
			Name (TMPL, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x00, 0x00, IO0)
				IRQNoFlags (IR0) {}
			})
			ENTER_CONFIG_MODE (SUPERIO_UART_LDN)
			  PNP_WRITE_IO(PNP_IO0, Arg0, IO0)
			  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR0)
			  PNP_DEVICE_ACTIVE = 1
			EXIT_CONFIG_MODE ()
		}
	}
#endif

#ifdef F81803A_SHOW_UARTB
	#undef SUPERIO_UART_LDN
	#undef SUPERIO_UART_PM_REG
	#undef SUPERIO_UART_PM_VAL
	#undef SUPERIO_UART_PM_LDN
	#define SUPERIO_UART_LDN 2

	Device (SUPERIO_ID(SER, SUPERIO_UART_LDN)) {
		Name (_HID, EisaId ("PNP0501"))
		Name (_UID, SUPERIO_UID(SER, SUPERIO_UART_LDN))

		Method (_STA)
		{
			PNP_GENERIC_STA(SUPERIO_UART_LDN)
		}

		Method (_CRS, 0, Serialized)
		{
			Name (CRS, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x08, 0x08, IO0)
				IRQNoFlags (IR0) {}
			})
			ENTER_CONFIG_MODE (SUPERIO_UART_LDN)
			  PNP_READ_IO(PNP_IO0, CRS, IO0)
			  PNP_READ_IRQ(PNP_IRQ0, CRS, IR0)
			EXIT_CONFIG_MODE ()
			Return (CRS)
		}

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0,0) {
				IO (Decode16, 0x03f8, 0x03f8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (0,0) {
				IO (Decode16, 0x02f8, 0x02f8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (1,0) {
				IO (Decode16, 0x03e8, 0x03e8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (1,0) {
				IO (Decode16, 0x02e8, 0x02e8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			StartDependentFn (2,0) {
				IO (Decode16, 0x0100, 0x0ff8, 0x08, 0x08)
				IRQNoFlags () {3,4,5,7,9,10,11,12}
			}
			EndDependentFn()
		})

		Method (_SRS, 1, Serialized)
		{
			Name (TMPL, ResourceTemplate () {
				IO (Decode16, 0x0000, 0x0000, 0x00, 0x00, IO0)
				IRQNoFlags (IR0) {}
			})
			ENTER_CONFIG_MODE (SUPERIO_UART_LDN)
			  PNP_WRITE_IO(PNP_IO0, Arg0, IO0)
			  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR0)
			  PNP_DEVICE_ACTIVE = 1
			EXIT_CONFIG_MODE ()
		}
	}
#endif

#ifdef F81803A_SHOW_PME
	#undef SUPERIO_PME_LDN
	#define SUPERIO_PME_LDN 0x0A

	OperationRegion(APCx, SystemIO, APC5, 0x01)
	Field(APCx, ByteAcc, Nolock, Preserve) /* bits in PME ACPI CONTROL Reg 5*/
	{
		    , 7, /*Control Reg 5 */
		PSIN, 1  /* PSIN_FLAG */
	}

	/* routine to clear PSIN_FLAG in ACPI_CONTROL_REG_5 of SIO */
	Method(CPSI, 0, Serialized)
	{
		/* DBG0("SIO CPSI") */
		ENTER_CONFIG_MODE(SUPERIO_PME_LDN)
		PSIN = 1
		EXIT_CONFIG_MODE()
	}
#endif
}
