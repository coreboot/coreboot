/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <fw_config.h>
#include <soc/soc_chip.h>

static void ext_vr_update(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();
	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_ABSENT)))
		cfg->disable_external_bypass_vr = 1;
}

void variant_devtree_update(void)
{
	ext_vr_update();
}
