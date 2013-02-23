/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Ricardo Martins <rasmartins@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <device/pci_ids.h>
#include <arch/pirq_routing.h>

/* Platform IRQs */
#define PIRQA 10
#define PIRQB 11
#define PIRQC 11
#define PIRQD 11

/* Links */
#define L_PIRQA 1
#define L_PIRQB 2
#define L_PIRQC 3
#define L_PIRQD 4

/* Bitmaps */
#define B_LINK0 (1 << PIRQA)
#define B_LINK1 (1 << PIRQB)
#define B_LINK2 (1 << PIRQC)
#define B_LINK3 (1 << PIRQD)

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,				/* u32 signature */
	PIRQ_VERSION,				/* u16 version */
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,	/* Max. number of devices on the bus */
	0x00,					/* Interrupt router bus */
	0x0f << 3,				/* Interrupt router dev */
	B_LINK0 | B_LINK1 | B_LINK2 | B_LINK3,	/* IRQs devoted exclusively to PCI usage */
	PCI_VENDOR_ID_AMD,			/* Vendor */
	PCI_DEVICE_ID_AMD_CS5536_ISA,		/* Device */
	0,					/* Miniport */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* Reserved */
	0xa6,					/* Checksum */
	{
		[0] = {				/* Host bridge */
			.slot = 0x00,
			.bus = 0x00,
			.devfn = (0x01 << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				},
				[1] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				},
				[2] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				},
				[3] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				}
			}
		},

		[1] = {				/* ISA bridge */
			.slot = 0x00,
			.bus = 0x00,
			.devfn = (0x0f << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				},
				[1] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				},
				[2] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				},
				[3] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				}
			}
		},

		[2] = {				/* Ethernet */
			.slot = 0x00,
			.bus = 0x00,
			.devfn = (0x0e << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				},
				[1] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				},
				[2] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				},
				[3] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				}
			}
		},

		[3] = {				/* PCI Connector - Slot 0 */
			.slot = 0x01,
			.bus = 0x00,
			.devfn = (0x09 << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				},
				[1] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				},
				[2] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				},
				[3] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				}
			}
		},

		[4] = {				/* PCI Connector - Slot 1 */
			.slot = 0x02,
			.bus = 0x00,
			.devfn = (0x0c << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				},
				[1] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				},
				[2] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				},
				[3] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				}
			}
		},

		[5] = {				/* PCI Connector - Slot 2 */
			.slot = 0x03,
			.bus = 0x00,
			.devfn = (0x0b << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				},
				[1] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				},
				[2] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				},
				[3] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				}
			}
		},

		[6] = {				/* PCI Connector - Slot 3 */
			.slot = 0x04,
			.bus = 0x00,
			.devfn = (0x0a << 3) | 0x0,
			.irq = {
				[0] = {
					.link = L_PIRQD,
					.bitmap = B_LINK3
				},
				[1] = {
					.link = L_PIRQA,
					.bitmap = B_LINK0
				},
				[2] = {
					.link = L_PIRQB,
					.bitmap = B_LINK1
				},
				[3] = {
					.link = L_PIRQC,
					.bitmap = B_LINK2
				}
			}
		}
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
