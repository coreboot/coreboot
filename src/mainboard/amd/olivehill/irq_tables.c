/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/bsd/helpers.h>
#include <device/pci_def.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>

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

unsigned long write_pirq_routing_table(unsigned long addr)
{
	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	u32 slot_num;
	u8 *v;

	u8 sum = 0;
	int i;

	/* Align the table to be 16 byte aligned. */
	addr = ALIGN_UP(addr, 16);

	/* This table must be between 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (u8 *)(addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;

	pirq->rtr_bus = 0;
	pirq->rtr_devfn = PCI_DEVFN(0x14, 4);

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = 0x1002;
	pirq->rtr_device = 0x4384;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;

	/* pci bridge */
	write_pirq_info(pirq_info, 0, PCI_DEVFN(0x14, 4),
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

	printk(BIOS_INFO, "%s done.\n", __func__);

	return (unsigned long)pirq_info;
}
