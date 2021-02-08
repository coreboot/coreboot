/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <types.h>

#include "variants/baseboard/include/eeprom.h"

const u32 cim_verb_data[] = {
	0x10ec0888,	/* Codec Vendor / Device ID: Realtek ALC888 */
	0x10ec0888,	/* Subsystem ID */
	10,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x10ec0888),

	/* Port A: Front, Audio Header #1 */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_DESC(
		INTEGRATED,
		INTERNAL,
		SPECIAL7,
		LINE_OUT,
		OTHER_ANALOG,
		COLOR_UNKNOWN,
		false,
		0xf,
		0
	)),
	/* Port B: Mic1, Line input (pink)  */
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_DESC(
		JACK,
		EXTERNAL_PRIMARY_CHASSIS,
		REAR, //FIXME
		LINE_IN,
		STEREO_MONO_1_8,
		PINK,
		false,
		0xf,
		0
	)),
	/* Port C: Line, Speaker (only L) */
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_DESC(
		INTEGRATED,
		INTERNAL,
		NA,
		SPEAKER,
		TYPE_UNKNOWN,
		COLOR_UNKNOWN,
		false,
		0xf,
		0
	)),
	/* Port D: Sidesurr, Line out (green) */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_DESC(
		//JACK,
		LINE_OUT,
		EXTERNAL_PRIMARY_CHASSIS,
		REAR, //FIXME
		LINE_OUT,
		STEREO_MONO_1_8,
		GREEN,
		false,
		0xf,
		0
	)),
	/* Port E: Line2, Line input (blue) */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_DESC(
		JACK,
		EXTERNAL_PRIMARY_CHASSIS,
		REAR, //FIXME
		LINE_IN,
		STEREO_MONO_1_8,
		BLUE,
		false,
		0xf,
		0
	)),
	/* Port F: Mic2, Audio Header #2 */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_DESC(
		INTEGRATED,
		INTERNAL,
		SPECIAL7,
		LINE_IN,
		OTHER_ANALOG,
		COLOR_UNKNOWN,
		false,
		0xf,
		0
	)),
	/* Port G: Not connected */
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_DESC(
		NC,
		LOCATION_OTHER,
		NA,
		DEVICE_OTHER,
		TYPE_OTHER,
		COLOR_OTHER,
		true,
		0,
		0
	)),
	/* Port H: Not connected */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_DESC(
		NC,
		LOCATION_OTHER,
		NA,
		DEVICE_OTHER,
		TYPE_OTHER,
		COLOR_OTHER,
		true,
		0,
		0
	)),

	/* S/PDIF-OUT */
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_DESC(
		INTEGRATED,
		INTERNAL,
		NA,
		SPDIF_OUT,
		OTHER_DIGITAL,
		COLOR_UNKNOWN,
		true,
		0xf,
		0
	)),
	/* S/PDIF-IN */
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_DESC(
		INTEGRATED,
		INTERNAL,
		NA,
		SPDIF_IN,
		OTHER_DIGITAL,
		COLOR_UNKNOWN,
		true,
		0xf,
		0
	)),

	//FIXME COdec ID
	/*
	 * VerbTable: CFL Display Audio Codec
	 * Revision ID = 0xFF
	 * Codec Vendor: 0x8086280B
	 */

	0x8086280B,
	0xFFFFFFFF,
	5,	/* Number of 4 dword sets */

	/*
	 * Display Audio Verb Table
	 * For GEN9, the Vendor Node ID is 08h
	 * Port to be exposed to the inbox driver in the vanilla mode
	 * PORT C - BIT[7:6] = 01b
	 */
	0x00878140,
	0x00878140,
	0x00878140,
	0x00878140,
	/* Pin Widget 5 - PORT B - Configuration Default: 0x18560010 */
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	/* Pin Widget 6 - PORT C - Configuration Default: 0x18560020 */
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	/* Pin Widget 7 - PORT D - Configuration Default: 0x18560030 */
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
	/* Disable the third converter and third Pin (NID 08h) */
	0x00878140,
	0x00878140,
	0x00878140,
	0x00878140,
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;

static u32 get_internal_audio_cfg(uint8_t internal_audio_connection)
{
	switch (internal_audio_connection) {
	default:
	case 0:
		return AZALIA_PIN_CFG_NC(0);
	case 1:
		return 0x022a4c40;
	case 2:
		return AZALIA_PIN_DESC(
			INTEGRATED,
			INTERNAL,
			NA,
			SPEAKER,
			TYPE_UNKNOWN,
			COLOR_UNKNOWN,
			false,
			0xf,
			0);
	}
}

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	if (viddid != 0x10ec0888)
		return;

	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	const u32 config = get_internal_audio_cfg(board_cfg->internal_audio_connection);

	const u32 verbs[] = {
		AZALIA_PIN_CFG(0, 0x1b, config),
	};

	azalia_program_verb_table(base, verbs, ARRAY_SIZE(verbs));
}
