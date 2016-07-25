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

#include "gma.h"
#include "opregion.h"
#include "vbt.h"

int init_igd_opregion(igd_opregion_t *opregion)
{
	struct region_device vbt_rdev;
	optionrom_vbt_t *vbt;

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
	memcpy(opregion->vbt.gvd1, vbt, MIN(vbt->hdr_vbt_size,
					sizeof(opregion->vbt.gvd1)));

	/* 8KiB */
	opregion->header.size = sizeof(igd_opregion_t) / KiB;
	opregion->header.version = IGD_OPREGION_VERSION;

	/* FIXME We just assume we're mobile for now */
	opregion->header.mailboxes = MAILBOXES_MOBILE;

	rdev_munmap(&vbt_rdev, vbt);

	return 1;
}
