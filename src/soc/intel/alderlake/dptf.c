/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/dptf/dptf.h>

static const struct dptf_platform_info adl_dptf_platform_info = {
	.use_eisa_hids = false,
	/* _HID for the toplevel DPTF device, typically \_SB.DPTF */
	.dptf_device_hid = "INTC1041",
	/* _HID for Intel DPTF Generic Device (these require PTYP as well) */
	.generic_hid = "INTC1046",
	/* _HID for Intel DPTF Fan Device */
	.fan_hid = "INTC1048",
	/* _HID for the toplevel TPCH device, typically \_SB.TPCH */
	.tpch_device_hid = "INTC1049",
	/* RFC0 method name */
	.tpch_rfc0_method = "RFC0",
	/* RFC1 method name */
	.tpch_rfc1_method = "RFC1",
};

const struct dptf_platform_info *get_dptf_platform_info(void)
{
	return &adl_dptf_platform_info;
}
