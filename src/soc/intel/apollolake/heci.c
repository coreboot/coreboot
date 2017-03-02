/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/heci.h>
#include <soc/pci_devs.h>

uint32_t heci_fw_sts(void)
{
	return pci_read_config32(HECI1_DEV, REG_SEC_FW_STS0);
}

bool heci_cse_normal(void)
{
	return ((heci_fw_sts() & MASK_SEC_STATUS) == SEC_STATE_NORMAL);
}

bool heci_cse_done(void)
{
	return (!!(heci_fw_sts() & MASK_SEC_FIRMWARE_COMPLETE));
}
