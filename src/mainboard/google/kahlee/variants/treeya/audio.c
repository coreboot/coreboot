/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <variant/sku.h>
#include <string.h>
#include <drivers/i2c/hid/chip.h>

#define RT58_I2C_ADDRESS 0x1a

extern struct chip_operations drivers_i2c_generic_ops;
extern struct chip_operations drivers_i2c_da7219_ops;

void variant_devtree_update(void)
{
	struct device *mmio_dev = NULL, *child = NULL;
	struct device *alc_dev = NULL, *da7219_dev = NULL;

	do {
		mmio_dev = dev_find_path(mmio_dev, DEVICE_PATH_MMIO);
		if (!mmio_dev) {
			printk(BIOS_INFO, "Checking audio codec\n");
			return;
		}
	} while (mmio_dev->path.mmio.addr != APU_I2C0_BASE);

	while ((child = dev_bus_each_child(mmio_dev->link_list, child)) != NULL) {
		if (child->path.type != DEVICE_PATH_I2C)
			continue;
		if (child->path.i2c.device != RT58_I2C_ADDRESS)
			continue;
		if (child->chip_ops == &drivers_i2c_generic_ops) {
			struct drivers_i2c_generic_config *config = child->chip_info;
			if (!strcmp(config->hid, "10EC5682"))
				alc_dev = child;
		} else if (child->chip_ops == &drivers_i2c_da7219_ops) {
			da7219_dev = child;
		}
	}

	switch (google_chromeec_get_sku_id()) {
	case SKU_TREEYA_ALC5682_AE:
	case SKU_TREEYA_ALC5682_AF:
		/* alc5682 only */
		if (da7219_dev)
			da7219_dev->enabled = 0;
		if (alc_dev)
			alc_dev->enabled = 1;
		break;
	default:
		/* da7219 only */
		if (da7219_dev)
			da7219_dev->enabled = 1;
		if (alc_dev)
			alc_dev->enabled = 0;
		break;
	}
}
