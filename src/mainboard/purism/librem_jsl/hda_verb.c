/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

static const u32 realtek_alc269_verbs[] = {
	AZALIA_RESET(0x1),

	AZALIA_SUBVENDOR(0, 0x10ec0269),
	AZALIA_PIN_CFG(0, 0x12, 0x90a60130), /* DMIC */
	AZALIA_PIN_CFG(0, 0x14, 0x90170110), /* FRONT */
	AZALIA_PIN_CFG(0, 0x17, 0x40000000), /* N/C */
	AZALIA_PIN_CFG(0, 0x18, 0x04a11020), /* MIC1 */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)), /* N/C */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)), /* N/C */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)), /* N/C */
	AZALIA_PIN_CFG(0, 0x1d, 0x40e38105), /* BEEP */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)), /* N/C */
	AZALIA_PIN_CFG(0, 0x21, 0x0421101f), /* HP-OUT */

	/* EQ */
	0x02050011,
	0x02040710,
	0x02050012,
	0x02041901,

	0x0205000D,
	0x02044440,
	0x02050007,
	0x02040040,

	0x02050002,
	0x0204AAB8,
	0x02050008,
	0x02040300,

	0x02050017,
	0x020400AF,
	0x02050005,
	0x020400C0,
};

static const u32 intel_display_audio_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC269",
		.vendor_id    = 0x10ec0269,
		.subsystem_id = 0x10ec0269,
		.address      = 0,
		.verbs        = realtek_alc269_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc269_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x8086281a,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
