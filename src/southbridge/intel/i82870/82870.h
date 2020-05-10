/* SPDX-License-Identifier: GPL-2.0-only */

/* for io APIC 1461 */
#define MBAR		0x10
#define ABAR		0x40

/* for pci bridge  1460 */
#define MTT		0x042
#define HCCR		0x0f0
#define ACNF		0x0e0
#define STRP		0x44		// Strap status register

#define STRP_EN133	0x0001		// 133 MHz-capable (Px_133EN)
#define STRP_HPCAP	0x0002		// Hot-plug capable (Hx_SLOT zero/nonzero)

#define ACNF_SYNCPH	0x0010		// PCI(-X) input clock is synchronous to hub input clock
