/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_codec/realtek.h>
#include <device/azalia_device.h>
#include <option.h>
#include <types.h>

static const uint32_t override_verb[] = {
	AZALIA_PIN_CFG(0, ALC269_DMIC12, AZALIA_PIN_CFG_NC(0)),
};

static void disable_microphone(uint8_t *base)
{
	azalia_program_verb_table(base, override_verb, ARRAY_SIZE(override_verb));
}

void mainboard_azalia_program_runtime_verbs(uint8_t *base, uint32_t viddid)
{
	if (get_uint_option("microphone", 1) == 0)
		disable_microphone(base);
}
