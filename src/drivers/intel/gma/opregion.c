/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2, or (at your option)
 * any later verion of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <types.h>
#include <string.h>
#include <cbfs.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <cbmem.h>
#include "intel_bios.h"
#include "opregion.h"

__weak
const char *mainboard_vbt_filename(void)
{
	return "vbt.bin";
}

static char vbt_data[8 * KiB];
static int vbt_data_used;

void *locate_vbt(size_t *vbt_size)
{
	uint32_t vbtsig = 0;

	if (vbt_data_used == 1)
		return (void *)vbt_data;

	const char *filename = mainboard_vbt_filename();

	size_t file_size = cbfs_boot_load_file(filename,
		vbt_data, sizeof(vbt_data), CBFS_TYPE_RAW);

	if (file_size == 0)
		return NULL;

	if (vbt_size)
		*vbt_size = file_size;

	memcpy(&vbtsig, vbt_data, sizeof(vbtsig));
	if (vbtsig != VBT_SIGNATURE) {
		printk(BIOS_ERR, "Missing/invalid signature in VBT data file!\n");
		return NULL;
	}

	printk(BIOS_INFO, "Found a VBT of %zu bytes after decompression\n",
		file_size);
	vbt_data_used = 1;

	return (void *)vbt_data;
}

/* Write ASLS PCI register and prepare SWSCI register. */
void intel_gma_opregion_register(uintptr_t opregion)
{
	struct device *igd;
	u16 reg16;
	u16 sci_reg;

	igd = dev_find_slot(0, PCI_DEVFN(0x2, 0));
	if (!igd || !igd->enabled)
		return;

	/*
	 * Intel BIOS Specification
	 * Chapter 5.3.7 "Initialize Hardware State"
	 */
	pci_write_config32(igd, ASLS, opregion);

	/*
	 * Atom-based platforms use a combined SMI/SCI register,
	 * whereas non-Atom platforms use a separate SCI register.
	 */
	if (IS_ENABLED(CONFIG_INTEL_GMA_SWSMISCI))
		sci_reg = SWSMISCI;
	else
		sci_reg = SWSCI;

	/*
	 * Intel's Windows driver relies on this:
	 * Intel BIOS Specification
	 * Chapter 5.4 "ASL Software SCI Handler"
	 */
	reg16 = pci_read_config16(igd, sci_reg);
	reg16 &= ~GSSCIE;
	reg16 |= SMISCISEL;
	pci_write_config16(igd, sci_reg, reg16);
}

/* Restore ASLS register on S3 resume and prepare SWSCI. */
void intel_gma_restore_opregion(void)
{
	if (acpi_is_wakeup_s3()) {
		const void *const gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
		uintptr_t aslb;

		if (gnvs && (aslb = gma_get_gnvs_aslb(gnvs)))
			intel_gma_opregion_register(aslb);
		else
			printk(BIOS_ERR, "Error: GNVS or ASLB not set.\n");
	}
}

static enum cb_err vbt_validate(struct region_device *rdev)
{
	uint32_t sig;

	if (rdev_readat(rdev, &sig, 0, sizeof(sig)) != sizeof(sig))
		return CB_ERR;

	if (sig != VBT_SIGNATURE)
		return CB_ERR;

	return CB_SUCCESS;
}

static enum cb_err locate_vbt_vbios(const u8 *vbios, struct region_device *rdev)
{
	const optionrom_header_t *oprom;
	const optionrom_pcir_t *pcir;
	struct region_device rd;
	enum cb_err ret;
	u8 opromsize;
	size_t offset;

	// FIXME: caller should supply a region_device instead of vbios pointer
	if (rdev_chain(&rd, &addrspace_32bit.rdev, (uintptr_t)vbios,
	    sizeof(*oprom)))
		return CB_ERR;

	if (rdev_readat(&rd, &opromsize, offsetof(optionrom_header_t, size),
	    sizeof(opromsize)) != sizeof(opromsize) || !opromsize)
		return CB_ERR;

	if (rdev_chain(&rd, &addrspace_32bit.rdev, (uintptr_t)vbios,
	    opromsize * 512))
		return CB_ERR;

	oprom = rdev_mmap(&rd, 0, sizeof(*oprom));
	if (!oprom)
		return CB_ERR;

	if (!oprom->pcir_offset || !oprom->vbt_offset) {
		rdev_munmap(&rd, (void *)oprom);
		return CB_ERR;
	}

	pcir = rdev_mmap(&rd, oprom->pcir_offset, sizeof(*pcir));
	if (pcir == NULL) {
		rdev_munmap(&rd, (void *)oprom);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "GMA: locate_vbt_vbios: %x %x %x %x %x\n",
		oprom->signature, pcir->vendor, pcir->classcode[0],
		pcir->classcode[1], pcir->classcode[2]);

	/* Make sure we got an Intel VGA option rom */
	if ((oprom->signature != OPROM_SIGNATURE) ||
	    (pcir->vendor != PCI_VENDOR_ID_INTEL) ||
	    (pcir->signature != 0x52494350) ||
	    (pcir->classcode[0] != 0x00) ||
	    (pcir->classcode[1] != 0x00) ||
	    (pcir->classcode[2] != 0x03)) {
		rdev_munmap(&rd, (void *)oprom);
		rdev_munmap(&rd, (void *)pcir);
		return CB_ERR;
	}

	rdev_munmap(&rd, (void *)pcir);

	/* Search for $VBT as some VBIOS are broken... */
	offset = oprom->vbt_offset;
	do {
		ret = rdev_chain(rdev, &rd, offset,
				 (opromsize * 512) - offset);
		offset++;
	} while (ret == CB_SUCCESS && vbt_validate(rdev) != CB_SUCCESS);

	offset--;

	if (ret == CB_SUCCESS && offset != oprom->vbt_offset)
		printk(BIOS_WARNING, "GMA: Buggy VBIOS found\n");
	else if (ret != CB_SUCCESS)
		printk(BIOS_ERR, "GMA: Broken VBIOS found\n");

	rdev_munmap(&rd, (void *)oprom);
	return ret;
}

static enum cb_err locate_vbt_cbfs(struct region_device *rdev)
{
	size_t vbt_data_size;
	void *vbt = locate_vbt(&vbt_data_size);

	if (vbt == NULL)
		return CB_ERR;

	if (rdev_chain(rdev, &addrspace_32bit.rdev, (uintptr_t)vbt,
	    vbt_data_size))
		return CB_ERR;

	printk(BIOS_INFO, "GMA: Found VBT in CBFS\n");

	return CB_SUCCESS;
}

static enum cb_err locate_vbt_vbios_cbfs(struct region_device *rdev)
{
	const u8 *oprom =
		(const u8 *)pci_rom_probe(dev_find_slot(0, PCI_DEVFN(0x2, 0)));
	if (oprom == NULL)
		return CB_ERR;

	printk(BIOS_INFO, "GMA: Found VBIOS in CBFS\n");

	return locate_vbt_vbios(oprom, rdev);
}

/* Initialize IGD OpRegion, called from ACPI code and OS drivers */
enum cb_err
intel_gma_init_igd_opregion(igd_opregion_t *opregion)
{
	struct region_device rdev;
	optionrom_vbt_t *vbt = NULL;
	optionrom_vbt_t *ext_vbt;
	bool found = false;

	/* Search for vbt.bin in CBFS. */
	if (locate_vbt_cbfs(&rdev) == CB_SUCCESS &&
	    vbt_validate(&rdev) == CB_SUCCESS) {
		found = true;
		printk(BIOS_INFO, "GMA: Found valid VBT in CBFS\n");
	}
	/* Search for pci8086,XXXX.rom in CBFS. */
	else if (locate_vbt_vbios_cbfs(&rdev) == CB_SUCCESS &&
		 vbt_validate(&rdev) == CB_SUCCESS) {
		found = true;
		printk(BIOS_INFO, "GMA: Found valid VBT in VBIOS\n");
	}
	/*
	 * Try to locate Intel VBIOS at 0xc0000. It might have been placed by
	 * Native Graphics Init as fake Option ROM or when coreboot did run the
	 * VBIOS on legacy platforms.
	 * TODO: Place generated fake VBT in CBMEM and get rid of this.
	 */
	else if (locate_vbt_vbios((u8 *)0xc0000, &rdev) == CB_SUCCESS &&
		 vbt_validate(&rdev) == CB_SUCCESS) {
		found = true;
		printk(BIOS_INFO, "GMA: Found valid VBT in legacy area\n");
	}

	if (!found) {
		printk(BIOS_ERR, "GMA: VBT couldn't be found\n");
		return CB_ERR;
	}

	vbt = rdev_mmap_full(&rdev);
	if (!vbt) {
		printk(BIOS_ERR, "GMA: Error mapping VBT\n");
		return CB_ERR;
	}

	if (vbt->hdr_vbt_size > region_device_sz(&rdev)) {
		printk(BIOS_ERR, "GMA: Error mapped only a partial VBT\n");
		rdev_munmap(&rdev, vbt);
		return CB_ERR;
	}

	memset(opregion, 0, sizeof(igd_opregion_t));

	memcpy(&opregion->header.signature, IGD_OPREGION_SIGNATURE,
		sizeof(opregion->header.signature));
	memcpy(opregion->header.vbios_version, vbt->coreblock_biosbuild,
					ARRAY_SIZE(vbt->coreblock_biosbuild));
	/* Extended VBT support */
	if (vbt->hdr_vbt_size > sizeof(opregion->vbt.gvd1)) {
		ext_vbt = cbmem_add(CBMEM_ID_EXT_VBT, vbt->hdr_vbt_size);

		if (ext_vbt == NULL) {
			printk(BIOS_ERR,
			       "GMA: Unable to add Ext VBT to cbmem!\n");
			rdev_munmap(&rdev, vbt);
			return CB_ERR;
		}

		memcpy(ext_vbt, vbt, vbt->hdr_vbt_size);
		opregion->mailbox3.rvda = (uintptr_t)ext_vbt;
		opregion->mailbox3.rvds = vbt->hdr_vbt_size;
	} else {
		/* Raw VBT size which can fit in gvd1 */
		memcpy(opregion->vbt.gvd1, vbt, vbt->hdr_vbt_size);
	}

	rdev_munmap(&rdev, vbt);

	/* 8kb */
	opregion->header.size = sizeof(igd_opregion_t) / 1024;

	/*
	 * Left-shift version field to accomodate Intel Windows driver quirk
	 * when not using a VBIOS.
	 * Required for Legacy boot + NGI, UEFI + NGI, and UEFI + GOP driver.
	 *
	 * Tested on: (platform, GPU, windows driver version)
	 * samsung/stumpy (SNB, GT2, 9.17.10.4459)
	 * google/link (IVB, GT2, 15.33.4653)
	 * google/wolf (HSW, GT1, 15.40.36.4703)
	 * google/panther (HSW, GT2, 15.40.36.4703)
	 * google/rikku (BDW, GT1, 15.40.36.4703)
	 * google/lulu (BDW, GT2, 15.40.36.4703)
	 * google/chell (SKL-Y, GT2, 15.45.21.4821)
	 * google/sentry (SKL-U, GT1, 15.45.21.4821)
	 * purism/librem13v2 (SKL-U, GT2, 15.45.21.4821)
	 *
	 * No adverse effects when using VBIOS or booting Linux.
	 */
	opregion->header.version = IGD_OPREGION_VERSION << 24;

	// FIXME We just assume we're mobile for now
	opregion->header.mailboxes = MAILBOXES_MOBILE;

	// TODO Initialize Mailbox 1
	opregion->mailbox1.clid = 1;

	// TODO Initialize Mailbox 3
	opregion->mailbox3.bclp = IGD_BACKLIGHT_BRIGHTNESS;
	opregion->mailbox3.pfit = IGD_FIELD_VALID | IGD_PFIT_STRETCH;
	opregion->mailbox3.pcft = 0; // should be (IMON << 1) & 0x3e
	opregion->mailbox3.cblv = IGD_FIELD_VALID | IGD_INITIAL_BRIGHTNESS;
	opregion->mailbox3.bclm[0] = IGD_WORD_FIELD_VALID + 0x0000;
	opregion->mailbox3.bclm[1] = IGD_WORD_FIELD_VALID + 0x0a19;
	opregion->mailbox3.bclm[2] = IGD_WORD_FIELD_VALID + 0x1433;
	opregion->mailbox3.bclm[3] = IGD_WORD_FIELD_VALID + 0x1e4c;
	opregion->mailbox3.bclm[4] = IGD_WORD_FIELD_VALID + 0x2866;
	opregion->mailbox3.bclm[5] = IGD_WORD_FIELD_VALID + 0x327f;
	opregion->mailbox3.bclm[6] = IGD_WORD_FIELD_VALID + 0x3c99;
	opregion->mailbox3.bclm[7] = IGD_WORD_FIELD_VALID + 0x46b2;
	opregion->mailbox3.bclm[8] = IGD_WORD_FIELD_VALID + 0x50cc;
	opregion->mailbox3.bclm[9] = IGD_WORD_FIELD_VALID + 0x5ae5;
	opregion->mailbox3.bclm[10] = IGD_WORD_FIELD_VALID + 0x64ff;

	/* Write ASLS PCI register and prepare SWSCI register. */
	intel_gma_opregion_register((uintptr_t)opregion);

	return CB_SUCCESS;
}
