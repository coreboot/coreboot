/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/hda.h>
#include <device/azalia_codec/realtek.h>
#include <device/azalia_device.h>
#include <option.h>

#define STARFIGHTER_GPIO2	0x04
#define STARFIGHTER_EAPD	0x02

static const uint32_t speaker_amp_enable_verb[] = {
	0x05750003,
	0x057486a6,
	0x02050034,
	0x02048204,

	0x0205001b,
	0x02040a0b,
	0x02050046,
	0x02040004,

	0x02050008,
	0x02046a0c,
	0x02050040,
	0x02041800,

	0x02050037,
	0x02044a06,
	0x0205004c,
	0x02044803,

	0x02050019,
	0x02040a10,
	0x02050035,
	0x020488aa,

	AZALIA_VERB_12B(0, 0x01, 0x716, STARFIGHTER_GPIO2),
	AZALIA_VERB_12B(0, 0x01, 0x717, STARFIGHTER_GPIO2),
	AZALIA_VERB_12B(0, 0x01, 0x715, STARFIGHTER_GPIO2),
	AZALIA_VERB_12B(0, ALC269_LINE2, 0x70c, STARFIGHTER_EAPD),
};

static const uint32_t microphone_disable_verb[] = {
	AZALIA_PIN_CFG(0, ALC269_DMIC12, AZALIA_PIN_CFG_NC(0)),
};

void mainboard_azalia_program_runtime_verbs(uint8_t *base, uint32_t viddid)
{
	starlabs_hda_program_legacy_subsystem_id_verbs(base, viddid);

	if (get_uint_option("microphone", 1) == 0)
		azalia_program_verb_table(base, microphone_disable_verb,
					  ARRAY_SIZE(microphone_disable_verb));

	if (get_uint_option("firmware_enable_amp", 1))
		azalia_program_verb_table(base, speaker_amp_enable_verb,
					  ARRAY_SIZE(speaker_amp_enable_verb));
}
