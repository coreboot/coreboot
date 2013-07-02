/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
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

/* =================== Generic PnP Device =================== */

/*
 * Generic setup for PnP devices.
 *
 * Controlled by the following preprocessor defines:
 *
 * SUPERIO_CHIP_NAME	The name of the super i/o chip (unique, required)
 * SUPERIO_PNP_LDN	The logical device number on the super i/o
 *			chip for this device (required)
 * SUPERIO_PNP_DDN	A string literal that identifies the dos device
 *                      name (DDN) of this device (e.g. "COM1", optional)
 * SUPERIO_PNP_PM_REG	Identifier of a 1-bit register to power down
 *			the logical device (optional)
 * SUPERIO_PNP_PM_VAL	The value for SUPERIO_PNP_PM_REG to power the logical
 *			device down (required if SUPERIO_PNP_PM_REG is defined)
 * SUPERIO_PNP_PM_LDN	The logical device number to access the PM_REG
 *			bit (required if SUPERIO_PNP_PM_REG is defined)
 * SUPERIO_PNP_IO0	The alignment and length of the first PnP i/o
 *			resource (comma seperated, e.g. `0x02, 0x08`,
 *			optional)
 * SUPERIO_PNP_IO1	The alignment and length of the second PnP i/o
 *			resource (comma seperated, e.g. `0x02, 0x08`,
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
	Name (_HID, EisaId ("PNP0c02")) /* TODO: Better fitting EisaId? */
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

	Method (_PS1) {
		PNP_GENERIC_PS1(SUPERIO_PNP_PM_REG, SUPERIO_PNP_PM_VAL, SUPERIO_PNP_PM_LDN)
	}
#else
	Method (_PSC) {
		PNP_DEFAULT_PSC
	}
#endif

	Method (_CRS)
	{
		Name (CRS, ResourceTemplate () {
#ifdef SUPERIO_PNP_IO0
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO0, IO0)
#endif
#ifdef SUPERIO_PNP_IO1
			IO (Decode16, 0x0000, 0x0000, SUPERIO_PNP_IO1, IO1)
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
#ifdef SUPERIO_PNP_IRQ0
		  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR0)
#endif
#ifdef SUPERIO_PNP_IRQ1
		  PNP_WRITE_IRQ(PNP_IRQ1, Arg0, IR1)
#endif
#ifdef SUPERIO_PNP_DMA
		  PNP_WRITE_DMA(PNP_DMA0, Arg0, DM0)
#endif
		  Store (One, PNP_DEVICE_ACTIVE)
		EXIT_CONFIG_MODE ()
	}
}
