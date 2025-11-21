/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci.h>
#include <soc/pci_devs.h>
#include <static.h>
#include <tdp.h>

enum soc_intel_pantherlake_cpu_tdps soc_get_cpu_tdp(void)
{
	const struct soc_intel_pantherlake_config *config = config_of_soc();
	return config->tdp || get_cpu_tdp();
}
