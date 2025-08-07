/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 via_vt1708s_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x10438415),
	AZALIA_PIN_CFG(0, 0x19, 0x410110f0),
	AZALIA_PIN_CFG(0, 0x1a, 0x01a19036),
	AZALIA_PIN_CFG(0, 0x1b, 0x0181303e),
	AZALIA_PIN_CFG(0, 0x1c, 0x01014010),
	AZALIA_PIN_CFG(0, 0x1d, 0x0221401f),
	AZALIA_PIN_CFG(0, 0x1e, 0x02a19037),
	AZALIA_PIN_CFG(0, 0x1f, 0x503701f0),
	AZALIA_PIN_CFG(0, 0x20, 0x585600f0),
	AZALIA_PIN_CFG(0, 0x21, 0x474411f0),
	AZALIA_PIN_CFG(0, 0x22, 0x410160f0),
	AZALIA_PIN_CFG(0, 0x23, 0x410120f0),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "VIA VT1708S",
		.vendor_id    = 0x11060397,
		.subsystem_id = 0x10438415,
		.address      = 0,
		.verbs        = via_vt1708s_verbs,
		.verb_count   = ARRAY_SIZE(via_vt1708s_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
