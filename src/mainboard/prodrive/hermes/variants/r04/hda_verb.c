/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <types.h>

#include "variant/variants.h"
#include "eeprom.h"

static const u32 r04_verb_data[] = {
	AZALIA_PIN_CFG(0, 0x19, 0x02a19c20), /* PORT F - front mic in */
	AZALIA_PIN_CFG(0, 0x1a, 0x01813c51), /* PORT C - rear line in (mic support) */
	AZALIA_PIN_CFG(0, 0x1b, 0x411111f0), /* PORT E - disabled */
	AZALIA_PIN_CFG(0, 0x15, 0x411111f0), /* PORT A - disabled */
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

void mainboard_r0x_configure_alc888(u8 *base, u32 viddid)
{
	/* Overwrite settings made by baseboard */
	azalia_program_verb_table(base, r04_verb_data, ARRAY_SIZE(r04_verb_data));

	const struct eeprom_board_settings *const board_cfg = get_board_settings();

	if (!board_cfg)
		return;

	const u32 config = get_internal_audio_cfg(board_cfg->internal_audio_connection);

	const u32 verbs[] = {
		AZALIA_PIN_CFG(0, 0x1b, config),
		0x0205000d,
		get_port_c_vref_cfg(board_cfg->blue_rear_vref),
	};
	azalia_program_verb_table(base, verbs, ARRAY_SIZE(verbs));
}
