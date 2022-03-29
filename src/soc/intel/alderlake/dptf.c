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
	/* _HID for the toplevel TPWR device, typically \_SB.DPTF.TPWR */
	.tpwr_device_hid = "INTC1060",

	.tpch_method_names = {
		.set_fivr_low_clock_method = "RFC0",
		.set_fivr_high_clock_method = "RFC1",
		.get_fivr_low_clock_method = "GFC0",
		.get_fivr_high_clock_method = "GFC1",
		.get_fivr_ssc_method = "GEMI",
		.get_fivr_switching_fault_status = "GFFS",
		.get_fivr_switching_freq_mhz = "GFCS",
	},
};

const struct dptf_platform_info *get_dptf_platform_info(void)
{
	return &adl_dptf_platform_info;
}
