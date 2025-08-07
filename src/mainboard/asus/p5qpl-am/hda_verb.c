/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc662_verbs[] = {
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT2, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_FRONT, 0x01014010),
	AZALIA_PIN_CFG(0, ALC887_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_MIC1, 0x01a19840),
	AZALIA_PIN_CFG(0, ALC887_MIC2, 0x02a19850),
	AZALIA_PIN_CFG(0, ALC887_LINE1, 0x0181304f),
	AZALIA_PIN_CFG(0, ALC887_LINE2, 0x02214020),
	AZALIA_PIN_CFG(0, ALC887_CD, 0x593301f0),
	AZALIA_PIN_CFG(0, ALC887_PC_BEEP, 0x4004c601),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT1, 0x99430130),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_IN, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC662",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x1043840b,
		.address      = 0,
		.verbs        = realtek_alc662_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc662_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
