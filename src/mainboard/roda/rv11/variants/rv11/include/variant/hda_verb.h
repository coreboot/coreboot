/* SPDX-License-Identifier: GPL-2.0-only */

static const u32 realtek_alc262_verbs[] = {
	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10ec0262 */
	AZALIA_SUBVENDOR(0, 0x1a864352),

	AZALIA_PIN_CFG(0, 0x11, 0x40000000),

	AZALIA_PIN_CFG(0, 0x12, 0x90a60140),

	AZALIA_PIN_CFG(0, 0x14, 0x90170110),

	AZALIA_PIN_CFG(0, 0x15, 0x01214020),

	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),

	AZALIA_PIN_CFG(0, 0x18, 0x01a19030),

	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),

	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),

	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),

	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),

	AZALIA_PIN_CFG(0, 0x1d, 0x4036a235),

	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),

	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),
};

static const u32 intel_display_audio_verbs[] = {
	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x80860101 */
	AZALIA_SUBVENDOR(3, 0x80860101),

	/* Pin Complex (NID 0x05) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x05, 0x18560010),

	/* Pin Complex (NID 0x06) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x06, 0x18560020),

	/* Pin Complex (NID 0x07) Digital Out at Int HDMI */
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),
};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC262",
		.vendor_id    = 0x10ec0262,
		.subsystem_id = 0x1a864352,
		.address      = 0,
		.verbs        = realtek_alc262_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc262_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862806,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};
