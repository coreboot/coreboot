/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 intel_hdmi_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

/* clang-format off */
struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name = "Intel Ivy Bridge HDMI",
		.vendor_id = 0x80862806,
		.subsystem_id = 0x80860101,
		.address = 3,
		.verbs = intel_hdmi_verbs,
		.verb_count = ARRAY_SIZE(intel_hdmi_verbs),
	},
};
/* clang-format on */

AZALIA_ARRAY_SIZES;
