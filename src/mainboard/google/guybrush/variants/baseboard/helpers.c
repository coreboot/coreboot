/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/iomap.h>

bool variant_has_fpmcu(void)
{
	DEVTREE_CONST struct device *mmio_dev = NULL;
	static const struct device_path fpmcu_path[] = {
		{
			.type = DEVICE_PATH_MMIO,
			.mmio.addr = APU_UART1_BASE
		},
		{
			.type = DEVICE_PATH_GENERIC,
			.generic.id = 0,
			.generic.subid = 0
		},
	};
	mmio_dev = find_dev_nested_path(
		all_devices->link_list, fpmcu_path, ARRAY_SIZE(fpmcu_path));

	if (mmio_dev == NULL)
		return false;

	return mmio_dev->enabled;
}
