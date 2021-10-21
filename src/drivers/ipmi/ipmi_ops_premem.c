/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/pnp.h>
#include <delay.h>
#include <timer.h>

#include "ipmi_if.h"
#include "chip.h"

enum cb_err ipmi_premem_init(const u16 port, const u16 device)
{
	const struct drivers_ipmi_config *conf = NULL;
	const struct device *dev;

	/* Find IPMI PNP device from devicetree in romstage */
	dev = dev_find_slot_pnp(port, device);

	if (!dev) {
		printk(BIOS_ERR, "IPMI: Cannot find PNP device port: %x, device %x\n",
			port, device);
		return CB_ERR;
	}
	if (!dev->enabled) {
		printk(BIOS_ERR, "IPMI: device is not enabled\n");
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "IPMI: romstage PNP KCS 0x%x\n", dev->path.pnp.port);
	if (dev->chip_info)
		conf = dev->chip_info;

	if (conf && conf->wait_for_bmc && conf->bmc_boot_timeout) {
		struct stopwatch sw;
		stopwatch_init_msecs_expire(&sw, conf->bmc_boot_timeout * 1000);
		printk(BIOS_DEBUG, "IPMI: Waiting for BMC...\n");

		while (!stopwatch_expired(&sw)) {
			if (inb(dev->path.pnp.port) != 0xff)
				break;
			mdelay(100);
		}
		if (stopwatch_expired(&sw)) {
			printk(BIOS_INFO, "IPMI: Waiting for BMC timed out\n");
			return CB_ERR;
		}
	}

	if (ipmi_process_self_test_result(dev))
		return CB_ERR;

	return CB_SUCCESS;
}
