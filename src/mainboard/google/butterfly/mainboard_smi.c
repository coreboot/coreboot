/*
 * This file is part of the coreboot project.
 *
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
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_206ax/model_206ax.h>

/* Include EC functions */
#include <ec/quanta/ene_kb3940q/ec.h>
#include "ec.h"

void mainboard_smi_gpi(u32 gpi_sts)
{
	printk(BIOS_DEBUG, "warn: unknown mainboard_smi_gpi: %x\n", gpi_sts);
}

void mainboard_smi_sleep(u8 slp_typ)
{
	printk(BIOS_DEBUG, "mainboard_smi_sleep: %x\n", slp_typ);

	/* Tell the EC to Enable USB power for S3 if requested */
	if (smm_get_gnvs()->s3u0 != 0 || smm_get_gnvs()->s3u1 != 0)
		ec_mem_write(EC_EC_PSW, ec_mem_read(EC_EC_PSW) | EC_PSW_USB);

	/* Disable wake on USB, LAN & RTC */
	/* Enable Wake from Keyboard */
	if ((slp_typ == 4) || (slp_typ == 5)) {
		printk(BIOS_DEBUG, "Disabling wake on RTC\n");
		ec_mem_write(EC_EC_PSW, EC_PSW_IKB);
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
		/* Clear all pending events and enable SCI */
		ec_write_cmd(EC_CMD_ENABLE_ACPI_MODE);
		break;

	case APMC_ACPI_DIS:
		printk(BIOS_DEBUG, "APMC: ACPI_DIS\n");
		/* Clear all pending events and tell the EC that ACPI is disabled */
		ec_write_cmd(EC_CMD_DISABLE_ACPI_MODE);
		break;
	}
	return 0;
}
