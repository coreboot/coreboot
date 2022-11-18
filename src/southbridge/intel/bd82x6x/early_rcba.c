/* SPDX-License-Identifier: GPL-2.0-only */

#include "pch.h"

void southbridge_configure_default_intmap(void)
{
	/*
	 * For the PCH internal PCI functions, provide a reasonable
	 * default IRQ mapping that utilizes only PIRQ A to D. Higher
	 * PIRQs are sometimes used for other on-board chips that
	 * require an edge triggered interrupt which is not shareable.
	 */

	/*
	 * We use a linear mapping for the pin numbers. They are not
	 * physical pins, and thus, have no relation between the dif-
	 * ferent devices. Only rule we must obey is that a single-
	 * function device has to use pin A.
	 */
	RCBA32(D31IP) = (INTD << D31IP_TTIP) | (INTC << D31IP_SIP2) |
			(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTD << D28IP_P8IP) | (INTC << D28IP_P7IP) |
			(INTB << D28IP_P6IP) | (INTA << D28IP_P5IP) |
			(INTD << D28IP_P4IP) | (INTC << D28IP_P3IP) |
			(INTB << D28IP_P2IP) | (INTA << D28IP_P1IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (INTA << D25IP_LIP);
	RCBA32(D22IP) = (INTA << D22IP_MEI1IP);
	RCBA32(D20IP) = (INTA << D20IP_XHCIIP);

	/*
	 * For the PIRQ allocation the following was taken into
	 * account:
	 *   o Interrupts of the PCIe root ports are only about
	 *     events at the ports, not downstream devices. So we
	 *     don't expect many interrupts there and ignore them.
	 *   o We don't expect to talk constantly to the ME either
	 *     so ignore that, too. Same for SMBus and the thermal
	 *     device.
	 *   o Second SATA interface is only used in non-AHCI mode
	 *     so unlikely to coexist with modern interfaces (e.g.
	 *     xHCI).
	 *   o An OS that knows USB3 will likely also know how to
	 *     use MSI.
	 *
	 * The functions that might matter first:
	 *
	 * D31IP_SIP	SATA 1	-> PIRQ A (MSI capable in AHCI mode)
	 * D31IP_SIP2	SATA 2	-> PIRQ B
	 * D29IP_E1P	EHCI 1	-> PIRQ C
	 * D27IP_ZIP	HDA	-> PIRQ D (MSI capable)
	 * D26IP_E2P	EHCI 2	-> PIRQ D
	 * D25IP_LIP	GbE	-> PIRQ B (MSI capable)
	 * D20IP_XHCIIP	xHCI	-> PIRQ B (MSI capable)
	 *
	 * D31IP_TTIP	Thermal	-> PIRQ B
	 * D31IP_SMIP	SMBUS	-> PIRQ A
	 * D28IP_*	PCIe RP	-> PIRQ A-D (MSI capable)
	 * D22IP_MEI1IP	ME	-> PIRQ A (MSI capable)
	 *
	 * Note, CPU-integrated functions seem to always use PIRQ A.
	 */
#define _none 0
	DIR_ROUTE(D31IR, PIRQA, PIRQA, PIRQB, PIRQB);
	DIR_ROUTE(D29IR, PIRQC, _none, _none, _none);
	DIR_ROUTE(D28IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D27IR, PIRQD, _none, _none, _none);
	DIR_ROUTE(D26IR, PIRQD, _none, _none, _none);
	DIR_ROUTE(D25IR, PIRQB, _none, _none, _none);
	DIR_ROUTE(D22IR, PIRQA, _none, _none, _none);
	DIR_ROUTE(D20IR, PIRQB, _none, _none, _none);
#undef _none

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void)RCBA16(OIC);
}

void southbridge_rcba_config(void)
{
	RCBA32(FD) = PCH_DISABLE_ALWAYS;
}
