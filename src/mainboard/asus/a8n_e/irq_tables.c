/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
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

#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>
#include <cpu/amd/amdk8_sysconf.h>

extern unsigned char bus_isa;
extern unsigned char bus_ck804[6];


/**
 * Add one line to IRQ table.
 */
static void write_pirq_info(struct irq_info *pirq_info, uint8_t bus,
			    uint8_t devfn, uint8_t link0, uint16_t bitmap0,
			    uint8_t link1, uint16_t bitmap1, uint8_t link2,
			    uint16_t bitmap2, uint8_t link3, uint16_t bitmap3,
			    uint8_t slot, uint8_t rfu)
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

/**
 * Create the IRQ routing table.
 * Values are derived from getpir generated code.
 */
unsigned long write_pirq_routing_table(unsigned long addr)
{
	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num, sbdn;
	uint8_t *v, sum = 0;
	int i;

	/* get_bus_conf() will find out all bus num and APIC that share with
	 * mptable.c and mptable.c.
	 */
	get_bus_conf();
	sbdn = sysconf.sbdn;

	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be betweeen 0xf0000 & 0x100000. */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *)(addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;
	pirq->rtr_bus = bus_ck804[0];
	pirq->rtr_devfn = ((sbdn + 9) << 3) | 0;
	pirq->exclusive_irqs = 0x828;
	pirq->rtr_vendor = 0x10de;
	pirq->rtr_device = 0x005c;
	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;

	/* Slot1 PCIE 16x */
	write_pirq_info(pirq_info, bus_ck804[1], (0 << 3) | 0, 0x3, 0xdeb8, 0x4,
			0xdeb8, 0x1, 0xdeb8, 0x2, 0xdeb8, 4, 0);
	pirq_info++;
	slot_num++;

	/* Slot2 PCIE 1x */
	write_pirq_info(pirq_info, bus_ck804[2], (0 << 3) | 0, 0x4, 0xdeb8, 0x1,
			0xdeb8, 0x2, 0xdeb8, 0x3, 0xdeb8, 5, 0);
	pirq_info++;
	slot_num++;

	/* Slot3 PCIE 1x */
	write_pirq_info(pirq_info, bus_ck804[3], (0 << 3) | 0, 0x1, 0xdeb8, 0x2,
			0xdeb8, 0x3, 0xdeb8, 0x4, 0xdeb8, 6, 0);
	pirq_info++;
	slot_num++;

	/* Slot4 PCIE 4x */
	write_pirq_info(pirq_info, bus_ck804[4], (0x4 << 3) | 0, 0x2,
			0xdeb8, 0x3, 0xdeb8, 0x4, 0xdeb8, 0x1, 0xdeb8, 7, 0);
	pirq_info++;
	slot_num++;

	/* Slot5 - Slot7 PCI */
	for (i = 0; i < 3; i++) {
		write_pirq_info(pirq_info, bus_ck804[5], (0 << (6 + i)) | 0,
				((i + 0) % 4) + 1, 0xdeb8,
				((i + 1) % 4) + 1, 0xdeb8,
				((i + 2) % 4) + 1, 0xdeb8,
				((i + 3) % 4) + 1, 0xdeb8, i, 0);
		pirq_info++;
		slot_num++;
	}

	/* PCI bridge */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 9) << 3) | 0, 0x1,
			0xdeb8, 0x2, 0xdeb8, 0x3, 0xdeb8, 0x4, 0xdeb8, 0, 0);
	pirq_info++;
	slot_num++;

	/* SMBus */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 1) << 3) | 0, 0x2,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

	/* USB */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 2) << 3) | 0, 0x1,
			0xdeb8, 0x2, 0xdeb8, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

	/* Audio */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 4) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

	/* SATA */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 7) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

	/* SATA */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 8) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

	/* NIC */
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 0xa) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

#if 0
	/* Firewire? */
	write_pirq_info(pirq_info, bus_ck804_1, (0x5 << 3) | 0, 0x3, 0xdeb8, 0,
			0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
#endif

	pirq->size = 32 + 16 * slot_num;

	for (i = 0; i < pirq->size; i++)
		sum += v[i];

	sum = pirq->checksum - sum;
	if (sum != pirq->checksum)
		pirq->checksum = sum;

	printk(BIOS_INFO, "done.\n");

	return (unsigned long)pirq_info;
}
