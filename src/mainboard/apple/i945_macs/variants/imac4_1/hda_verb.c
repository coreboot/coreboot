/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

static const u32 sigmatel_stac9221_a1_subsystem_id = 0x106b1700;

static const u32 sigmatel_stac9221_a1_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x106b1700),
	AZALIA_PIN_CFG(0, 0x0a, 0x012b4050),
	AZALIA_PIN_CFG(0, 0x0b, 0x90a00110),
	AZALIA_PIN_CFG(0, 0x0c, 0x90100140),
	AZALIA_PIN_CFG(0, 0x0d, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x0e, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x0f, 0x018b3020),
	AZALIA_PIN_CFG(0, 0x10, 0x014be060),
	AZALIA_PIN_CFG(0, 0x11, 0x01cbe030),
	AZALIA_PIN_CFG(0, 0x15, 0x400000f0),
	AZALIA_PIN_CFG(0, 0x1b, 0x400000f0),
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
};

AZALIA_ARRAY_SIZES;
