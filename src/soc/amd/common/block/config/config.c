/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/chip.h>
#include <soc/soc_chip.h>
#include <static.h>

const struct soc_amd_common_config *soc_get_common_config(void)
{
	const soc_amd_config_t *config = config_of_soc();

	return &config->common_config;
}
