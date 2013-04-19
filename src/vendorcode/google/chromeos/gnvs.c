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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <elog.h>

#include "chromeos.h"
#include "gnvs.h"
#if CONFIG_VBOOT_VERIFY_FIRMWARE
#include "vboot_handoff.h"
#endif

chromeos_acpi_t *vboot_data = NULL;
static u32 me_hash_saved[8];

void chromeos_init_vboot(chromeos_acpi_t *chromeos)
{
	vboot_data = chromeos;

	/* Copy saved ME hash into NVS */
	memcpy(vboot_data->mehh, me_hash_saved, sizeof(vboot_data->mehh));

#if CONFIG_VBOOT_VERIFY_FIRMWARE
	/* Save the vdat from the vboot handoff structure. Downstream software
	 * consumes the data located in the ACPI table. Ensure it reflects
	 * the shared data from VbInit() and VbSelectFirmware(). */
	struct vboot_handoff *vboot_handoff;

	vboot_handoff = cbmem_find(CBMEM_ID_VBOOT_HANDOFF);

	if (vboot_handoff != NULL)
		memcpy(&chromeos->vdat[0], &vboot_handoff->shared_data[0],
		       ARRAY_SIZE(chromeos->vdat));
#endif

#if CONFIG_ELOG
	if (developer_mode_enabled() ||
	    (vboot_wants_oprom() && !recovery_mode_enabled()))
		elog_add_event(ELOG_TYPE_CROS_DEVELOPER_MODE);
	if (recovery_mode_enabled()) {
		int reason = get_recovery_mode_from_vbnv();
		elog_add_event_byte(ELOG_TYPE_CROS_RECOVERY_MODE,
			    reason ? reason : ELOG_CROS_RECOVERY_MODE_BUTTON);
	}
#endif
}

void chromeos_set_me_hash(u32 *hash, int len)
{
	if ((len*sizeof(u32)) > sizeof(vboot_data->mehh))
		return;

	/* Copy to NVS or save until it is ready */
	if (vboot_data)
		/* This does never happen! */
		memcpy(vboot_data->mehh, hash, len*sizeof(u32));
	else
		memcpy(me_hash_saved, hash, len*sizeof(u32));
}

void acpi_get_vdat_info(uint64_t *vdat_addr, uint32_t *vdat_size)
{
	*vdat_addr = (intptr_t)vboot_data;
	*vdat_size = sizeof(*vboot_data);
}
