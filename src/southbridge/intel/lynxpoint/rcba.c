/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_def.h>
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
