/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

#if CONFIG(BOARD_APPLE_MACBOOK11) || CONFIG(BOARD_APPLE_MACBOOK21)
static const u32 sigmatel_stac9221_a1_subsystem_id = 0x106b2200;

static const u32 sigmatel_stac9221_a1_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x106b2200),
	AZALIA_PIN_CFG(0, 0x0a, 0x0321e21f),
	AZALIA_PIN_CFG(0, 0x0b, 0x03a1e02e),
	AZALIA_PIN_CFG(0, 0x0c, 0x9017e110),
	AZALIA_PIN_CFG(0, 0x0d, 0x9017e11f),
	AZALIA_PIN_CFG(0, 0x0e, 0x400000fe),
	AZALIA_PIN_CFG(0, 0x0f, 0x0381e020),
	AZALIA_PIN_CFG(0, 0x10, 0x1345e230),
	AZALIA_PIN_CFG(0, 0x11, 0x13c5e240),
	AZALIA_PIN_CFG(0, 0x15, 0x400000fc),
	AZALIA_PIN_CFG(0, 0x1b, 0x400000fb),
};
#else /* CONFIG_BOARD_APPLE_IMAC52 */
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
#endif

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
