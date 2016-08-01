/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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
#define __SIMPLE_DEVICE__

#include <arch/early_variables.h>
#include <console/console.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/reg_access.h>

static const struct reg_script clear_smi_and_wake_events_script[] = {
	/* Clear any SMI or wake events */
	REG_GPE0_READ(R_QNC_GPE0BLK_GPE0S),
	REG_GPE0_READ(R_QNC_GPE0BLK_SMIS),
	REG_GPE0_OR(R_QNC_GPE0BLK_GPE0S, B_QNC_GPE0BLK_GPE0S_ALL),
	REG_GPE0_OR(R_QNC_GPE0BLK_SMIS, B_QNC_GPE0BLK_SMIS_ALL),
	REG_SCRIPT_END
};

void clear_smi_and_wake_events(void)
{
	struct chipset_power_state *ps;

	/* Clear SMI and wake events */
	ps = get_power_state();
	if (ps->prev_sleep_state != 3) {
		printk(BIOS_SPEW, "Clearing SMI interrupts and wake events\n");
		reg_script_run_on_dev(LPC_BDF,
			clear_smi_and_wake_events_script);
	}
}

void disable_rom_shadow(void)
{
	uint32_t data;

	/* Determine if the shadow ROM is enabled */
	data = port_reg_read(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
				QNC_MSG_FSBIC_REG_HMISC);
	if ((data & (ESEG_RD_DRAM | FSEG_RD_DRAM))
		!= (ESEG_RD_DRAM | FSEG_RD_DRAM)) {

		/* Disable the ROM shadow 0x000e0000 - 0x000fffff */
		data |= ESEG_RD_DRAM | FSEG_RD_DRAM;
		port_reg_write(QUARK_NC_HOST_BRIDGE_SB_PORT_ID,
			QNC_MSG_FSBIC_REG_HMISC, data);
	}
}
