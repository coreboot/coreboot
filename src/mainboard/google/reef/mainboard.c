/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/ec.h>
#include <gpio.h>
#include <nhlt.h>
#include <smbios.h>
#include <soc/nhlt.h>
#include <string.h>
#include <variant/ec.h>
#include <variant/gpio.h>

/* override specific gpio by sku id */
const struct pad_config __weak
*variant_sku_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

static void mainboard_init(void *chip_info)
{
	int boardid;
	const struct pad_config *pads;
	size_t num;

	boardid = board_id();
	printk(BIOS_INFO, "Board ID: %d\n", boardid);

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);

	pads = variant_sku_gpio_table(&num);
	gpio_configure_pads(pads, num);

	mainboard_ec_init();

	variant_board_ec_set_skuid();
}

/*
 * There are 2 pins on reef-like boards that can be used for SKU'ing
 * board differences. They each have optional stuffing for a pullup and
 * a pulldown. This way we can generate 9 different values with the
 * 2 pins.
 */
uint8_t sku_strapping_value(void)
{
	gpio_t board_sku_gpios[] = {
		[1] = GPIO_17, [0] = GPIO_16,
	};
	const size_t num = ARRAY_SIZE(board_sku_gpios);

	return gpio_base3_value(board_sku_gpios, num);
}

uint8_t __weak variant_board_sku(void)
{
	static int board_sku_num = -1;

	if (board_sku_num < 0)
		board_sku_num = sku_strapping_value();

	return board_sku_num;
}

/* Set variant board sku to ec by sku id */
void __weak variant_board_ec_set_skuid(void)
{
}

const char *smbios_system_sku(void)
{
	static char sku_str[7]; /* sku{0..255} */

	snprintf(sku_str, sizeof(sku_str), "sku%d", variant_board_sku());

	return sku_str;
}

void __weak variant_nhlt_oem_overrides(const char **oem_id,
						const char **oem_table_id,
						uint32_t *oem_revision)
{
	*oem_id = "reef";
	*oem_table_id = CONFIG_VARIANT_DIR;
	*oem_revision = variant_board_sku();
}

static unsigned long mainboard_write_acpi_tables(
	const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;
	const char *oem_id = NULL;
	const char *oem_table_id = NULL;
	uint32_t oem_revision = 0;

	start_addr = current;

	nhlt = nhlt_init();

	if (nhlt == NULL)
		return start_addr;

	variant_nhlt_init(nhlt);
	variant_nhlt_oem_overrides(&oem_id, &oem_table_id, &oem_revision);

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
			oem_id, oem_table_id, oem_revision);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
