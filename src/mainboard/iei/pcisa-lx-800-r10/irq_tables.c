/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Nikolay Petukhov <nikolay.petukhov@gmail.com>
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

#include <arch/pirq_routing.h>
// #include <console/console.h>
#include <arch/io.h>

/* Platform IRQs */
#define PIRQA 11
#define PIRQB 10
#define PIRQC 11
#define PIRQD 5

/* Link */
#define LINK_PIRQA 1
#define LINK_PIRQB 2
#define LINK_PIRQC 3
#define LINK_PIRQD 4
#define LINK_NONE 0

/* Map */
#define IRQ_BITMAP_LINKA (1 << PIRQA)
#define IRQ_BITMAP_LINKB (1 << PIRQB)
#define IRQ_BITMAP_LINKC (1 << PIRQC)
#define IRQ_BITMAP_LINKD (1 << PIRQD)
#define IRQ_BITMAP_NOLINK 0x0

#define EXCLUSIVE_PCI_IRQS (IRQ_BITMAP_LINKA | IRQ_BITMAP_LINKB | IRQ_BITMAP_LINKC | IRQ_BITMAP_LINKD)

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version   */
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* there can be total CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0x00,			/* Where the interrupt router lies (bus) */
	(0x0F << 3) | 0x0,	/* Where the interrupt router lies (dev) */
	EXCLUSIVE_PCI_IRQS,	/* IRQs devoted exclusively to PCI usage */
	0x1078,			/* Vendor */
	0x0002,			/* Device */
	0,			/* Miniport data */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x62,			/* u8 checksum , this has to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */

	.slots = {
		[0] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x01<<3)|0x0,	/* 0x01 is CS5536 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},

		[1] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x0f<<3)|0x0,	/* 0x0f is CS5536 (USB, AUDIO) */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_PIRQB, /* Audio */
					.bitmap = IRQ_BITMAP_LINKB
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_PIRQD, /* USB */
					.bitmap = IRQ_BITMAP_LINKD
				}
			}
		},

		[2] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x0e<<3)|0x0,	/* 0x0e is eth0 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQD,
					.bitmap = IRQ_BITMAP_LINKD
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},

		[3] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x10<<3)|0x0,	/* 0x10 is eth1 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQB,
					.bitmap = IRQ_BITMAP_LINKB
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},

		[4] = {
			.slot = 0x0,	/* means also "on board" */
			.bus = 0x00,
			.devfn = (0x11<<3)|0x0,	/* 0x11 is SATA */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_NONE,
					.bitmap = IRQ_BITMAP_NOLINK
				}
			}
		},

/*
 * ################### backplane ###################
 */

/*
 * PCI1
 */
		[5] = {
			.slot = 0x1,	/* This is real PCI slot. */
			.bus = 0x00,
			.devfn = (0x09<<3)|0x0,	/* 0x09 is PCI1 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_PIRQB,
					.bitmap = IRQ_BITMAP_LINKB
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_PIRQC,
					.bitmap = IRQ_BITMAP_LINKC
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_PIRQD,
					.bitmap = IRQ_BITMAP_LINKD
				}
			}
		},
/*
 * PCI2
 */
		[6] = {
			.slot = 0x2,	/* This is real PCI slot. */
			.bus = 0x00,
			.devfn = (0x0a<<3)|0x0,	/* 0x0a is PCI2 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQD,
					.bitmap = IRQ_BITMAP_LINKD
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_PIRQB,
					.bitmap = IRQ_BITMAP_LINKB
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_PIRQC,
					.bitmap = IRQ_BITMAP_LINKC
				}
			}
		},
/*
 * PCI3
 */
		[7] = {
			.slot = 0x3,	/* This is real PCI slot. */
			.bus = 0x00,
			.devfn = (0x0b<<3)|0x0,	/* 0x0b is PCI3 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQC,
					.bitmap = IRQ_BITMAP_LINKC
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_PIRQD,
					.bitmap = IRQ_BITMAP_LINKD
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_PIRQB,
					.bitmap = IRQ_BITMAP_LINKB
				}
			}
		},
/*
 * PCI4
 */
		[8] = {
			.slot = 0x4,	/* This is real PCI slot. */
			.bus = 0x00,
			.devfn = (0x0c<<3)|0x0,	/* 0x0c is PCI4 */
			.irq = {
				[0] = {	/* <-- 0 means this is INTA# output from the device or slot */
					.link = LINK_PIRQB,
					.bitmap = IRQ_BITMAP_LINKB
				},
				[1] = {	/* <-- 1 means this is INTB# output from the device or slot */
					.link = LINK_PIRQC,
					.bitmap = IRQ_BITMAP_LINKC
				},
				[2] = {	/* <-- 2 means this is INTC# output from the device or slot */
					.link = LINK_PIRQD,
					.bitmap = IRQ_BITMAP_LINKD
				},
				[3] = {	/* <-- 3 means this is INTD# output from the device or slot */
					.link = LINK_PIRQA,
					.bitmap = IRQ_BITMAP_LINKA
				}
			}
		},
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	/* Put the PIR table in memory and checksum. */
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
