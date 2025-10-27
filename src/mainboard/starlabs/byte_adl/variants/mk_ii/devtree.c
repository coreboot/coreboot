/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <option.h>
#include <static.h>
#include <types.h>
#include <variants.h>
#include <common/powercap.h>

void devtree_update(void)
{
	config_t *cfg = config_of_soc();
	update_power_limits(cfg);

	/* Enable/Disable WiFi based on CMOS settings */
	if (get_uint_option("wifi", 1) == 0)
		DEV_PTR(cnvi_wifi)->enabled = 0;

	/* Enable/Disable Bluetooth based on CMOS settings */
	if (get_uint_option("bluetooth", 1) == 0)
		cfg->usb2_ports[9].enable = 0;

	/* Enable/Disable GNA based on CMOS settings */
	if (get_uint_option("gna", 0) == 0)
		DEV_PTR(gna)->enabled = 0;
}
