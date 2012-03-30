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

#include "chromeos.h"
#include <arch/coreboot_tables.h>
#include <console/console.h>

int developer_mode_enabled(void)
{
	return get_developer_mode_switch();
}

int recovery_mode_enabled(void)
{
	/* TODO(reinauer): get information from VbInit.
	 * the recovery mode switch is not the only reason to go
	 * to recovery mode.
	 */
	return get_recovery_mode_switch() || get_recovery_mode_from_vbnv();
}

