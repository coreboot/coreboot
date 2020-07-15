/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <soc/romstage.h>
#include <console/console.h>

void mainboard_romstage_entry_s3(int s3_resume)
{
	variant_pcie_gpio_configure();
}
