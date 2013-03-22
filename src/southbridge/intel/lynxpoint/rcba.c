/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <console/console.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include "pch.h"

void pch_config_rcba(const struct rcba_config_instruction *rcba_config)
{
	const struct rcba_config_instruction *rc;
	u32 value;

	rc = rcba_config;
	while (rc->command != RCBA_COMMAND_END)
	{
		if ((rc->command & RCBA_REG_SIZE_MASK) == RCBA_REG_SIZE_16) {
			switch (rc->command & RCBA_COMMAND_MASK) {
			case RCBA_COMMAND_SET:
				RCBA16(rc->reg) = (u16)rc->or_value;
				break;
			case RCBA_COMMAND_READ:
				(void)RCBA16(rc->reg);
				break;
			case RCBA_COMMAND_RMW:
				value = RCBA16(rc->reg);
				value &= rc->mask;
				value |= rc->or_value;
				RCBA16(rc->reg) = (u16)value;
				break;
			}
		} else {
			switch (rc->command & RCBA_COMMAND_MASK) {
			case RCBA_COMMAND_SET:
				RCBA32(rc->reg) = rc->or_value;
				break;
			case RCBA_COMMAND_READ:
				(void)RCBA32(rc->reg);
				break;
			case RCBA_COMMAND_RMW:
				value = RCBA32(rc->reg);
				value &= rc->mask;
				value |= rc->or_value;
				RCBA32(rc->reg) = value;
				break;
			}
		}
		rc++;
	}
}

