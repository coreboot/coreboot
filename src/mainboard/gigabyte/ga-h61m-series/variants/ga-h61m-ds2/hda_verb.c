/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc887_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x1458a002),
	AZALIA_PIN_CFG(2, ALC887_SPDIF_OUT2, 0x411110f0),
	AZALIA_PIN_CFG(2, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_FRONT, 0x01014410),
	AZALIA_PIN_CFG(2, ALC887_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_CENTER_LFE, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_SIDE_SURROUND, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_MIC1, 0x01a19c20),
	AZALIA_PIN_CFG(2, ALC887_MIC2, 0x02a19c30),
	AZALIA_PIN_CFG(2, ALC887_LINE1, 0x0181342f),
	AZALIA_PIN_CFG(2, ALC887_LINE2, 0x02214c1f),
	AZALIA_PIN_CFG(2, ALC887_CD, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_PC_BEEP, 0x4004c601),
	AZALIA_PIN_CFG(2, ALC887_SPDIF_OUT1, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(2, ALC887_SPDIF_IN, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC887",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x1458a002,
		.address      = 2,
		.verbs        = realtek_alc887_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc887_verbs),
	},
};

AZALIA_ARRAY_SIZES;
