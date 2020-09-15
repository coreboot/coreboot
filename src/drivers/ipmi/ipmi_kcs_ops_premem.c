/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/pnp.h>
#include <delay.h>
#include <timer.h>

#include "ipmi_kcs.h"
#include "chip.h"

static int ipmi_get_bmc_self_test_result(const struct device *dev,
	struct ipmi_selftest_rsp *rsp)
{
	int ret;

	ret = ipmi_kcs_message(dev->path.pnp.port, IPMI_NETFN_APPLICATION, 0,
				 IPMI_BMC_GET_SELFTEST_RESULTS, NULL, 0, (u8 *)rsp,
				 sizeof(*rsp));

	if (ret < sizeof(struct ipmi_rsp) || rsp->resp.completion_code) {
		printk(BIOS_ERR, "IPMI: %s command failed (ret=%d resp=0x%x)\n",
		       __func__, ret, rsp->resp.completion_code);
		return 1;
	}
	if (ret != sizeof(*rsp)) {
		printk(BIOS_ERR, "IPMI: %s response truncated\n", __func__);
		return 1;
	}

	return 0;
}

enum cb_err ipmi_kcs_premem_init(const u16 port, const u16 device)
{
	const struct drivers_ipmi_config *conf = NULL;
	struct ipmi_selftest_rsp selftestrsp = {0};
	uint8_t retry_count;
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

	printk(BIOS_INFO, "Get BMC self test result...");
	if (conf && conf->bmc_boot_timeout) {
		for (retry_count = 0; retry_count < conf->bmc_boot_timeout; retry_count++) {
			if (!ipmi_get_bmc_self_test_result(dev, &selftestrsp))
				break;

			mdelay(1000);
		}
	} else {
		/* At least run once */
		ipmi_get_bmc_self_test_result(dev, &selftestrsp);
	}

	int ret = CB_ERR;
	switch (selftestrsp.result) {
	case IPMI_APP_SELFTEST_NO_ERROR: /* 0x55 */
		printk(BIOS_DEBUG, "No Error\n");
		ret = CB_SUCCESS;
		break;
	case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: /* 0x56 */
		printk(BIOS_DEBUG, "Function Not Implemented\n");
		ret = CB_SUCCESS;
		break;
	case IPMI_APP_SELFTEST_ERROR: /* 0x57 */
		printk(BIOS_ERR, "Corrupted or inaccessible data or device\n");
		break;
	case IPMI_APP_SELFTEST_FATAL_HW_ERROR: /* 0x58 */
		printk(BIOS_ERR, "Fatal Hardware Error\n");
		break;
	case IPMI_APP_SELFTEST_RESERVED: /* 0xFF */
		printk(BIOS_DEBUG, "Reserved\n");
		ret = CB_SUCCESS;
		break;

	default: /* Other Device Specific Hardware Error */
		printk(BIOS_ERR, "Device Specific Error 0x%x 0x%x\n", selftestrsp.result,
			selftestrsp.param);
		break;
	}
	return ret;
}
