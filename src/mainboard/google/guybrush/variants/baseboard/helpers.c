/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/gpio.h>

WEAK_DEV_PTR(fpmcu);

bool variant_has_fpmcu(void)
{
	return is_dev_enabled(DEV_PTR(fpmcu));
}

bool __weak variant_has_pcie_wwan(void)
{
	return false;
}

uint8_t __weak variant_sd_aux_reset_gpio(void)
{
	return GPIO_5;
}
