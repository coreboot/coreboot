/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 cirrus_logic_cs4210_verbs[] = {
	/* NID 0x01, HDA Codec Subsystem ID Verb Table: 0x10134210 */
	AZALIA_SUBVENDOR(0, 0x10134210),

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x05)     1/8   Gray  HP Out at Ext Front */
	AZALIA_PIN_CFG(0, 0x05, 0x022120f0),

	/* Pin Complex (NID 0x06)  Analog Unknown  Speaker at Int N/A */
	AZALIA_PIN_CFG(0, 0x06, 0x90170010),

	/* Pin Complex (NID 0x07)     1/8    Grey  Line In at Ext Front */
	AZALIA_PIN_CFG(0, 0x07, 0x02a120f0),

	/* Pin Complex (NID 0x08)  Analog Unknown  Mic at Oth Mobile-In */
	AZALIA_PIN_CFG(0, 0x08, 0x77a70037),

	/* Pin Complex (NID 0x09) Digital Unknown  Mic at Oth Mobile-In */
	AZALIA_PIN_CFG(0, 0x09, 0x77a6003e),

	/* Pin Complex (NID 0x0a) Optical   Black  SPDIF Out at Ext N/A */
	AZALIA_PIN_CFG(0, 0x0a, 0x434510f0),
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

const u32 pc_beep_verbs[] = {
	0x00170500,			/* power up codec */
	0x00270500,			/* power up DAC */
	0x00670500,			/* power up speaker */
	0x00670740,			/* enable speaker output */
	0x0023B04B,			/* set DAC gain */
};

static struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Cirrus Logic CS4210",
		.vendor_id    = 0x10134210,
		.subsystem_id = 0x10134210,
		.address      = 0,
		.verbs        = cirrus_logic_cs4210_verbs,
		.verb_count   = ARRAY_SIZE(cirrus_logic_cs4210_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862805,
		.subsystem_id = 0x80860101,
		.address      = 3,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
};

AZALIA_ARRAY_SIZES;
