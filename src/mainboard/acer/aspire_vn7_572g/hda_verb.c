/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Convert to macros */

#include <device/azalia_device.h>

static const u32 realtek_alc255_verbs[] = {
	/* --- Codec #0 --- */

	/* Codec Address: Bits 31:28 */
	/* Node ID: Bits 27:20 */
	/* Verb ID: Bits 19:8 / Bits 19:16 */
	/* Payload: Bits 7:0  / Bits 15:0 */

	/* Reset Codec */
	AZALIA_RESET(0x1),
	/* NOTE: Corrected the table in vendor FW, programming subsystem after reset */
	/* HDA Codec Subsystem ID Verb Table */
	AZALIA_SUBVENDOR(0, 0x10251037),

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(0, 0x12, 0x411111c0),
	AZALIA_PIN_CFG(0, 0x14, 0x90172120),	/* Speaker */
	AZALIA_PIN_CFG(0, 0x17, 0x40000000),
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x40700001),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x21, 0x02211030),	/* Headphone */

	/*
	 * See data blob in "InstallPchHdaVerbTablePei" of vendor firmware
	 * (some appear in https://github.com/torvalds/linux/blob/master/sound/pci/hda/patch_realtek.c).
	 * - Largely coefficient programming (undocumented): Select coeff; write data
	 * - Also programs speaker amplifier gain
	 * - Sets speaker output
	 * NOTE: NID 0x20 holds the "Realtek Defined Hidden registers"
	 */
	0x02050038,	/* Set coeff idx: 0x38 */
	0x02048981,	/* Set processing coeff: 0x8981 */
	0x02050045,	/* Set coeff idx: 0x45 */
	0x0204c489,	/* Set processing coeff: 0xc489 */

	0x02050037,	/* Set coeff idx: 0x37 */
	0x02044a05,	/* Set processing coeff: 0x4a05 */
	0x05750003,	/* Set coeff idx on NID 0x57?: 0x3 */
	0x057486a6,	/* Set processing coeff on NID 0x57?: 0x86a6 */

	0x02050046,	/* Set coeff idx: 0x46 */
	0x02040004,	/* Set processing coeff: 0x4 */
	0x0205001b,	/* Set coeff idx: 0x1b */
	0x02040a0b,	/* Set processing coeff: 0xa0b */

	0x02050008,	/* Set coeff idx: 0x8 */
	0x02046a0c,	/* Set processing coeff: 0x6a0c */
	0x02050009,	/* Set coeff idx: 0x9 */
	0x0204e003,	/* Set processing coeff: 0xe003 */

	0x0205000a,	/* Set coeff idx: 0xa */
	0x02047770,	/* Set processing coeff: 0x7770 */
	0x02050040,	/* Set coeff idx: 0x40 */
	0x02049800,	/* Set processing coeff: 0x9800 */

	0x02050010,	/* Set coeff idx: 0x10 */
	0x02040e20,	/* Set processing coeff: 0xe20 */
	0x0205000d,	/* Set coeff idx: 0xd */
	0x02042801,	/* Set processing coeff: 0x2801 */

	0x0143b000,	/* Sends unknown verb 0x3B to speaker */
	0x0143b000,	/* Repeated for units? */
	0x01470740,	/* Set widget control on speaker: Output; VrefEn: Hi-Z (disabled) */
	0x01470740,	/* Repeated for units? */

	0x01470740,	/* Repeated for units? */
	0x01470740,	/* Repeated for units? */
	0x02050010,	/* Set coeff idx: 0x10 */
	0x02040f20,	/* Set processing coeff: 0xf20 */
};

static const u32 intel_display_audio_verbs[] = {
	/* --- Codec #2 --- */

	/* Codec Address: Bits 31:28 */
	/* Node ID: Bits 27:20 */
	/* Verb ID: Bits 19:8 */
	/* Payload: Bits 7:0 */

	/* NOTE: Corrected the table in vendor FW, using codec address 0x2, not 0x0 */

	/* Enable the third converter and pin first */
	0x20878101,
	0x20878101,
	0x20878101,
	0x20878101,

	/* Pin Widget Verb Table */
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),

	/* Disable the third converter and third pin */
	0x20878100,
	0x20878100,
	0x20878100,
	0x20878100,
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "Realtek ALC255",
		.vendor_id    = 0x10ec0255,
		.subsystem_id = 0x10251037,
		.address      = 0,
		.verbs        = realtek_alc255_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc255_verbs),
	},
	{
		.name         = "Intel Display Audio (HDMI/DP)",
		.vendor_id    = 0x80862809,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_display_audio_verbs,
		.verb_count   = ARRAY_SIZE(intel_display_audio_verbs),
	},
	{ /* terminator */ }
};

AZALIA_ARRAY_SIZES;
