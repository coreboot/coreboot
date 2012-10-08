/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Copyright (C) 2012 Patrick Georgi <patrick@georgi-clan.de>
 * Copyright (C) 2010 Stefan Reinauer <stepan@coreboot.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ARCH_PIRQ_ROUTING_H
#define ARCH_PIRQ_ROUTING_H

/* This is the maximum number on interrupt entries that a PCI device may have.
 *   This is NOT the number of slots or devices in the system
 *   This is NOT the number of entries in the PIRQ table
 * This tells us that in the PIRQ table, we are going to have 4 link-bitmap
 * entries per PCI device
 * It is fixed at 4: INTA, INTB, INTC, and INTD
 * CAUTION: If you change this, pirq_routing will not work correctly*/
#define MAX_INTX_ENTRIES 4

#if CONFIG_GENERATE_PIRQ_TABLE
#include <stdint.h>

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

struct irq_info {
	u8 bus, devfn;	    /* Bus, device and function */
	struct {
		u8 link;    /* IRQ line ID, chipset dependent, 0=not routed */
		u16 bitmap; /* Available IRQs */
	} __attribute__((packed)) irq[4];
	u8 slot;	    /* Slot number, 0=onboard */
	u8 rfu;
} __attribute__((packed));

#ifndef CONFIG_IRQ_SLOT_COUNT
#warning "IRQ_SLOT_COUNT is not defined in Kconfig. PIRQ won't work correctly."
#endif

struct irq_routing_table {
	u32 signature;		/* PIRQ_SIGNATURE should be here */
	u16 version;		/* PIRQ_VERSION */
	u16 size;		/* Table size in bytes */
	u8  rtr_bus, rtr_devfn;	/* Where the interrupt router lies */
	u16 exclusive_irqs;	/* IRQs devoted exclusively to PCI usage */
	u16 rtr_vendor, rtr_device;/* Vendor/device ID of interrupt router */
	u32 miniport_data;
	u8  rfu[11];
	u8  checksum;		/* Modulo 256 checksum must give zero */
	struct irq_info slots[CONFIG_IRQ_SLOT_COUNT];
} __attribute__((packed));

unsigned long copy_pirq_routing_table(unsigned long addr, const struct irq_routing_table *routing_table);
unsigned long write_pirq_routing_table(unsigned long start);

#if CONFIG_PIRQ_ROUTE
void pirq_assign_irqs(const unsigned char pirq[CONFIG_MAX_PIRQ_LINKS]);
#endif

#else
#define copy_pirq_routing_table(start) (start)
#define write_pirq_routing_table(start) (start)
#endif

#endif /* ARCH_PIRQ_ROUTING_H */
