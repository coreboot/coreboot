/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/pmutil.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <elog.h>
#include <ec/compal/ene932/ec.h>
#include "ec.h"

static u8 mainboard_smi_ec(void)
{
	u8 src;
	u32 pm1_cnt;
#if IS_ENABLED(CONFIG_ELOG_GSMI)
	static int battery_critical_logged;
#endif

	ec_kbc_write_cmd(0x56);
	src = ec_kbc_read_ob();
	printk(BIOS_DEBUG, "mainboard_smi_ec src: %x\n", src);

	switch (src) {
	case EC_BATTERY_CRITICAL:
#if IS_ENABLED(CONFIG_ELOG_GSMI)
		if (!battery_critical_logged)
			elog_add_event_byte(ELOG_TYPE_EC_EVENT,
					    EC_EVENT_BATTERY_CRITICAL);
		battery_critical_logged = 1;
#endif
		break;
	case EC_LID_CLOSE:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, EC_EVENT_LID_CLOSED);
#endif
		/* Go to S5 */
		pm1_cnt = inl(smm_get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, smm_get_pmbase() + PM1_CNT);
		break;
	}

	return src;
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	u32 pm1_cnt;

	printk(BIOS_DEBUG, "mainboard_smi_gpi: %x\n", gpi_sts);
	if (gpi_sts & (1 << EC_SMI_GPI)) {
		/* Process all pending events from EC */
		while (mainboard_smi_ec() != EC_NO_EVENT);
	}
	else if (gpi_sts & (1 << EC_LID_GPI)) {
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

#if IS_ENABLED(CONFIG_ELOG_GSMI)
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, EC_EVENT_LID_CLOSED);
#endif
		/* Go to S5 */
		pm1_cnt = inl(smm_get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, smm_get_pmbase() + PM1_CNT);
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "mainboard_smi_sleep: %x\n", slp_typ);
	/* Disable SCI and SMI events */


	/* Clear pending events that may trigger immediate wake */


	/* Enable wake events */


	/* Tell the EC to Disable USB power */
	if (smm_get_gnvs()->s3u0 == 0 && smm_get_gnvs()->s3u1 == 0) {
		ec_kbc_write_cmd(0x45);
		ec_kbc_write_ib(0xF2);
	}
}

#define APMC_ACPI_EN  0xe1
#define APMC_ACPI_DIS 0x1e

int mainboard_smi_apmc(u8 apmc)
{
	printk(BIOS_DEBUG, "mainboard_smi_apmc: %x\n", apmc);
	switch (apmc) {
	case APMC_ACPI_EN:
		printk(BIOS_DEBUG, "APMC: ACPI_EN\n");
		/* Clear all pending events */
		/* EC cmd:59 data:E8 */
		ec_kbc_write_cmd(0x59);
		ec_kbc_write_ib(0xE8);

		/* Set LID GPI to generate SCIs */
		gpi_route_interrupt(EC_LID_GPI, GPI_IS_SCI);
		break;
	case APMC_ACPI_DIS:
		printk(BIOS_DEBUG, "APMC: ACPI_DIS\n");
		/* Clear all pending events */
		/* EC cmd:59 data:e9 */
		ec_kbc_write_cmd(0x59);
		ec_kbc_write_ib(0xE9);

		/* Set LID GPI to generate SMIs */
		gpi_route_interrupt(EC_LID_GPI, GPI_IS_SMI);
		break;
	}
	return 0;
}
