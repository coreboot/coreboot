/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <arch/io.h>
#include <bootstate.h>
#include <console/console.h>
#include <reg_script.h>
#include <soc/iosf.h>

static const struct reg_script dptf_init_settings[] = {
	/* SocThermInit */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PTMC, 0x00030708),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_GFXT, 0x0000C000),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_VEDT, 0x00000004),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_ISPT, 0x00000004),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_PTPS, 0x00000000),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_TE_AUX3, 0x00061029),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_TTE_VRIccMax, 0x00061029),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_TTE_VRHot, 0x00061029),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_TTE_XXPROCHOT, 0x00061029),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_TTE_SLM0, 0x00001029),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_TTE_SLM1, 0x00001029),
	/* ratio 11 = 1466mhz for mid and entry celeron */
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_SOC_POWER_BUDGET, 0x00000B00),
	REG_IOSF_WRITE(IOSF_PORT_PMC, PUNIT_SOC_ENERGY_CREDIT, 0x00000002),
	REG_SCRIPT_END,
};

static void dptf_init(void *unused)
{
	printk(BIOS_DEBUG, "Applying SOC Thermal settings for DPTF.\n");
	reg_script_run(dptf_init_settings);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, dptf_init, NULL);
