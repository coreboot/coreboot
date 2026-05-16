/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

static const u32 sigmatel_stac9221_a1_subsystem_id = 0x106b0f00;

static const u32 sigmatel_stac9221_a1_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x106b0f00),
	AZALIA_PIN_CFG(0, 0x0a, 0x012be032),
	AZALIA_PIN_CFG(0, 0x0b, 0x90afe111),
	AZALIA_PIN_CFG(0, 0x0c, 0x9017e131),
	AZALIA_PIN_CFG(0, 0x0d, 0x4080e10f),
	AZALIA_PIN_CFG(0, 0x0e, 0x40f0e00f),
	AZALIA_PIN_CFG(0, 0x0f, 0x018be021),
	AZALIA_PIN_CFG(0, 0x10, 0x114bf033),
	AZALIA_PIN_CFG(0, 0x11, 0x11cbc022),
	AZALIA_PIN_CFG(0, 0x15, 0x4080e10f),
	AZALIA_PIN_CFG(0, 0x1b, 0x4080e10f),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Sigmatel STAC9221 A1",
		.vendor_id    = 0x83847680,
		.subsystem_id = sigmatel_stac9221_a1_subsystem_id,
		.address      = 0,
		.verbs        = sigmatel_stac9221_a1_verbs,
		.verb_count   = ARRAY_SIZE(sigmatel_stac9221_a1_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
