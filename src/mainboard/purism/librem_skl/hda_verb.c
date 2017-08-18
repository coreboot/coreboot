/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation
 * (Written by Naresh G Solanki <naresh.solanki@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootstate.h>
#include <chip.h>
#include <console/console.h>
#include <device/azalia_device.h>
#include <soc/intel/common/hda_verb.h>
#include <soc/pci_devs.h>

#include "hda_verb.h"

static void codecs_init(u8 *base, u32 codec_mask)
{
	int i;

	/* Can support up to 4 codecs */
	for (i = 3; i >= 0; i--) {
		if (codec_mask & (1 << i))
			hda_codec_init(base, i, cim_verb_data_size,
					cim_verb_data);
	}

	if (pc_beep_verbs_size)
		hda_codec_write(base, pc_beep_verbs_size, pc_beep_verbs);
}

static void mb_hda_codec_init(void *unused)
{
	static struct soc_intel_skylake_config *config;
	u8 *base;
	struct resource *res;
	u32 codec_mask;
	struct device *dev;

	dev = SA_DEV_ROOT;
	/* Check if HDA is enabled, else return */
	if (dev == NULL || dev->chip_info == NULL)
		return;

	config = dev->chip_info;

	/*
	 * IoBufferOwnership 0:HD-A Link, 1:Shared HD-A Link and I2S Port,
	 * 3:I2S Ports. In HDA mode where codec need to be programmed with
	 * verb table
	 */
	if (config->IoBufferOwnership == 3)
		return;

	/* Find base address */
	dev = dev_find_slot(0, PCH_DEVFN_HDA);
	if (dev == NULL)
		return;
	res = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (!res)
		return;

	base = res2mmio(res, 0, 0);
	printk(BIOS_DEBUG, "HDA: base = %p\n", base);

	codec_mask = hda_codec_detect(base);

	if (codec_mask) {
		printk(BIOS_DEBUG, "HDA: codec_mask = %02x\n", codec_mask);
		codecs_init(base, codec_mask);
	}
}

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, mb_hda_codec_init, NULL);
