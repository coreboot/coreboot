/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <acpi/acpi.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/i2c.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/acpimmio.h>
#include <variant/ec.h>
#include <variant/thermal.h>

/***********************************************************
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 *
 * These values are used by the PCI configuration space,
 * MP Tables.  TODO: Make ACPI use these values too.
 */

static const u8 mainboard_picr_data[FCH_IRQ_ROUTING_ENTRIES] = {
	[0x00] = 0x03, 0x04, 0x05, 0x07, 0x0B, 0x1F, 0x1F, 0x1F,
	[0x08] = 0xFA, 0xF1, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,
	[0x10] = 0x09, 0x1F, 0x1F, 0x03, 0x1F, 0x1F, 0x1F, 0x03,
	[0x18] = 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x20] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00,
	[0x28] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x30] = 0x05, 0x04, 0x05, 0x04, 0x04, 0x05, 0x04, 0x05,
	[0x38] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x40] = 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x48] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x50] = 0x03, 0x04, 0x05, 0x07, 0x1F, 0x1F, 0x1F, 0x1F,
	[0x58] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	[0x60] = 0x1F, 0x1F, 0x07, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	[0x68] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	[0x70] = 0x03, 0x0F, 0x06, 0x0E, 0x0A, 0x0B, 0x1F, 0x1F,
	[0x78] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
};

static const u8 mainboard_intr_data[FCH_IRQ_ROUTING_ENTRIES] = {
	[0x00] = 0x10, 0x11, 0x12, 0x13, 0x14, 0x1F, 0x16, 0x17,
	[0x08] = 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F,
	[0x10] = 0x09, 0x1F, 0x1F, 0x10, 0x1F, 0x1F, 0x1F, 0x10,
	[0x18] = 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x20] = 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00,
	[0x28] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x30] = 0x12, 0x11, 0x12, 0x11, 0x12, 0x11, 0x12, 0x00,
	[0x38] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x40] = 0x11, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x48] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x50] = 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00,
	[0x58] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x60] = 0x1F, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x68] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	[0x70] = 0x03, 0x0F, 0x06, 0x0E, 0x0A, 0x0B, 0x1F, 0x1F,
	[0x78] = 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/*
 * This table defines the index into the picr/intr_data tables for each
 * device.  Any enabled device and slot that uses hardware interrupts should
 * have an entry in this table to define its index into the FCH PCI_INTR
 * register 0xC00/0xC01.  This index will define the interrupt that it should
 *  use. Putting PIRQ_A into the PIN A index for a device will tell that
 * device to use PIC IRQ 10 if it uses PIN A for its hardware INT.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	{ PCIE0_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE1_DEVFN,	{ PIRQ_B, PIRQ_C, PIRQ_D, PIRQ_A } },
	{ PCIE2_DEVFN,	{ PIRQ_C, PIRQ_D, PIRQ_A, PIRQ_B } },
	{ PCIE3_DEVFN,	{ PIRQ_D, PIRQ_A, PIRQ_B, PIRQ_C } },
	{ PCIE4_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ HDA0_DEVFN,	{ PIRQ_NC, PIRQ_HDA, PIRQ_NC, PIRQ_NC } },
	{ SD_DEVFN,	{ PIRQ_SD, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ SMBUS_DEVFN,	{ PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ SATA_DEVFN,	{ PIRQ_SATA, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ EHCI1_DEVFN,	{ PIRQ_EHCI, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
	{ XHCI_DEVFN,	{ PIRQ_XHCI, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
};

/* PIRQ Setup */
static void pirq_setup(void)
{
	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = mainboard_intr_data;
	picr_data_ptr = mainboard_picr_data;
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

static void mainboard_init(void *chip_info)
{
	int boardid = board_id();
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	printk(BIOS_INFO, "Board ID: %d\n", boardid);

	mainboard_ec_init();

	gpios = variant_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	/* Initialize i2c buses that were not initialized in bootblock */
	i2c_soc_init();

	/* Set GenIntDisable so that GPIO 90 is configured as a GPIO. */
	pm_write8(PM_PCIB_CFG, pm_read8(PM_PCIB_CFG) | PM_GENINT_DISABLE);

	/* Set low-power mode for BayHub eMMC bridge's PCIe clock. */
	clrsetbits32(acpimmio_misc + GPP_CLK_CNTRL,
		     GPP_CLK2_REQ_MAP_MASK,
		     GPP_CLK2_REQ_MAP_CLK_REQ2 <<
		     GPP_CLK2_REQ_MAP_SHIFT);

	/* Same for the WiFi */
	clrsetbits32(acpimmio_misc + GPP_CLK_CNTRL,
		     GPP_CLK0_REQ_MAP_MASK,
		     GPP_CLK0_REQ_MAP_CLK_REQ0 <<
		     GPP_CLK0_REQ_MAP_SHIFT);

	variant_devtree_update();
}

/*************************************************
 * Dedicated mainboard function
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();

}

int mainboard_get_xhci_oc_map(uint16_t *map)
{
	return variant_get_xhci_oc_map(map);
}

int mainboard_get_ehci_oc_map(uint16_t *map)
{
	return variant_get_ehci_oc_map(map);
}

void mainboard_suspend_resume(void)
{
	variant_mainboard_suspend_resume();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

/* Variants may override these functions so see definitions in variants/ */
uint8_t __weak variant_board_sku(void)
{
	return 0;
}

void __weak variant_mainboard_suspend_resume(void)
{
}

const char *smbios_system_sku(void)
{
	static char sku_str[7]; /* sku{0..255} */

	snprintf(sku_str, sizeof(sku_str), "sku%d", variant_board_sku());

	return sku_str;
}
