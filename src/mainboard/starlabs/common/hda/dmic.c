/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/hda.h>
#include <device/azalia_codec/realtek.h>
#include <device/azalia_device.h>
#include <option.h>
#include <types.h>

static const uint32_t microphone_disable_verbs[] = {
	AZALIA_PIN_CFG(0, ALC269_DMIC12, AZALIA_PIN_CFG_NC(0)),
};

void starlabs_hda_program_dmic_runtime_verbs(uint8_t *base)
{
	if (get_uint_option("microphone", 1) == 0)
		azalia_program_verb_table(base, microphone_disable_verbs,
					  ARRAY_SIZE(microphone_disable_verbs));
}
