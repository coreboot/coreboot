/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2017 Google, Inc.
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

#include <arch/io.h>
#include <reset.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <device/pci_ops.h>
#include <soc/southbridge.h>

void set_warm_reset_flag(void)
{
	u32 htic;
	htic = pci_read_config32(SOC_HT_DEV, HT_INIT_CONTROL);
	htic |= HTIC_COLD_RST_DET;
	pci_write_config32(SOC_HT_DEV, HT_INIT_CONTROL, htic);
}

int is_warm_reset(void)
{
	u32 htic;
	htic = pci_read_config32(SOC_HT_DEV, HT_INIT_CONTROL);
	return !!(htic & HTIC_COLD_RST_DET);
}

/* Clear bits 5, 9 & 10, used to signal the reset type */
static void clear_bios_reset(void)
{
	u32 htic;
	htic = pci_read_config32(SOC_HT_DEV, HT_INIT_CONTROL);
	htic &= ~HTIC_BIOSR_DETECT;
	pci_write_config32(SOC_HT_DEV, HT_INIT_CONTROL, htic);
}

void do_hard_reset(void)
{
	clear_bios_reset();

	/* De-assert and then assert all PwrGood signals on CF9 reset. */
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) |
		TOGGLE_ALL_PWR_GOOD);
	outb(RST_CMD | SYS_RST, SYS_RESET);
}

void do_soft_reset(void)
{
	set_warm_reset_flag();
	clear_bios_reset();

	/* Assert reset signals only. */
	outb(RST_CMD | SYS_RST, SYS_RESET);
}
