/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <elog.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>
#include <vboot_struct.h>

#include "chromeos.h"
#include "gnvs.h"

static chromeos_acpi_t *chromeos_acpi;
static u32 me_hash_saved[8];

void chromeos_init_chromeos_acpi(chromeos_acpi_t *init)
{
	chromeos_acpi = init;

	/* Copy saved ME hash into NVS */
	memcpy(chromeos_acpi->mehh, me_hash_saved, sizeof(chromeos_acpi->mehh));

	struct vboot_handoff *vboot_handoff;

	if (vboot_get_handoff_info((void **)&vboot_handoff, NULL) == 0)
		memcpy(&chromeos_acpi->vdat[0], &vboot_handoff->shared_data[0],
		       ARRAY_SIZE(chromeos_acpi->vdat));

	chromeos_ram_oops_init(chromeos_acpi);
}

void chromeos_set_me_hash(u32 *hash, int len)
{
	if ((len*sizeof(u32)) > sizeof(chromeos_acpi->mehh))
		return;

	/* Copy to NVS or save until it is ready */
	if (chromeos_acpi)
		/* This does never happen! */
		memcpy(chromeos_acpi->mehh, hash, len*sizeof(u32));
	else
		memcpy(me_hash_saved, hash, len*sizeof(u32));
}

chromeos_acpi_t *chromeos_get_chromeos_acpi(void)
{
	return chromeos_acpi;
}
