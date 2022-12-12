/* SPDX-License-Identifier: GPL-2.0-only */

/* ========================== UART ========================== */

/*
 * Generic setup for 16550A compatible UARTs.
 *
 * Controlled by the following preprocessor defines:
 *
 * SUPERIO_CHIP_NAME	The name of the Super I/O chip (unique, required)
 * SUPERIO_UART_LDN	The logical device number on the Super I/O
 *			chip for this UART (required)
 * SUPERIO_UART_DDN	A string literal that identifies the dos device
 *			name (DDN) of this uart (e.g. "COM1", optional)
 * SUPERIO_UART_PM_REG	Identifier of a 1-bit register to power down
 *			the UART (optional)
 * SUPERIO_UART_PM_VAL	The value for SUPERIO_UART_PM_REG to power the logical
 *			device down (required if SUPERIO_UART_PM_REG is defined)
 * SUPERIO_UART_PM_LDN	The logical device number to access the PM_REG
 *			bit (required if SUPERIO_UART_PM_REG is defined)
 */

#include "pnp.asl"

#ifndef SUPERIO_CHIP_NAME
# error "SUPERIO_CHIP_NAME is not defined."
#endif

#ifndef SUPERIO_UART_LDN
# error "SUPERIO_UART_LDN is not defined."
#endif

Device (SUPERIO_ID(SER, SUPERIO_UART_LDN)) {
	Name (_HID, EisaId ("PNP0501"))
	Name (_UID, SUPERIO_UID(SER, SUPERIO_UART_LDN))
	#ifdef SUPERIO_UART_DDN
	Name (_DDN, SUPERIO_UART_DDN)
	#endif

	Method (_STA)
	{
		PNP_GENERIC_STA(SUPERIO_UART_LDN)
	}

	Method (_DIS)
	{
		PNP_GENERIC_DIS(SUPERIO_UART_LDN)
	}

#ifdef SUPERIO_UART_PM_REG
	Method (_PSC) {
		PNP_GENERIC_PSC(SUPERIO_UART_PM_REG, SUPERIO_UART_PM_VAL, SUPERIO_UART_PM_LDN)
	}

	Method (_PS0) {
		PNP_GENERIC_PS0(SUPERIO_UART_PM_REG, SUPERIO_UART_PM_VAL, SUPERIO_UART_PM_LDN)
	}

	Method (_PS3) {
		PNP_GENERIC_PS3(SUPERIO_UART_PM_REG, SUPERIO_UART_PM_VAL, SUPERIO_UART_PM_LDN)
	}
#else
	Method (_PSC) {
		PNP_DEFAULT_PSC
	}
#endif

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
		  PNP_DEVICE_ACTIVE = One
		EXIT_CONFIG_MODE ()
	}
}
