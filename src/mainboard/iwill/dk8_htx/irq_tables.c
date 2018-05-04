#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>
#include <cpu/amd/amdk8_sysconf.h>

#include "mb_sysconf.h"

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

unsigned long write_pirq_routing_table(unsigned long addr)
{

	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num;
	uint8_t *v;

	uint8_t sum = 0;
	int i;

	struct mb_sysconf_t *m;

	get_bus_conf();		// it will find out all bus num and apic that share with mptable.c and mptable.c and acpi_tables.c

	m = sysconf.mb;

	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be between 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *) (addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;

	pirq->rtr_bus = m->bus_8111_0;
	pirq->rtr_devfn = ((sysconf.sbdn + 1) << 3) | 0;

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = 0x1022;
	pirq->rtr_device = 0x746b;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;

	{
		struct device *dev;
		dev =
		    dev_find_slot(m->bus_8111_0,
				  PCI_DEVFN(sysconf.sbdn + 1, 3));
		if (dev) {
			/* initialize PCI interupts - these assignments depend
			   on the PCB routing of PINTA-D

			   PINTA = IRQ3
			   PINTB = IRQ5
			   PINTC = IRQ10
			   PINTD = IRQ11
			 */
			pci_write_config16(dev, 0x56, 0xba53);
		}
	}

//pci bridge
	printk(BIOS_DEBUG, "setting Onboard AMD Southbridge\n");
	static const unsigned char slotIrqs_1_4[4] = { 3, 5, 10, 11 };
	pci_assign_irqs(m->bus_8111_0, sysconf.sbdn + 1, slotIrqs_1_4);
	write_pirq_info(pirq_info, m->bus_8111_0, ((sysconf.sbdn + 1) << 3) | 0,
			0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0,
			0);
	pirq_info++;
	slot_num++;

	printk(BIOS_DEBUG, "setting Onboard AMD USB\n");
	static const unsigned char slotIrqs_8111_1_0[4] = { 0, 0, 0, 11 };
	pci_assign_irqs(m->bus_8111_1, 0, slotIrqs_8111_1_0);
	write_pirq_info(pirq_info, m->bus_8111_1, 0, 0, 0, 0, 0, 0, 0, 0x4,
			0xdef8, 0, 0);
	pirq_info++;
	slot_num++;

//pcix bridge
//        write_pirq_info(pirq_info, m->bus_8132_0, (sbdn3 << 3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0, 0);
//        pirq_info++; slot_num++;

	int j = 0;

	for (i = 1; i < sysconf.hc_possible_num; i++) {
		if (!(sysconf.pci1234[i] & 0x1))
			continue;
		unsigned busn = (sysconf.pci1234[i] >> 16) & 0xff;
		unsigned devn = sysconf.hcdn[i] & 0xff;

		write_pirq_info(pirq_info, busn, (devn << 3) | 0, 0x1, 0xdef8,
				0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0, 0);
		pirq_info++;
		slot_num++;
		j++;

	}

	pirq->size = 32 + 16 * slot_num;

	for (i = 0; i < pirq->size; i++)
		sum += v[i];

	sum = pirq->checksum - sum;

	if (sum != pirq->checksum) {
		pirq->checksum = sum;
	}

	printk(BIOS_INFO, "done.\n");

	return (unsigned long)pirq_info;

}
