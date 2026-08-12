/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/hda.h>
#include <device/azalia_device.h>
#include <device/azalia_codec/realtek.h>

static const u32 realtek_alc256_verbs[] = {
#include <mainboard/starlabs/common/hda/alc256_external_amp_eq.inc>
};

const u32 pc_beep_verbs[] = {};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC256",
		.vendor_id    = 0x10ec0256,
		.subsystem_id = STARLABS_HDA_CODEC_SUBSYSTEM_ID(
			STARLABS_HDA_POLICY_ALC256_EXTERNAL_EQ),
		.address      = 0,
		.verbs        = realtek_alc256_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc256_verbs),
	},
};

AZALIA_ARRAY_SIZES;
