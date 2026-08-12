/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/hda.h>
#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc269_verbs[] = {
#define STARLABS_HDA_DMIC_LOCATION	(AZALIA_INTERNAL | AZALIA_FRONT)
#define STARLABS_HDA_SPEAKER_LOCATION	AZALIA_FRONT
#include <mainboard/starlabs/common/hda/alc269_vc3.inc>
#include <mainboard/starlabs/common/hda/alc269_vc3_rpl_u_coefficients.inc>
};

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC269",
		.vendor_id    = 0x10ec0269,
		.subsystem_id =
			STARLABS_HDA_CODEC_SUBSYSTEM_ID(STARLABS_HDA_POLICY_ALC269_VC3),
		.address      = 0,
		.verbs        = realtek_alc269_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc269_verbs),
	},
};

AZALIA_ARRAY_SIZES;
