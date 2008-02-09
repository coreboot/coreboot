/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 Ollie Lo, Silicon Integrated Systems
 * Copyright (C) 2008 Ron Minnich
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#ifndef ARCH_PIRQ_ROUTING_H
#define ARCH_PIRQ_ROUTING_H

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

struct irq_info {
	u8 bus, devfn;		/* Bus, device and function */
	struct {
		u8 link;	/* IRQ line ID, chipset dependent, 0=not routed */
		u16 bitmap;	/* Available IRQs */
	} __attribute__((packed)) irq[4];
	u8 slot;		/* Slot number, 0=onboard */
	u8 rfu;
} __attribute__((packed));

struct irq_routing_table {
	u32 signature;			/* PIRQ_SIGNATURE should be here */
	u16 version;			/* PIRQ_VERSION */
	u16 size;			/* Table size in bytes */
	u8  rtr_bus, rtr_devfn;		/* Where the interrupt router lies */
	u16 exclusive_irqs;		/* IRQs devoted exclusively to PCI usage */
	u16 rtr_vendor, rtr_device;	/* Vendor and device ID of interrupt router */
	u32 miniport_data;		/* Crap */
	u8  rfu[11];
	u8  checksum;			/* Modulo 256 checksum must give zero */
	struct irq_info slots[];
} __attribute__((packed));

extern const struct irq_routing_table intel_irq_routing_table;

unsigned long copy_pirq_routing_table(unsigned long start);
unsigned long write_pirq_routing_table(unsigned long start);

#endif /* ARCH_PIRQ_ROUTING_H */
