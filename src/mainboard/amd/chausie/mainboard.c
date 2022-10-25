/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/acpi.h>
#include <string.h>
#include <types.h>
#include "gpio.h"

/*
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 */
static uint8_t fch_pic_routing[FCH_IRQ_ROUTING_ENTRIES];
static uint8_t fch_apic_routing[FCH_IRQ_ROUTING_ENTRIES];

/*
 * This controls the device -> IRQ routing.
 *
 * Hardcoded IRQs:
 *  0: timer < soc/amd/common/acpi/lpc.asl
 *  1: i8042 - Keyboard
 *  2: cascade
 *  8: rtc0 <- soc/amd/common/acpi/lpc.asl
 *  9: acpi <- soc/amd/common/acpi/lpc.asl
 */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	12,		PIRQ_NC },
	{ PIRQ_B,	14,		PIRQ_NC },
	{ PIRQ_C,	15,		PIRQ_NC },
	{ PIRQ_D,	12,		PIRQ_NC },
	{ PIRQ_E,	14,		PIRQ_NC },
	{ PIRQ_F,	15,		PIRQ_NC },
	{ PIRQ_G,	12,		PIRQ_NC },
	{ PIRQ_H,	14,		PIRQ_NC },

	{ PIRQ_SCI,	ACPI_SCI_IRQ,	ACPI_SCI_IRQ },
	{ PIRQ_SD,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_SDIO,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_EMMC,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_GPIO,	11,		11 },
	{ PIRQ_I2C0,	10,		10 },
	{ PIRQ_I2C1,	 7,		 7 },
	{ PIRQ_I2C2,	 6,		 6 },
	{ PIRQ_I2C3,	 5,		 5 },
	{ PIRQ_UART0,	 4,		 4 },
	{ PIRQ_UART1,	 3,		 3 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
	{ PIRQ_HPET_L,	0x00,		0x00 },
	{ PIRQ_HPET_H,	0x00,		0x00 },
};

static const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void init_tables(void)
{
	const struct fch_irq_routing *mb_irq_map;
	size_t mb_fch_irq_mapping_table_size;
	size_t i;

	mb_irq_map = mb_get_fch_irq_mapping(&mb_fch_irq_mapping_table_size);

	memset(fch_pic_routing, PIRQ_NC, sizeof(fch_pic_routing));
	memset(fch_apic_routing, PIRQ_NC, sizeof(fch_apic_routing));

	for (i = 0; i < mb_fch_irq_mapping_table_size; i++) {
		if (mb_irq_map[i].intr_index >= FCH_IRQ_ROUTING_ENTRIES) {
			printk(BIOS_WARNING,
			       "Invalid IRQ index %u in FCH IRQ routing table entry %zu\n",
			       mb_irq_map[i].intr_index, i);
			continue;
		}
		fch_pic_routing[mb_irq_map[i].intr_index] = mb_irq_map[i].pic_irq_num;
		fch_apic_routing[mb_irq_map[i].intr_index] = mb_irq_map[i].apic_irq_num;
	}
}

static void pirq_setup(void)
{
	intr_data_ptr = fch_apic_routing;
	picr_data_ptr = fch_pic_routing;
}

static void mainboard_init(void *chip_info)
{
	mainboard_program_gpios();
}

static void mainboard_enable(struct device *dev)
{
	init_tables();
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
