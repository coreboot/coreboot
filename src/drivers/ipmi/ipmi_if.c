/* SPDX-License-Identifier: GPL-2.0-only */

#include "ipmi_if.h"

#include <console/console.h>
#include <delay.h>

#include "chip.h"

int ipmi_get_device_id(const struct device *dev, struct ipmi_devid_rsp *rsp)
{
	int ret;

	ret = ipmi_message(dev->path.pnp.port, IPMI_NETFN_APPLICATION, 0,
			   IPMI_BMC_GET_DEVICE_ID, NULL, 0, (u8 *)rsp,
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

static int ipmi_get_bmc_self_test_result(const struct device *dev,
					 struct ipmi_selftest_rsp *rsp)
{
	int ret;

	ret = ipmi_message(dev->path.pnp.port, IPMI_NETFN_APPLICATION, 0,
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

int ipmi_process_self_test_result(const struct device *dev)
{
	int failure = 0;
	uint8_t retry_count = 0;
	struct ipmi_selftest_rsp selftestrsp = {0};

	const struct drivers_ipmi_config *conf = dev->chip_info;
	uint8_t retry_limit = 0;

	if (conf && conf->wait_for_bmc)
		retry_limit = conf->bmc_boot_timeout;

	if (retry_limit == 0)
		/* Try to get self-test results at least once */
		retry_limit = 1;

	printk(BIOS_INFO, "Get BMC self test result...");
	for (retry_count = 0; retry_count < retry_limit; retry_count++) {
		if (!ipmi_get_bmc_self_test_result(dev, &selftestrsp))
			break;

		mdelay(1000);
	}

	switch (selftestrsp.result) {
	case IPMI_APP_SELFTEST_NO_ERROR: /* 0x55 */
		printk(BIOS_DEBUG, "No Error\n");
		break;
	case IPMI_APP_SELFTEST_NOT_IMPLEMENTED: /* 0x56 */
		printk(BIOS_DEBUG, "Function Not Implemented\n");
		break;
	case IPMI_APP_SELFTEST_ERROR: /* 0x57 */
		printk(BIOS_ERR, "BMC: Corrupted or inaccessible data or device\n");
		failure = 1;
		break;
	case IPMI_APP_SELFTEST_FATAL_HW_ERROR: /* 0x58 */
		printk(BIOS_ERR, "BMC: Fatal Hardware Error\n");
		failure = 1;
		break;
	case IPMI_APP_SELFTEST_RESERVED: /* 0xFF */
		printk(BIOS_DEBUG, "Reserved\n");
		break;

	default: /* Other Device Specific Hardware Error */
		printk(BIOS_ERR, "BMC: Device Specific Error: 0x%02x\n", selftestrsp.result);
		failure = 1;
		break;
	}

	return failure;
}
