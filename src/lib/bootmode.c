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

#include <rules.h>
#include <bootmode.h>
#if CONFIG_CHROMEOS || CONFIG_VBOOT_VERIFY_FIRMWARE
#include <vendorcode/google/chromeos/chromeos.h>
#endif

#if CONFIG_BOOTMODE_STRAPS
int developer_mode_enabled(void)
{
	if (get_developer_mode_switch())
		return 1;
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	if (vboot_enable_developer())
		return 1;
#endif
	return 0;
}

/*
 * This is called in multiple places and has to detect
 * recovery mode triggered from the EC and via shared
 * recovery reason set with crossystem.
 *
 * If shared recovery reason is set:
 * - before VbInit then get_recovery_mode_from_vbnv() is true
 * - after VbInit then vboot_enable_recovery() is true
 *
 * Otherwise the mainboard handler for get_recovery_mode_switch()
 * will detect recovery mode initiated by the EC.
 */
int recovery_mode_enabled(void)
{
	if (get_recovery_mode_switch())
		return 1;
#if CONFIG_CHROMEOS
	if (get_recovery_mode_from_vbnv())
		return 1;
#endif
#if CONFIG_VBOOT_VERIFY_FIRMWARE
	if (vboot_enable_recovery())
		return 1;
#endif
	return 0;
}
#endif /* CONFIG_BOOTMODE_STRAPS */

#if ENV_RAMSTAGE
static int gfx_init_done = -1;

int gfx_get_init_done(void)
{
	if (gfx_init_done < 0)
		return 0;
	return gfx_init_done;
}

void gfx_set_init_done(int done)
{
	gfx_init_done = done;
}
#endif
