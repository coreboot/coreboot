/* SPDX-License-Identifier: GPL-2.0-only */

#include "i82801hx.h"

void southbridge_configure_default_intmap(void)
{
	/*
	 * For the ICH8 internal PCI functions, provide a reasonable
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
	RCBA32(D31IP) = (INTD << D31IP_TTIP) | (INTC << D31IP_SMIP) | (INTB << D31IP_SIP) |
			(INTA << D31IP_IDEP);
	RCBA32(D30IP) = (INTA << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P) | (INTC << D29IP_U3P) | (INTB << D29IP_U2P) |
			(INTA << D29IP_U1P);
	RCBA32(D28IP) = (INTB << D28IP_P6IP) | (INTA << D28IP_P5IP) | (INTD << D28IP_P4IP) |
			(INTC << D28IP_P3IP) | (INTB << D28IP_P2IP) | (INTA << D28IP_P1IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P) | (INTB << D26IP_U5P) | (INTA << D26IP_U4P);
	RCBA32(D25IP) = (INTA << D25IP_LIP);

	/*
	 * PIRQ allocation rationale (mirrors bd82x6x logic):
	 *
	 *   o Interrupts of the PCIe root ports are only about
	 *     events at the ports, not downstream devices. So we
	 *     don't expect many interrupts there and ignore them.
	 *   o We don't expect to talk constantly to the SMBus or
	 *     thermal device, so ignore those too.
	 *
	 * The functions that might matter first:
	 *
	 * D31IP_IDEP	IDE	-> PIRQ A
	 * D31IP_SIP	SATA	-> PIRQ A (MSI capable in AHCI mode)
	 * D29IP_E1P	EHCI 1	-> PIRQ C
	 * D27IP_ZIP	HDA	-> PIRQ D (MSI capable)
	 * D26IP_E2P	EHCI 2	-> PIRQ D
	 * D25IP_LIP	GbE	-> PIRQ B (MSI capable)
	 *
	 * D31IP_SMIP	SMBus	-> PIRQ B
	 * D31IP_TTIP	Thermal	-> PIRQ B
	 * D30IP_PIP	PCI Brg	-> PIRQ A
	 * D28IP_*	PCIe RP	-> PIRQ A-D (MSI capable)
	 */
#define _none 0
	DIR_ROUTE(D31IR, PIRQA, PIRQA, PIRQB, PIRQB);
	DIR_ROUTE(D30IR, PIRQA, _none, _none, _none);
	DIR_ROUTE(D29IR, PIRQC, _none, _none, _none);
	DIR_ROUTE(D28IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D27IR, PIRQD, _none, _none, _none);
	DIR_ROUTE(D26IR, PIRQD, _none, _none, _none);
	DIR_ROUTE(D25IR, PIRQB, _none, _none, _none);
#undef _none
}
