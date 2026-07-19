/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc887_verbs[] = {
	AZALIA_SUBVENDOR(0, 0x14627707),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT2, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_DMIC_LR, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_FRONT, 0x01014410),
	AZALIA_PIN_CFG(0, ALC887_SURROUND, 0x01011412),
	AZALIA_PIN_CFG(0, ALC887_CENTER_LFE, 0x01016411),
	AZALIA_PIN_CFG(0, ALC887_SIDE_SURROUND, 0x01012414),
	AZALIA_PIN_CFG(0, ALC887_MIC1, 0x01813c40),
	AZALIA_PIN_CFG(0, ALC887_MIC2, 0x02a19c50),
	AZALIA_PIN_CFG(0, ALC887_LINE1, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_LINE2, 0x02214c20),
	AZALIA_PIN_CFG(0, ALC887_CD, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_PC_BEEP, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_OUT1, 0x01454130),
	AZALIA_PIN_CFG(0, ALC887_SPDIF_IN, AZALIA_PIN_CFG_NC(0)),
};

const u32 pc_beep_verbs[0] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC887",
		.vendor_id    = 0x10ec0887,
		.subsystem_id = 0x14627707,
		.address      = 0,
		.verbs        = realtek_alc887_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc887_verbs),
	},
};

AZALIA_ARRAY_SIZES;
