/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* for io apic 1461 */
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
