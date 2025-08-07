/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 via_vt1708s_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x1043836c),
	AZALIA_PIN_CFG(0, 0x19, 0x01011012),
	AZALIA_PIN_CFG(0, 0x1a, 0x01a19036),
	AZALIA_PIN_CFG(0, 0x1b, 0x0181303e),
	AZALIA_PIN_CFG(0, 0x1c, 0x01014010),
	AZALIA_PIN_CFG(0, 0x1d, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x1e, 0x02a19038),
	AZALIA_PIN_CFG(0, 0x1f, 0x503701f0),
	AZALIA_PIN_CFG(0, 0x20, 0x185600f0),
	AZALIA_PIN_CFG(0, 0x21, 0x074521f0),
	AZALIA_PIN_CFG(0, 0x22, 0x01016011),
	AZALIA_PIN_CFG(0, 0x23, 0x01012014),
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),

};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "VIA VT1708S",
		.vendor_id    = 0x11060397,
		.subsystem_id = 0x1043836c,
		.address      = 0,
		.verbs        = via_vt1708s_verbs,
		.verb_count   = ARRAY_SIZE(via_vt1708s_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862806,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
