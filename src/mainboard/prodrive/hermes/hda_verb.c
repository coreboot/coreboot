/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <types.h>

#include "eeprom.h"

const u32 cim_verb_data[] = {
	0x10ec0888,	/* Codec Vendor / Device ID: Realtek ALC888 */
	0x10ec0888,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x1d336700),

	/* Pin widgets */
	AZALIA_PIN_CFG(0, 0x11, AZALIA_PIN_CFG_NC(0)), /* SPDIF-OUT2 - disabled */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)), /* Digital MIC - disabled */
	AZALIA_PIN_CFG(0, 0x14, 0x01014430),           /* Port D - rear line out */
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)), /* Port G - disabled */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)), /* Port H - disabled */
	AZALIA_PIN_CFG(0, 0x18, 0x01a19c50),           /* Port B - rear mic in */
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)), /* CD audio - disabled */
	AZALIA_PIN_CFG(0, 0x1d, 0x4004c601),           /* BEEPIN */
	AZALIA_PIN_CFG(0, 0x1e, 0x01452160),           /* SPDIF-OUT */
	AZALIA_PIN_CFG(0, 0x1f, 0x01c52170),           /* SPDIF-IN */

	/* Config for R02 and older */
	AZALIA_PIN_CFG(0, 0x19, 0x02214c40), /* Port F - front hp out */
	AZALIA_PIN_CFG(0, 0x1a, 0x901001f0), /* Port C - internal speaker */
	AZALIA_PIN_CFG(0, 0x1b, 0x01813c10), /* Port E - rear line in/mic - Blue */
	AZALIA_PIN_CFG(0, 0x15, 0x02a19c20), /* Port A - audio hdr input */

	/*
	 * VerbTable: CFL Display Audio Codec
	 * Revision ID = 0xff
	 * Codec Vendor: 0x8086280b
	 */
	0x8086280b,
	0xffffffff,
	5,	/* Number of 4 dword sets */

	AZALIA_SUBVENDOR(2, 0x80860101),

	/*
	 * Display Audio Verb Table
	 * For GEN9, the Vendor Node ID is 08h
	 * Port to be exposed to the inbox driver in the vanilla mode
	 * PORT C - BIT[7:6] = 01b
	 */
	0x20878101,

	/* Pin Widget 5 - PORT B - Configuration Default: 0x18560010 */
	AZALIA_PIN_CFG(2, 0x05, 0x18560010),
	/* Pin Widget 6 - PORT C - Configuration Default: 0x18560020 */
	AZALIA_PIN_CFG(2, 0x06, 0x18560020),
	/* Pin Widget 7 - PORT D - Configuration Default: 0x18560030 */
	AZALIA_PIN_CFG(2, 0x07, 0x18560030),
	/* Disable the third converter and third Pin (NID 08h) */
	0x20878100,

	/* Dummy entries */
	0x20878100,
	0x20878100,
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;

static const u32 r04_verb_data[] = {
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c20),           /* Port F - front mic in */
	AZALIA_PIN_CFG(0, 0x1a, 0x01813c51),           /* Port C - rear line in (mic support) */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)), /* Port E - disabled */
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)), /* Port A - disabled */
};

static u32 get_port_c_vref_cfg(uint8_t blue_rear_vref)
{
	switch (blue_rear_vref) {
	default:
	case 0:
		return 0x02040000;
	case 1:
		return 0x02041000;
	case 2:
		return 0x02044000;
	case 3:
		return 0x02045000;
	case 4:
		return 0x02046000;
	}
}

static u32 get_port_b_vref_cfg(uint8_t pink_rear_vref)
{
	switch (pink_rear_vref) {
	default:
	case 0:
		return 0; /* Disabled (Hi-Z) */
	case 1:
		return 1; /* 50% of LDO out */
	case 2:
		return 4; /* 80% of LDO out */
	case 3:
		return 5; /* 100% of LDO out */
	case 4:
		return 2; /* Ground */
	}
}

static u32 get_front_panel_cfg(uint8_t front_panel_audio)
{
	switch (front_panel_audio) {
	default:
	case 0:
		return AZALIA_PIN_CFG_NC(0);
	case 1:
		return 0x02214c40;
	case 2:
		return 0x0227ec40;
	}
}

static u32 get_front_mic_cfg(uint8_t front_panel_audio)
{
	return front_panel_audio == 2 ? AZALIA_PIN_CFG_NC(0) : 0x02a19c20;
}

static void mainboard_r0x_configure_alc888(u8 *base, u32 viddid)
{
	/* Overwrite settings made by baseboard */
	azalia_program_verb_table(base, r04_verb_data, ARRAY_SIZE(r04_verb_data));

	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	const u32 front_panel_cfg = get_front_panel_cfg(board_cfg->front_panel_audio);

	const u32 front_mic_cfg = get_front_mic_cfg(board_cfg->front_panel_audio);

	const u32 port_b_vref_cfg = get_port_b_vref_cfg(board_cfg->pink_rear_vref);

	const u32 verbs[] = {
		/*
		 * Write port B Vref settings to unused non-volatile NID 0x12 instead of
		 * NID 0x18, the actual port B NID. Because per-port Vref settings don't
		 * persist after codec resets, a custom Realtek driver (ab)uses NID 0x12
		 * to restore port B Vref after resetting the codec.
		 */
		AZALIA_PIN_CFG(0, 0x12, 0x411110f0 | port_b_vref_cfg << 8),
		AZALIA_PIN_CFG(0, 0x19, front_mic_cfg),
		AZALIA_PIN_CFG(0, 0x1b, front_panel_cfg),
		0x0205000d, /* Pin 37 vrefo hidden register - used as port C vref */
		get_port_c_vref_cfg(board_cfg->blue_rear_vref),
	};
	azalia_program_verb_table(base, verbs, ARRAY_SIZE(verbs));
}

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	if (viddid == 0x10ec0888) {
		u8 hsi = get_bmc_hsi();

		if (hsi >= 3) /* R04 and newer */
			mainboard_r0x_configure_alc888(base, viddid);
	}
}
