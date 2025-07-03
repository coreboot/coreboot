/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/pci_ops.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <soc/pci_devs.h>
#include <static.h>
#include <intelblocks/power_limit.h>

struct soc_power_limits_config *variant_get_soc_power_limit_config(void)
{
	config_t *config = config_of_soc();
	size_t i;
	struct device *sa = pcidev_path_on_root(SA_DEVFN_ROOT);
	uint16_t sa_pci_id;
	u8 tdp;

	if (!sa)
		return NULL;

	sa_pci_id = pci_read_config16(sa, PCI_DEVICE_ID);

	if (sa_pci_id == 0xffff)
		return NULL;

	tdp = get_cpu_tdp();

	for (i = 0; i < ARRAY_SIZE(cpuid_to_adl); i++) {
		if (sa_pci_id == cpuid_to_adl[i].cpu_id &&
				tdp == cpuid_to_adl[i].cpu_tdp) {
			return &config->power_limits_config[cpuid_to_adl[i].limits];
		}
	}

	return NULL;
}
