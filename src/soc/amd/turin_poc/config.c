/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/chip.h>
#include <device/device.h>
#include <static.h>
#include "chip.h"

const struct soc_amd_common_config *soc_get_common_config(void)
{
	const struct soc_amd_turin_poc_config *cfg = config_of_soc();
	return &cfg->common_config;
}
