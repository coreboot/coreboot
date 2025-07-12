/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/gpio.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/acpi/ec.h>
#include <option.h>
#include <types.h>

#include "h8.h"
#include "chip.h"

/*
 * Controls BDC (Bluetooth daughter card) power.
 */
void h8_bluetooth_enable(int on)
{
	if (on)
		ec_set_bit(0x3a, 4);
	else
		ec_clr_bit(0x3a, 4);
}

/*
 * Detect BDC on supported MBs.
 */
bool h8_has_bdc(const struct device *dev)
{
	struct ec_lenovo_h8_config *conf = dev->chip_info;

	if (!CONFIG(H8_HAS_BDC_GPIO_DETECTION)) {
		printk(BIOS_INFO, "H8: BDC detection not implemented. "
				  "Assuming BDC installed\n");
		return true;
	}

	if (get_gpio(conf->bdc_gpio_num) == conf->bdc_gpio_lvl) {
		printk(BIOS_INFO, "H8: BDC installed\n");
		return true;
	}

	printk(BIOS_INFO, "H8: BDC not installed\n");
	return false;
}

/*
 * Return BDC NVRAM setting.
 */
bool h8_bluetooth_nv_enable(void)
{
	return get_uint_option("bluetooth", true);
}
