/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <ec/acpi/ec.h>
#include <gpio.h>
#include <option.h>
#include <types.h>

#include "h8.h"
#include "chip.h"

/* Controls radio-off pin in WWAN MiniPCIe slot.  */
void h8_wwan_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 6);
	else
		ec_clr_bit(0x3a, 6);
}

/*
 * Detect WWAN on supported MBs.
 */
bool h8_has_wwan(const struct device *dev)
{
	struct ec_lenovo_h8_config *conf = dev->chip_info;

	if (!CONFIG(H8_HAS_WWAN_GPIO_DETECTION)) {
		printk(BIOS_INFO, "H8: WWAN detection not implemented. "
				  "Assuming WWAN installed\n");
		return true;
	}

	if (gpio_get(conf->wwan_gpio_num) == conf->wwan_gpio_lvl) {
		printk(BIOS_INFO, "H8: WWAN installed\n");
		return true;
	}

	printk(BIOS_INFO, "H8: WWAN not installed\n");
	return false;
}

/*
 * Return WWAN NVRAM setting.
 */
bool h8_wwan_nv_enable(void)
{
	return get_uint_option("wwan", true);
}
