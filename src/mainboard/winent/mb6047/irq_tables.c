#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>

#include <cpu/amd/amdk8_sysconf.h>

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

extern unsigned char bus_ck804_0;	//1
extern unsigned char bus_ck804_1;	//2
extern unsigned char bus_ck804_2;	//3
extern unsigned char bus_ck804_3;	//4
extern unsigned char bus_ck804_4;	//5
extern unsigned char bus_ck804_5;	//6

unsigned long write_pirq_routing_table(unsigned long addr)
{

	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num;
	uint8_t *v;
	unsigned sbdn;

	uint8_t sum = 0;
	int i;

	get_bus_conf();		// it will find out all bus num and apic that share with mptable.c and mptable.c and acpi_tables.c
	sbdn = sysconf.sbdn;

	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be between 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *) (addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;

	pirq->rtr_bus = bus_ck804_0;
	pirq->rtr_devfn = ((sbdn + 9) << 3) | 0;

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = 0x10de;
	pirq->rtr_device = 0x005c;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;
//pci bridge
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 9) << 3) | 0, 0x1,
			0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0, 0);
	pirq_info++;
	slot_num++;

#if 0
//smbus
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 1) << 3) | 0, 0x2,
			0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

//usb
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 2) << 3) | 0, 0x1,
			0xdef8, 0x2, 0xdef8, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

//audio
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 4) << 3) | 0, 0x1,
			0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
//sata
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 7) << 3) | 0, 0x1,
			0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
//sata
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 8) << 3) | 0, 0x1,
			0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
//nic
	write_pirq_info(pirq_info, bus_ck804_0, ((sbdn + 0xa) << 3) | 0, 0x1,
			0xdef8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

//Slot1 PCIE x16
	write_pirq_info(pirq_info, bus_ck804_5, (0 << 3) | 0, 0x3, 0xdef8, 0x4,
			0xdef8, 0x1, 0xdef8, 0x2, 0xdef8, 1, 0);
	pirq_info++;
	slot_num++;

//firewire
	write_pirq_info(pirq_info, bus_ck804_1, (0x5 << 3) | 0, 0x3, 0xdef8, 0,
			0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

//Slot2 pci
	write_pirq_info(pirq_info, bus_ck804_1, (0x4 << 3) | 0, 0x1, 0xdef8,
			0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 2, 0);
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
