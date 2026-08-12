/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <common/hda.h>
#include <console/console.h>
#include <device/azalia_device.h>
#include <device/pci_ids.h>
#include <static.h>
#include <types.h>

static const uint32_t legacy_subsystem_id_verbs[] = {
	AZALIA_SUBVENDOR(0, CONFIG_STARLABS_HDA_LEGACY_SUBSYSTEM_ID),
};

static void apply_legacy_hda_subsystem_id(void *unused)
{
	struct azalia_codec *codecs;
	const uint32_t subsystem_id = CONFIG_STARLABS_HDA_LEGACY_SUBSYSTEM_ID;
	size_t codec_count;

	(void)unused;

	if (!starlabs_hda_use_legacy_subsystem_id())
		return;

	DEV_PTR(hda)->subsystem_vendor = subsystem_id >> 16;
	DEV_PTR(hda)->subsystem_device = subsystem_id & 0xffff;

	codec_count = azalia_get_mainboard_codecs(&codecs);
	for (size_t i = 0; i < codec_count; i++) {
		if ((codecs[i].vendor_id >> 16) == PCI_VID_REALTEK)
			codecs[i].subsystem_id = subsystem_id;
	}

	printk(BIOS_INFO, "HDA: using legacy subsystem ID %08x\n", subsystem_id);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY,
		      apply_legacy_hda_subsystem_id, NULL);

void starlabs_hda_program_legacy_subsystem_id_verbs(uint8_t *base, uint32_t viddid)
{
	if (starlabs_hda_use_legacy_subsystem_id() &&
	    (viddid >> 16) == PCI_VID_REALTEK)
		azalia_program_verb_table(base, legacy_subsystem_id_verbs,
					  ARRAY_SIZE(legacy_subsystem_id_verbs));
}

void __weak starlabs_hda_program_dmic_runtime_verbs(uint8_t *base)
{
	(void)base;
}

void __weak mainboard_azalia_program_runtime_verbs(uint8_t *base, uint32_t viddid)
{
	starlabs_hda_program_legacy_subsystem_id_verbs(base, viddid);
	starlabs_hda_program_dmic_runtime_verbs(base);
}
