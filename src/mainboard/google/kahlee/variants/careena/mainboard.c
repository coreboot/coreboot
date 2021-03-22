/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <variant/sku.h>
#include <string.h>
#include <drivers/i2c/hid/chip.h>

extern struct chip_operations drivers_i2c_generic_ops;
extern struct chip_operations drivers_i2c_da7219_ops;

void variant_devtree_update(void)
{
	uint32_t sku = google_chromeec_get_sku_id();
	struct device *mmio_dev = NULL, *child = NULL;
	struct device *alc_dev = NULL, *da7219_dev = NULL;

	while (1) {
		mmio_dev = dev_find_path(mmio_dev, DEVICE_PATH_MMIO);
		if (mmio_dev == NULL)
			break;
		if (mmio_dev->path.mmio.addr == 0xfedc2000)
			break;
	}

	if (mmio_dev == NULL)
		return;

	while ((child = dev_bus_each_child(mmio_dev->link_list, child)) != NULL) {
		if (child->path.type != DEVICE_PATH_I2C)
			continue;
		if (child->path.i2c.device != 0x1a)
			continue;
		if (child->chip_ops == &drivers_i2c_generic_ops) {
			struct drivers_i2c_generic_config *config = child->chip_info;
			if (!strcmp(config->hid, "10EC5682"))
				alc_dev = child;
		} else if (child->chip_ops == &drivers_i2c_da7219_ops) {
			da7219_dev = child;
		}
	}

	switch (sku) {
	default:
		/* da7219 only */
		if (da7219_dev)
			da7219_dev->enabled = 1;
		if (alc_dev)
			alc_dev->enabled = 0;
		break;
	case SKU_BARLA_ALC5682_44:
	case SKU_BARLA_ALC5682_45:
	case SKU_BARLA_ALC5682_46:
	case SKU_BARLA_ALC5682_47:
		/* alc5682 only */
		if (da7219_dev)
			da7219_dev->enabled = 0;
		if (alc_dev)
			alc_dev->enabled = 1;
		break;
	}
}
