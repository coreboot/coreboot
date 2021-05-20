/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <fw_config.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

static bool variant_has_device_enabled(const struct device_path *device_path, size_t path_length)
{
	const struct device *dev =
		find_dev_nested_path(all_devices->link_list, device_path, path_length);

	return is_dev_enabled(dev);
}

__weak bool variant_has_fpmcu(void)
{
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

	return variant_has_device_enabled(fpmcu_path, ARRAY_SIZE(fpmcu_path));
}

__weak bool variant_has_pcie_wwan(void)
{
	static const struct device_path pcie_wwan_path[] = {
		{
			.type = DEVICE_PATH_PCI,
			.pci.devfn = PCIE_GPP_2_2_DEVFN,
		},
	};

	return variant_has_device_enabled(pcie_wwan_path, ARRAY_SIZE(pcie_wwan_path));
}
