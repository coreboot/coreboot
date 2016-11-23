/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <string.h>
#include <cbmem.h>

#include "gma.h"
#include "opregion.h"
#include "vbt.h"

int init_igd_opregion(igd_opregion_t *opregion)
{
	struct region_device vbt_rdev;
	optionrom_vbt_t *vbt;
	optionrom_vbt_t *ext_vbt;

	if (locate_vbt(&vbt_rdev) == CB_ERR) {
		printk(BIOS_ERR, "VBT not found\n");
		return 0;
	};

	vbt = rdev_mmap_full(&vbt_rdev);

	if (!vbt) {
		printk(BIOS_ERR, "VBT couldn't be read\n");
		return 0;
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
			printk(BIOS_ERR, "Unable to add Ext VBT to cbmem!\n");
			return 0;
		}

		memcpy(ext_vbt, vbt, vbt->hdr_vbt_size);
		opregion->mailbox3.rvda = (uintptr_t)ext_vbt;
		opregion->mailbox3.rvds = vbt->hdr_vbt_size;
	} else {
		/* Raw VBT size which can fit in gvd1 */
		memcpy(opregion->vbt.gvd1, vbt, vbt->hdr_vbt_size);
	}

	/* 8KiB */
	opregion->header.size = sizeof(igd_opregion_t) / KiB;
	opregion->header.version = IGD_OPREGION_VERSION;

	/* FIXME We just assume we're mobile for now */
	opregion->header.mailboxes = MAILBOXES_MOBILE;

	rdev_munmap(&vbt_rdev, vbt);

	return 1;
}
