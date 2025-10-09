/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include "amdtool.h"
#include "smn.h"

#define AMD_PCI_INTR_IDX	0xc00
#define AMD_PCI_INTR_DATA	0xc01

#define PCI_INTR_ROUTE_IOAPIC	0x80

struct irq_routing_table {
	const io_register_t irq_reg;
	void (*print_irq_reg)(uint8_t val);
};

static void print_misc_irq(uint8_t val)
{
	static const char * const irq14_irq15_map[] = {
		"legacy IDE",
		"SATA_IDE",
		"SATA2",
		"SERIRQ/PCI",
	};

	printf("\tIRQ0 source: %s\n"
	       "\tIRQ1 source: %s\n"
	       "\tIRQ8 source: %s\n"
	       "\tIRQ12 source: %s\n"
	       "\tIRQ14 mapped to: %s\n"
	       "\tIRQ15 mapped to: %s\n",
	       val & 0x1 ? "SERIRQ/PCI" : "i8254",
	       val & 0x2 ? "SERIRQ/PCI" : "IMC",
	       val & 0x4 ? "SERIRQ/PCI" : "RTC",
	       val & 0x8 ? "SERIRQ/PCI" : "IMC",
	       irq14_irq15_map[(val >> 4) & 0x3],
 	       irq14_irq15_map[(val >> 6) & 0x3]);
}

static void print_misc0_irq(uint8_t val)
{
	printf("\t%s\n"
	       "\tUSB IRQ1 source: %s\n"
	       "\tUSB IRQ12 source: %s\n"
	       "\tIRQ1/IRQ12 mask: %sabled\n"
	       "\tIRQ input: %s\n"
	       "\tIRQ1 filter: %sabled\n"
	       "\tIRQ12 filter: %sabled\n"
	       "\tINTR delay 600ns: %sabled\n",
	       val & 0x1 ? "IOAPIC INT2 from PIC IRQ0, IOAPIC INT0 from PIC INTR"
			 : "IOAPIC INT0 from PIC IRQ0, IOAPIC INT2 from PIC INTR",
	       val & 0x2 ? "IMC IRQ1" : "serial IRQ1",
	       val & 0x4 ? "IMC IRQ12" : "serial IRQ12",
	       val & 0x8 ? "en" : "dis",
	       val & 0x10 ? "enabled" : "masked off",
	       val & 0x20 ? "en" : "dis",
	       val & 0x40 ? "en" : "dis",
	       val & 0x80 ? "en" : "dis");
}

static const struct irq_routing_table kunlun_irq_routing[] = {
	{ { 0x00, 1, "INTA#" }, NULL },
	{ { 0x01, 1, "INTB#" }, NULL },
	{ { 0x02, 1, "INTC#" }, NULL },
	{ { 0x03, 1, "INTD#" }, NULL },
	{ { 0x04, 1, "INTE#" }, NULL },
	{ { 0x05, 1, "INTF#/GENINT2" }, NULL },
	{ { 0x06, 1, "INTG#" }, NULL },
	{ { 0x07, 1, "INTH#" }, NULL },
	{ { 0x08, 1, "Misc" }, print_misc_irq },
	{ { 0x09, 1, "Misc0" }, print_misc0_irq },
	{ { 0x0A, 1, "Misc1/HPET_L" }, NULL },
	{ { 0x0B, 1, "Misc2/HPET_H" }, NULL },
	{ { 0x0C, 1, "INTA from SERIRQ" }, NULL },
	{ { 0x0D, 1, "INTB from SERIRQ" }, NULL },
	{ { 0x0E, 1, "INTC from SERIRQ" }, NULL },
	{ { 0x0F, 1, "INTD from SERIRQ" }, NULL },
	{ { 0x10, 1, "SCI" }, NULL },
	{ { 0x11, 1, "SMBUS0" }, NULL },
	{ { 0x12, 1, "ASF" }, NULL },
	{ { 0x13, 1, "HDA" }, NULL },
	{ { 0x14, 1, "GBE0" }, NULL },
	{ { 0x15, 1, "GBE1" }, NULL },
	{ { 0x16, 1, "PerMon" }, NULL },
	{ { 0x17, 1, "SD" }, NULL },
	{ { 0x1A, 1, "SDIO" }, NULL },
	{ { 0x20, 1, "CIR (no IRQ connected)" }, NULL },
	{ { 0x21, 1, "GPIOa (from PAD_FANIN0)" }, NULL },
	{ { 0x22, 1, "GPIOb (from PAD_FANOUT0)" }, NULL },
	{ { 0x23, 1, "GPIOc (no IRQ connected)" }, NULL },
	{ { 0x30, 1, "USB Emu" }, NULL },
	{ { 0x31, 1, "USB Dual Role 1" }, NULL },
	{ { 0x32, 1, "USB Dual Role 2" }, NULL },
	{ { 0x34, 1, "XHCI0" }, NULL },
	{ { 0x35, 1, "USB SSIC" }, NULL },
	{ { 0x40, 1, "IDE" }, NULL },
	{ { 0x41, 1, "SATA" }, NULL },
	{ { 0x42, 1, "UFS" }, NULL },
	{ { 0x43, 1, "EMMC" }, NULL },
	{ { 0x50, 1, "GPPInt0" }, NULL },
	{ { 0x51, 1, "GPPInt1" }, NULL },
	{ { 0x52, 1, "GPPInt2" }, NULL },
	{ { 0x53, 1, "GPPInt3" }, NULL },
	{ { 0x60, 1, "Gevent SCI interrupt" }, NULL },
	{ { 0x61, 1, "Gevent SMI interrupt" }, NULL },
	{ { 0x62, 1, "GPIO controller interrupt" }, NULL },
	{ { 0x70, 1, "I2C0/I3C0" }, NULL },
	{ { 0x71, 1, "I2C1/I3C1" }, NULL },
	{ { 0x72, 1, "I2C2/I3C2" }, NULL },
	{ { 0x73, 1, "I2C3/I3C3" }, NULL },
	{ { 0x74, 1, "UART0" }, NULL },
	{ { 0x75, 1, "UART1" }, NULL },
	{ { 0x76, 1, "I2C4" }, NULL },
	{ { 0x77, 1, "I2C5" }, NULL },
	{ { 0x78, 1, "UART2" }, NULL },
	{ { 0x79, 1, "UART3" }, NULL },
};

static uint8_t read_irq_reg(uint8_t addr)
{
	outb(addr, AMD_PCI_INTR_IDX);
	return inb(AMD_PCI_INTR_DATA);
}

int print_irq_routing(struct pci_dev *sb)
{
	int i, irq_table_size;
	const struct irq_routing_table *irq_table = NULL;
	int smbus_rev;
	uint8_t reg;

	printf("\n============= IRQ routing ==============\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_AMD_FCH_LPC_2:
		smbus_rev = find_smbus_dev_rev(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2);
		if (smbus_rev == -1)
			return 1;

		switch (smbus_rev) {
		case 0x71:
			irq_table_size = ARRAY_SIZE(kunlun_irq_routing);
			irq_table = kunlun_irq_routing;
			break;
		default:
			printf("Error: Dumping IRQ routing on this southbridge is not (yet) supported.\n");
			return 1;
		}

		break;
	default:
		printf("Error: Dumping IRQ routing on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("PIC routing:\n\n");
	for (i = 0; i < irq_table_size; i++) {
		reg = read_irq_reg(irq_table[i].irq_reg.addr);
		printf("IRQ REG 0x%02x = 0x%02x (%s)\n", irq_table[i].irq_reg.addr,
							 reg, irq_table[i].irq_reg.name);
		if (irq_table[i].print_irq_reg)
			irq_table[i].print_irq_reg(reg);
	}

	printf("\nIOAPIC routing:\n\n");
	for (i = 0; i < irq_table_size; i++) {
		reg = read_irq_reg(irq_table[i].irq_reg.addr | PCI_INTR_ROUTE_IOAPIC);
		printf("IRQ REG 0x%02x = 0x%02x (%s)\n",
		       irq_table[i].irq_reg.addr | PCI_INTR_ROUTE_IOAPIC,
		       reg, irq_table[i].irq_reg.name);
		if (irq_table[i].print_irq_reg)
			irq_table[i].print_irq_reg(reg);
	}

	printf("========================================\n");

	return 0;
}
