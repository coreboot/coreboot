/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "variant.h"

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	// Setup GPIOs
	variant_config_gpios();
}


static void mainboard_enable(struct device *dev)
{
	if (CONFIG(VARIANT_HAS_DGPU)) {
		dgpu_detect();
		dev->ops->acpi_fill_ssdt = ssdt_add_dgpu;
	}
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
