/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <device/device.h>
#include <string.h>
#include <device/pci.h>
#include <drivers/intel/gma/opregion.h>

#include "i915.h"
#include "intel_bios.h"

static size_t generate_vbt(const struct i915_gpu_controller_info *const conf,
			   struct vbt_header *const head,
			   const char *const idstr)
{
	u8 *ptr;

	memset(head, 0, sizeof (*head));

	memset(head->signature, ' ', sizeof (head->signature));
	memcpy(head->signature, idstr,
		MIN(strlen(idstr), sizeof (head->signature)));
	head->version = 100;
	head->header_size = sizeof (*head);
	head->bdb_offset = sizeof (*head);

	struct bdb_header *const bdb_head = (struct bdb_header *)(head + 1);
	memset(bdb_head, 0, sizeof (*bdb_head));
	memcpy(bdb_head->signature, "BIOS_DATA_BLOCK ", 16);
	bdb_head->version = 0xa8;
	bdb_head->header_size = sizeof (*bdb_head);

	ptr = (u8 *)(bdb_head + 1);

	ptr[0] = BDB_GENERAL_FEATURES;
	ptr[1] = sizeof (struct bdb_general_features);
	ptr[2] = sizeof (struct bdb_general_features) >> 8;
	ptr += 3;

	struct bdb_general_features *const genfeat =
		(struct bdb_general_features *)ptr;
	memset(genfeat, 0, sizeof (*genfeat));
	genfeat->panel_fitting = 3;
	genfeat->flexaim = 1;
	genfeat->download_ext_vbt = 1;
	genfeat->enable_ssc = conf->use_spread_spectrum_clock;
	genfeat->ssc_freq = CONFIG(INTEL_GMA_SSC_ALTERNATE_REF);
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
generate_fake_intel_oprom(const struct i915_gpu_controller_info *const conf,
			  struct device *const dev, const char *const idstr)
{
	optionrom_header_t *const oh = (void *)PCI_VGA_RAM_IMAGE_START;

	memset(oh, 0, 8192);

	oh->signature = PCI_ROM_HDR;
	oh->pcir_offset = 0x40;
	oh->vbt_offset = 0x80;

	optionrom_pcir_t *const pcir = (void *)((u8 *)oh + oh->pcir_offset);
	pcir->signature = 0x52494350;	// PCIR
	pcir->vendor = dev->vendor;
	pcir->device = dev->device;
	pcir->length = sizeof(*pcir);
	pcir->revision = pci_read_config8(dev, PCI_CLASS_REVISION);
	pcir->classcode[0] = dev->class;
	pcir->classcode[1] = dev->class >> 8;
	pcir->classcode[2] = dev->class >> 16;
	pcir->indicator = 0x80;

	const size_t vbt_size =
		generate_vbt(conf, (void *)((u8 *)oh + oh->vbt_offset), idstr);
	const size_t fake_oprom_size =
		DIV_ROUND_UP(oh->vbt_offset + vbt_size, 512);
	oh->size = fake_oprom_size;
	pcir->imagelength = fake_oprom_size;
}
