/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 via_vt2020_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x1458a014),
	AZALIA_PIN_CFG(2, 0x24, 0x01014010),
	AZALIA_PIN_CFG(2, 0x25, 0x410110f0),
	AZALIA_PIN_CFG(2, 0x26, 0x410160f0),
	AZALIA_PIN_CFG(2, 0x27, 0x410120f0),
	AZALIA_PIN_CFG(2, 0x28, 0x0221401f),
	AZALIA_PIN_CFG(2, 0x29, 0x02a19037),
	AZALIA_PIN_CFG(2, 0x2a, 0x0181303e),
	AZALIA_PIN_CFG(2, 0x2b, 0x01a19036),
	AZALIA_PIN_CFG(2, 0x2c, 0x503701f0),
	AZALIA_PIN_CFG(2, 0x2d, 0x474511f0),
	AZALIA_PIN_CFG(2, 0x2e, 0x074521f0),
	AZALIA_PIN_CFG(2, 0x2f, 0x47c521f0),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "VIA VT2020",
		.vendor_id    = 0x11060441,
		.subsystem_id = 0x1458a014,
		.address      = 2,
		.verbs        = via_vt2020_verbs,
		.verb_count   = ARRAY_SIZE(via_vt2020_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862806,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
