/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_STARLABS_COMMON_HDA_H
#define MAINBOARD_STARLABS_COMMON_HDA_H

#include <option.h>
#include <types.h>

#define STARLABS_HDA_SUBSYSTEM_VENDOR		0x2145

/*
 * Keep zero reserved: coreboot's device model treats a zero subsystem device
 * ID as unset, so devicetree cannot reliably program 0x2145:0x0000.
 */
#define STARLABS_HDA_POLICY_ALC235_STARFIGHTER	0x0001
#define STARLABS_HDA_POLICY_ALC269_VC3		0x0002
#define STARLABS_HDA_POLICY_ALC269_VB6		0x0003
#define STARLABS_HDA_POLICY_LITE_ALC269_VC	0x0004
#define STARLABS_HDA_POLICY_LITE_ALC269_VB	0x0005
#define STARLABS_HDA_POLICY_ALC256_INTERNAL	0x0006
#define STARLABS_HDA_POLICY_ALC256_EXTERNAL_EQ	0x0007

/* Reserved for the PHX external-amplifier profile, which has no RPL EQ data. */
#define STARLABS_HDA_POLICY_ALC256_EXTERNAL	0x0008

#define STARLABS_HDA_PCI_SUBSYSTEM_ID(policy) \
	(((policy) << 16) | STARLABS_HDA_SUBSYSTEM_VENDOR)

#define STARLABS_HDA_CODEC_SUBSYSTEM_ID(policy) \
	((STARLABS_HDA_SUBSYSTEM_VENDOR << 16) | (policy))

enum starlabs_hda_subsystem_id_mode {
	STARLABS_HDA_SUBSYSTEM_ID_CURRENT,
	STARLABS_HDA_SUBSYSTEM_ID_LEGACY,
};

static inline bool starlabs_hda_use_legacy_subsystem_id(void)
{
	return CONFIG_STARLABS_HDA_LEGACY_SUBSYSTEM_ID &&
		get_uint_option("hda_subsystem_id", STARLABS_HDA_SUBSYSTEM_ID_CURRENT) ==
			STARLABS_HDA_SUBSYSTEM_ID_LEGACY;
}

static inline uint32_t starlabs_hda_selected_pci_subsystem_id(uint16_t policy)
{
	if (!starlabs_hda_use_legacy_subsystem_id())
		return STARLABS_HDA_PCI_SUBSYSTEM_ID(policy);

	return ((CONFIG_STARLABS_HDA_LEGACY_SUBSYSTEM_ID & 0xffff) << 16) |
		(CONFIG_STARLABS_HDA_LEGACY_SUBSYSTEM_ID >> 16);
}

void starlabs_hda_program_legacy_subsystem_id_verbs(uint8_t *base, uint32_t viddid);
void starlabs_hda_program_dmic_runtime_verbs(uint8_t *base);

#endif /* MAINBOARD_STARLABS_COMMON_HDA_H */
