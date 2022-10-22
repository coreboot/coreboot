/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/dptf/dptf.h>
#include <stdbool.h>

static const struct dptf_platform_info tgl_dptf_platform_info = {
	.use_eisa_hids = false,
	/* _HID for the toplevel DPTF device, typically \_SB.DPTF */
	.dptf_device_hid = "INTC1040",
	/* _HID for Intel DPTF Generic Device (these require PTYP as well) */
	.generic_hid = "INTC1043",
	/* _HID for Intel DPTF Fan Device */
	.fan_hid = "INTC1044",
};

const struct dptf_platform_info *get_dptf_platform_info(void)
{
	return &tgl_dptf_platform_info;
}
