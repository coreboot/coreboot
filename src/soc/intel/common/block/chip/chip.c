/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/soc_chip.h>

struct soc_intel_common_config *chip_get_common_soc_structure(void)
{
	return &((config_t *)config_of_soc())->common_soc_config;
}
