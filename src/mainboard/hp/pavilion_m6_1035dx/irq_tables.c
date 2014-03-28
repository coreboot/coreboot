/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#include <arch/pirq_routing.h>
#include <cpu/amd/amdfam15.h>
#include <console/console.h>
#include <string.h>
#include <stdint.h>

static void write_pirq_info(struct irq_info *pirq_info, u8 bus, u8 devfn,
			    u8 link0, u16 bitmap0, u8 link1, u16 bitmap1,
			    u8 link2, u16 bitmap2, u8 link3, u16 bitmap3,
			    u8 slot, u8 rfu)
{
	pirq_info->bus = bus;
	pirq_info->devfn = devfn;
	pirq_info->irq[0].link = link0;
	pirq_info->irq[0].bitmap = bitmap0;
	pirq_info->irq[1].link = link1;
	pirq_info->irq[1].bitmap = bitmap1;
	pirq_info->irq[2].link = link2;
	pirq_info->irq[2].bitmap = bitmap2;
	pirq_info->irq[3].link = link3;
	pirq_info->irq[3].bitmap = bitmap3;
	pirq_info->slot = slot;
	pirq_info->rfu = rfu;
}

extern u8 bus_isa;
extern u8 bus_hudson[6];
extern unsigned long sbdn_hudson;

unsigned long write_pirq_routing_table(unsigned long addr)
{
	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	u32 slot_num;
	u8 *v;

	u8 sum = 0;
	int i;

	get_bus_conf();		/* it will find out all bus num and apic that share with mptable.c and mptable.c and acpi_tables.c */

	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be betweeen 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (u8 *) (addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;

	pirq->rtr_bus = bus_hudson[0];
	pirq->rtr_devfn = ((sbdn_hudson + 0x14) << 3) | 4;

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = 0x1002;
	pirq->rtr_device = 0x4384;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;

	/* pci bridge */
	write_pirq_info(pirq_info, bus_hudson[0], ((sbdn_hudson + 0x14) << 3) | 4,
			0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0,
			0);
	pirq_info++;

	slot_num++;

	pirq->size = 32 + 16 * slot_num;

	for (i = 0; i < pirq->size; i++)
		sum += v[i];

	sum = pirq->checksum - sum;

	if (sum != pirq->checksum) {
		pirq->checksum = sum;
	}

	printk(BIOS_INFO, "write_pirq_routing_table done.\n");

	return (unsigned long)pirq_info;
}
