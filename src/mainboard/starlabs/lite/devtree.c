/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/turbo.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <option.h>
#include <static.h>
#include <types.h>
#include <variants.h>
#include <common/powercap.h>

#include "soc/intel/apollolake/chip.h"

void devtree_update(void)
{
	config_t *cfg = config_of_soc();

	struct device *wifi_dev = pcidev_on_root(0x0c, 0);

	update_power_limits(cfg);

	/* Enable/Disable WiFi based on CMOS settings */
	if (get_uint_option("wifi", 1) == 0)
		wifi_dev->enabled = 0;

	/* Enable/Disable Webcam based on CMOS settings */
	cfg->usb2_port[CONFIG_WEBCAM_USB_PORT].enable = get_uint_option("webcam", 1);
}
