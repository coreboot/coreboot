/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/dptf/dptf.h>
#include <soc/dptf.h>
#include <stdbool.h>

static const struct dptf_platform_info tgl_dptf_platform_info = {
	.use_eisa_hids = CONFIG(DPTF_USE_EISA_HID),
	/* _HID for the toplevel DPTF device, typically \_SB.DPTF */
	.dptf_device_hid = DPTF_DPTF_DEVICE,
	/* _HID for Intel DPTF Generic Device (these require PTYP as well) */
	.generic_hid = DPTF_GEN_DEVICE,
	/* _HID for Intel DPTF Fan Device */
	.fan_hid = DPTF_FAN_DEVICE,
};

const struct dptf_platform_info *soc_get_dptf_platform_info(void)
{
	return &tgl_dptf_platform_info;
}
