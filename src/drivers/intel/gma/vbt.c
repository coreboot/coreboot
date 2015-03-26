/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013, 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or (at your option)
 *  any later version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <string.h>
#include <device/pci_rom.h>

#include "i915.h"
#include "intel_bios.h"

static size_t generate_vbt(const struct i915_gpu_controller_info *conf,
		    void *vbt, const char *idstr)
{
	struct vbt_header *head = vbt;
	struct bdb_header *bdb_head;
	struct bdb_general_features *genfeat;
	u8 *ptr;

	memset (head, 0, sizeof (*head));

	memcpy (head->signature, idstr, 20);
	head->version = 100;
	head->header_size = sizeof (*head);
	head->bdb_offset = sizeof (*head);

	bdb_head = (struct bdb_header *) (head + 1);
	memset (bdb_head, 0, sizeof (*bdb_head));
	memcpy (bdb_head->signature, "BIOS_DATA_BLOCK ", 16);
	bdb_head->version = 0xa8;
	bdb_head->header_size = sizeof (*bdb_head);

	ptr = (u8 *) (bdb_head + 1);

	ptr[0] = BDB_GENERAL_FEATURES;
	ptr[1] = sizeof (*genfeat);
	ptr[2] = sizeof (*genfeat) >> 8;
	ptr += 3;

	genfeat = (struct bdb_general_features *) ptr;
	memset (genfeat, 0, sizeof (*genfeat));
	genfeat->panel_fitting = 3;
	genfeat->flexaim = 1;
	genfeat->download_ext_vbt = 1;
	genfeat->enable_ssc = conf->use_spread_spectrum_clock;
	genfeat->ssc_freq = !conf->link_frequency_270_mhz;
	genfeat->rsvd10 = 0x4;
	genfeat->legacy_monitor_detect = 1;
	genfeat->int_crt_support = 1;
	genfeat->dp_ssc_enb = 1;

	ptr += sizeof (*genfeat);

	bdb_head->bdb_size = ptr - (u8 *)bdb_head;
	head->vbt_size = ptr - (u8 *)head;
	head->vbt_checksum = 0;
	return ptr - (u8 *)head;
}

void
generate_fake_intel_oprom(const struct i915_gpu_controller_info *conf,
			  struct device *dev, const char *idstr)
{
		optionrom_header_t *oh = (void *)PCI_VGA_RAM_IMAGE_START;
		optionrom_pcir_t *pcir;
		size_t vbt_size;
		size_t fake_oprom_size;

		memset(oh, 0, 8192);

		oh->signature = PCI_ROM_HDR;
		oh->pcir_offset = 0x40;
		oh->vbt_offset = 0x80;

		pcir = (void *)(PCI_VGA_RAM_IMAGE_START + 0x40);
		pcir->signature = 0x52494350;	// PCIR
		pcir->vendor = dev->vendor;
		pcir->device = dev->device;
		pcir->length = sizeof(*pcir);
		pcir->revision = dev->class;
		pcir->classcode[0] = dev->class >> 8;
		pcir->classcode[1] = dev->class >> 16;
		pcir->classcode[2] = dev->class >> 24;
		pcir->indicator = 0x80;

		vbt_size = generate_vbt (conf, (void *)(PCI_VGA_RAM_IMAGE_START + 0x80), idstr);
		fake_oprom_size = (0x80 + vbt_size + 511) / 512;
		oh->size = fake_oprom_size;
		pcir->imagelength = fake_oprom_size;

}
