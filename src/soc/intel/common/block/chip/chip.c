/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pci_devs.h>
#include <soc/soc_chip.h>

const struct soc_intel_common_config *chip_get_common_soc_structure(void)
{
	const struct soc_intel_common_config *soc_config;
	const config_t *config;

	config = config_of_soc();
	soc_config = &config->common_soc_config;

	return soc_config;
}
