/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
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
#include <fsp/util.h>
#include <reset.h>
#include <soc/me.h>
#include <soc/pm.h>
#include <timer.h>

static void do_force_global_reset(void)
{
	u32 reg32;
	/*PMC Controller Device 0x1F, Func 02*/
	uint8_t *pmc_regs;

	/*
	 * BIOS should ensure it does a global reset
	 * to reset both host and Intel ME by setting
	 * PCH PMC [B0:D31:F2 register offset 0x1048 bit 20]
	 */
	pmc_regs = pmc_mmio_regs();
	reg32 = read32(pmc_regs + ETR3);
	reg32 |= ETR3_CF9GR;
	write32(pmc_regs + ETR3, reg32);

	/* Now BIOS can write 0x06 or 0x0E to 0xCF9 port
	 * to global reset platform */
	hard_reset();
}

void global_reset(void)
{
	if (send_global_reset() != 0) {
		/* If ME unable to reset platform then
		 * force global reset using PMC CF9GR register*/
		do_force_global_reset();
	}
}

void chipset_handle_reset(uint32_t status)
{
	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_3: /* Global Reset */
		printk(BIOS_DEBUG, "GLOBAL RESET!!\n");
		global_reset();
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", status);
		die("unknown reset type");
		break;
	}
}
