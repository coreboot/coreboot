/* SPDX-License-Identifier: GPL-2.0-only */

/* =================== Generic PnP Device =================== */

/*
 * Generic setup for PnP devices.
 *
 * Controlled by the following preprocessor defines:
 *
 * SUPERIO_CHIP_NAME	The name of the Super I/O chip (unique, required)
 * SUPERIO_PNP_HID	The EisaId string that identifies this device (optional)
 * SUPERIO_PNP_LDN	The logical device number on the Super I/O
 *			chip for this device (required)
 * SUPERIO_PNP_DDN	A string literal that identifies the dos device
 *			name (DDN) of this device (e.g. "COM1", optional)
 * SUPERIO_PNP_PM_REG	Identifier of a 1-bit register to power down
 *			the logical device (optional)
 * SUPERIO_PNP_PM_VAL	The value for SUPERIO_PNP_PM_REG to power the logical
 *			device down (required if SUPERIO_PNP_PM_REG is defined)
 * SUPERIO_PNP_PM_LDN	The logical device number to access the PM_REG
 *			bit (required if SUPERIO_PNP_PM_REG is defined)
 * SUPERIO_PNP_IO0	The alignment and length of the first PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * SUPERIO_PNP_IO1	The alignment and length of the second PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * SUPERIO_PNP_IO2	The alignment and length of the third PnP i/o
 *			resource (comma separated, e.g. `0x02, 0x08`,
 *			optional)
 * SUPERIO_PNP_IRQ0	If defined, the first PnP IRQ register is enabled
 * SUPERIO_PNP_IRQ1	If defined, the second PnP IRQ register is enabled
 * SUPERIO_PNP_DMA	If defined, the PnP DMA register is enabled
 */

#include "pnp.asl"

#ifndef SUPERIO_CHIP_NAME
# error "SUPERIO_CHIP_NAME is not defined."
#endif

#ifndef SUPERIO_PNP_LDN
# error "SUPERIO_PNP_LDN is not defined."
#endif

Device (SUPERIO_ID(PN, SUPERIO_PNP_LDN)) {
	#ifdef SUPERIO_PNP_HID
	Name (_HID, EisaId (SUPERIO_PNP_HID))
	#else
	Name (_HID, EisaId ("PNP0c02")) /* TODO: Better fitting EisaId? */
	#endif
	Name (_UID, SUPERIO_UID(PN, SUPERIO_PNP_LDN))
	#ifdef SUPERIO_PNP_DDN
	Name (_DDN, SUPERIO_PNP_DDN)
	#endif

	Method (_STA)
	{
		PNP_GENERIC_STA(SUPERIO_PNP_LDN)
	}

	Method (_DIS)
	{
		PNP_GENERIC_DIS(SUPERIO_PNP_LDN)
	}

#ifdef SUPERIO_PNP_PM_REG
	Method (_PSC) {
		PNP_GENERIC_PSC(SUPERIO_PNP_PM_REG, SUPERIO_PNP_PM_VAL, SUPERIO_PNP_PM_LDN)
	}

	Method (_PS0) {
		PNP_GENERIC_PS0(SUPERIO_PNP_PM_REG, SUPERIO_PNP_PM_VAL, SUPERIO_PNP_PM_LDN)
	}

	Method (_PS3) {
		PNP_GENERIC_PS3(SUPERIO_PNP_PM_REG, SUPERIO_PNP_PM_VAL, SUPERIO_PNP_PM_LDN)
	}
#else
	Method (_PSC) {
		PNP_DEFAULT_PSC
	}
#endif

	Method (_CRS, 0, Serialized)
	{
		Name (CRS, ResourceTemplate () {
#ifdef SUPERIO_PNP_IO0
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO0, IO0)
#endif
#ifdef SUPERIO_PNP_IO1
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO1, IO1)
#endif
#ifdef SUPERIO_PNP_IO2
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO2, IO2)
#endif
#ifdef SUPERIO_PNP_IRQ0
			IRQNoFlags (IR0) {}
#endif
#ifdef SUPERIO_PNP_IRQ1
			IRQNoFlags (IR1) {}
#endif
#ifdef SUPERIO_PNP_DMA
			DMA (Compatibility, NotBusMaster, Transfer8, DM0) {}
#endif
		})
		ENTER_CONFIG_MODE (SUPERIO_PNP_LDN)
#ifdef SUPERIO_PNP_IO0
		  PNP_READ_IO(PNP_IO0, CRS, IO0)
#endif
#ifdef SUPERIO_PNP_IO1
		  PNP_READ_IO(PNP_IO1, CRS, IO1)
#endif
#ifdef SUPERIO_PNP_IO2
		  PNP_READ_IO(PNP_IO2, CRS, IO2)
#endif
#ifdef SUPERIO_PNP_IRQ0
		  PNP_READ_IRQ(PNP_IRQ0, CRS, IR0)
#endif
#ifdef SUPERIO_PNP_IRQ1
		  PNP_READ_IRQ(PNP_IRQ1, CRS, IR1)
#endif
#ifdef SUPERIO_PNP_DMA
		  PNP_READ_DMA(PNP_DMA0, CRS, DM0)
#endif
		EXIT_CONFIG_MODE ()
		Return (CRS)
	}

	Method (_SRS, 1, Serialized)
	{
		Name (TMPL, ResourceTemplate () {
#ifdef SUPERIO_PNP_IO0
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO0, IO0)
#endif
#ifdef SUPERIO_PNP_IO1
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO1, IO1)
#endif
#ifdef SUPERIO_PNP_IO2
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO2, IO2)
#endif
#ifdef SUPERIO_PNP_IRQ0
			IRQNoFlags (IR0) {}
#endif
#ifdef SUPERIO_PNP_IRQ1
			IRQNoFlags (IR1) {}
#endif
#ifdef SUPERIO_PNP_DMA
			DMA (Compatibility, NotBusMaster, Transfer8, DM0) {}
#endif
		})
		ENTER_CONFIG_MODE (SUPERIO_PNP_LDN)
#ifdef SUPERIO_PNP_IO0
		  PNP_WRITE_IO(PNP_IO0, Arg0, IO0)
#endif
#ifdef SUPERIO_PNP_IO1
		  PNP_WRITE_IO(PNP_IO1, Arg0, IO1)
#endif
#ifdef SUPERIO_PNP_IO2
		  PNP_WRITE_IO(PNP_IO2, Arg0, IO2)
#endif
#ifdef SUPERIO_PNP_IRQ0
		  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR0)
#endif
#ifdef SUPERIO_PNP_IRQ1
		  PNP_WRITE_IRQ(PNP_IRQ1, Arg0, IR1)
#endif
#ifdef SUPERIO_PNP_DMA
		  PNP_WRITE_DMA(PNP_DMA0, Arg0, DM0)
#endif
		  PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE ()
	}

	/* This is used for _SRS. Since _DIS only disables the device
	 * without changing the resources this works.
	 */
	Method (_PRS, 0)
	{
		return (_CRS)
	}
}
