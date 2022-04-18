/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio.h>
#include <amdblocks/amd_pci_util.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/acpi.h>
#include <variant/ec.h>

/*
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 */
static uint8_t fch_pic_routing[0x80];
static uint8_t fch_apic_routing[0x80];

_Static_assert(sizeof(fch_pic_routing) == sizeof(fch_apic_routing),
	"PIC and APIC FCH interrupt tables must be the same size");

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

static const struct fch_irq_routing {
	uint8_t intr_index;
	uint8_t pic_irq_num;
	uint8_t apic_irq_num;
} skyrim_fch[] = {
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

static void init_tables(void)
{
	const struct fch_irq_routing *entry;
	int i;

	memset(fch_pic_routing, PIRQ_NC, sizeof(fch_pic_routing));
	memset(fch_apic_routing, PIRQ_NC, sizeof(fch_apic_routing));

	for (i = 0; i < ARRAY_SIZE(skyrim_fch); i++) {
		entry = skyrim_fch + i;
		fch_pic_routing[entry->intr_index] = entry->pic_irq_num;
		fch_apic_routing[entry->intr_index] = entry->apic_irq_num;
	}
}

static void pirq_setup(void)
{
	intr_data_ptr = fch_apic_routing;
	picr_data_ptr = fch_pic_routing;
}

static void mainboard_configure_gpios(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	variant_base_gpio_table(&base_gpios, &base_num_gpios);
	variant_override_gpio_table(&override_gpios, &override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios,
				override_gpios, override_num_gpios);
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	init_tables();
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();

	/* TODO: b/184678786 - Move into espi_config */
	/* Unmask eSPI IRQ 1 (Keyboard) */
	pm_write32(PM_ESPI_INTR_CTRL, PM_ESPI_DEV_INTR_MASK & ~(BIT(1)));
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
