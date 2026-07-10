/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc888b_verbs[] = {
	/* Pin Widget Verb Table */

	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT2, 0x411110f0),
	AZALIA_PIN_CFG(0, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_FRONT, 0x01014410),
	AZALIA_PIN_CFG(0, ALC887_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_MIC1, 0x01a19c40),
	AZALIA_PIN_CFG(0, ALC887_MIC2, 0x02a19c50),
	AZALIA_PIN_CFG(0, ALC887_LINE1, 0x0181344f),
	AZALIA_PIN_CFG(0, ALC887_LINE2, 0x02214c20),
	AZALIA_PIN_CFG(0, ALC887_CD, 0x593301f0),
	AZALIA_PIN_CFG(0, ALC887_PC_BEEP, 0x4005c603),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT1, 0x014b6130),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_IN, 0x01cb7160),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC888B",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x1458a002,
		.address      = 0,
		.verbs        = realtek_alc888b_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc888b_verbs),
	},
};

AZALIA_ARRAY_SIZES;
