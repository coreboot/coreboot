/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <halt.h>
#include <reset.h>
#include <fsp_util.h>
#include <northbridge/intel/fsp_sandybridge/northbridge.h>
#include <northbridge/intel/fsp_sandybridge/raminit.h>
#include <southbridge/intel/fsp_i89xx/pch.h>
#include <southbridge/intel/fsp_i89xx/gpio.h>
#include <southbridge/intel/fsp_i89xx/me.h>
#include <southbridge/intel/fsp_i89xx/romstage.h>
#include <superio/winbond/wpcd376i/wpcd376i.h>
#include <superio/intel/i8900/i8900.h>
#include "gpio.h"

#define SIO_PORT    0x2e
#define SERIAL_DEV PNP_DEV(SIO_PORT, 0x03)
#define WDT_DEV PNP_DEV(SIO_PORT, 0x06)
#define WDT_BASE_ADDR 0x600

/*
 * Get function disables - any changes here should match in devicetree
 * @param fd_mask
 * @param fd2_mask
 */
void get_func_disables(uint32_t *fd_mask, uint32_t *fd2_mask)
{
	*fd_mask |= PCH_DISABLE_ALWAYS;
}

/**
 * Get LPC setting - enables various devices (KB, mouse, etc.)
 */
uint16_t get_lpc_setting(void)
{
	/* Enable SuperIO (2E/4E) + COM1 & Keyboard controller */
	return CNF1_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN | KBC_LPC_EN;
}

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry(void)
{
	/* Early SuperIO setup - Using SIO Serial Port*/
	wpcd376i_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	i8900_enable_wdt(WDT_DEV, WDT_BASE_ADDR);
}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry(void)
{

}

void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	/* No overrides needed */
	return;
}
