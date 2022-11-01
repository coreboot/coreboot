/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <intelblocks/graphics.h>
#include <soc/ramstage.h>

const struct i915_gpu_controller_info *
intel_igd_get_controller_info(const struct device *const dev)
{
	const struct soc_intel_alderlake_config *const chip = dev->chip_info;
	return &chip->gfx;
}
