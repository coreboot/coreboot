/* SPDX-License-Identifier: GPL-2.0-only */

/* =================== Keyboard Controller ================== */

/*
 * Just uses the default i/o ports 0x60/0x64, irq 1 (and 12 for PS/2
 * mouse). Do we have any system that needs this configurable?
 *
 * Controlled by the following preprocessor defines:
 *
 * SUPERIO_CHIP_NAME	The name of the Super I/O chip (unique, required)
 * SUPERIO_KBC_LDN	The logical device number on the Super I/O
 *			chip for this keyboard controller (required)
 * SUPERIO_KBC_PS2KID	The name of PS/2 keyboard controller device.
 *			If not defined, defaults to KBD# where # is
 *			SUPERIO_KBC_LDN.
 * SUPERIO_KBC_PS2M	If defined, PS/2 mouse support is included in
 *			the KBC_LDN. Mouse irq is set at IRQ1 of the
 *			KBC_LDN.
 * SUPERIO_KBC_PS2LDN	If defined, specifies a second LDN to configure
 *			PS/2 mouse support. Mouse irq is set at IRQ0 of
 *			this LDN.
 * SUPERIO_KBC_PS2MID	The name of PS/2 mouse support device.
 *			If not defined, defaults to PS2# where # is
 *			KBC_LDN or KBC_PS2LDN as appropriate.
 * SUPERIO_KBC_PS2M and SUPERIO_KBC_PS2LDN are mutually exclusive.
 */

#include "pnp.asl"

#ifndef SUPERIO_CHIP_NAME
# error "SUPERIO_CHIP_NAME is not defined."
#endif

#ifndef SUPERIO_KBC_LDN
# error "SUPERIO_KBC_LDN is not defined."
#endif

#if defined(SUPERIO_KBC_PS2M) && defined(SUPERIO_KBC_PS2LDN)
# error "SUPERIO_KBC_PS2M and SUPERIO_KBC_PS2LDN are mutually exclusive."
#endif

#ifndef SUPERIO_KBC_PS2KID
#define SUPERIO_KBC_PS2KID SUPERIO_ID(KBD, SUPERIO_KBC_LDN)
#endif

#ifndef SUPERIO_KBC_PS2MID
#ifdef SUPERIO_KBC_PS2LDN
#define SUPERIO_KBC_PS2MID SUPERIO_ID(PS2, SUPERIO_KBC_PS2LDN)
#else
#define SUPERIO_KBC_PS2MID SUPERIO_ID(PS2, SUPERIO_KBC_LDN)
#endif /* SUPERIO_KBC_PS2LDN */
#endif

Device (SUPERIO_KBC_PS2KID) {
	Name (_HID, EisaId (CONFIG_PS2K_EISAID))
	Name (_UID, SUPERIO_UID(KBD, SUPERIO_KBC_LDN))

	Method (_STA)
	{
		PNP_GENERIC_STA(SUPERIO_KBC_LDN)
	}

	Method (_DIS)
	{
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_DEVICE_ACTIVE = 0
		EXIT_CONFIG_MODE ()
		#if defined(SUPERIO_KBC_PS2LDN) || defined(SUPERIO_KBC_PS2M)
		Notify (SUPERIO_KBC_PS2MID, 1)
		#endif
	}

	Method (_PSC) {
		PNP_DEFAULT_PSC
	}

	Method (_CRS, 0, Serialized)
	{
		Name (CRS, ResourceTemplate () {
			IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
			IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO1)
			IRQNoFlags (IR0) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_READ_IO(PNP_IO0, CRS, IO0)
		  PNP_READ_IO(PNP_IO1, CRS, IO1)
		  PNP_READ_IRQ(PNP_IRQ0, CRS, IR0)
		EXIT_CONFIG_MODE ()
		Return (CRS)
	}

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0,0) {
			IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
			IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
			IRQNoFlags () {1}
		}
		EndDependentFn()
	})

	Method (_SRS, 1, Serialized)
	{
		Name (TMPL, ResourceTemplate () {
			IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO0)
			IO (Decode16, 0x0000, 0x0000, 0x01, 0x01, IO1)
			IRQNoFlags (IR0) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_WRITE_IO(PNP_IO0, Arg0, IO0)
		  PNP_WRITE_IO(PNP_IO1, Arg0, IO1)
		  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR0)
		  PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE ()
		#if defined(SUPERIO_KBC_PS2LDN) || defined(SUPERIO_KBC_PS2M)
		Notify (SUPERIO_KBC_PS2MID, 1)
		#endif
	}
}

#if defined(SUPERIO_KBC_PS2M)
Device (SUPERIO_KBC_PS2MID) {
	Name (_HID, EisaId (CONFIG_PS2M_EISAID))
	Name (_UID, SUPERIO_UID(PS2, SUPERIO_KBC_LDN))

	Method (_STA)
	{
		Return (^^SUPERIO_KBC_PS2KID._STA ())
	}

	Method (_PSC) {
		Return (^^SUPERIO_KBC_PS2KID._PSC ())
	}

	Method (_CRS, 0, Serialized)
	{
		Name (CRS, ResourceTemplate () {
			IRQNoFlags (IR1) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_READ_IRQ(PNP_IRQ1, CRS, IR1)
		EXIT_CONFIG_MODE ()
		Return (CRS)
	}

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0,0) {
			IRQNoFlags () {12}
		}
		EndDependentFn()
	})

	Method (_SRS, 1, Serialized)
	{
		Name (TMPL, ResourceTemplate () {
			IRQNoFlags (IR1) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_LDN)
		  PNP_WRITE_IRQ(PNP_IRQ1, Arg0, IR1)
		EXIT_CONFIG_MODE ()
	}
}
#elif defined(SUPERIO_KBC_PS2LDN)
Device (SUPERIO_KBC_PS2MID) {
	Name (_HID, EisaId (CONFIG_PS2M_EISAID))
	Name (_UID, SUPERIO_UID(PS2, SUPERIO_KBC_PS2LDN))

	Method (_STA)
	{
		Local0 = ^^SUPERIO_KBC_PS2KID._STA ()
		If (Local0 == DEVICE_PRESENT_ACTIVE) {
			PNP_GENERIC_STA(SUPERIO_KBC_PS2LDN)
		} Else {
			Return (Local0)
		}
	}

	Method (_DIS)
	{
		ENTER_CONFIG_MODE (SUPERIO_KBC_PS2LDN)
		  PNP_DEVICE_ACTIVE = 0
		EXIT_CONFIG_MODE ()
	}

	Method (_PSC) {
		PNP_DEFAULT_PSC
	}

	Method (_CRS, 0, Serialized)
	{
		Name (CRS, ResourceTemplate () {
			IRQNoFlags (IR1) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_PS2LDN)
		  PNP_READ_IRQ(PNP_IRQ0, CRS, IR1)
		EXIT_CONFIG_MODE ()
		Return (CRS)
	}

	Name (_PRS, ResourceTemplate ()
	{
		StartDependentFn (0,0) {
			IRQNoFlags () {12}
		}
		EndDependentFn()
	})

	Method (_SRS, 1, Serialized)
	{
		Name (TMPL, ResourceTemplate () {
			IRQNoFlags (IR1) {}
		})
		ENTER_CONFIG_MODE (SUPERIO_KBC_PS2LDN)
		  PNP_WRITE_IRQ(PNP_IRQ0, Arg0, IR1)
		  PNP_DEVICE_ACTIVE = 1
		EXIT_CONFIG_MODE ()
	}
}
#endif
