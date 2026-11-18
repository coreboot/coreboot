/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/hda.h>
#include <device/azalia_codec/realtek.h>
#include <device/azalia_device.h>
#include <option.h>

#define STARFIGHTER_GPIO2	0x04
#define STARFIGHTER_EAPD	0x02

static const uint32_t speaker_amp_enable_verb[] = {
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

	if (starlabs_hda_use_legacy_subsystem_id() && get_uint_option("firmware_enable_amp", 1))
		azalia_program_verb_table(base, speaker_amp_enable_verb,
					  ARRAY_SIZE(speaker_amp_enable_verb));
}
