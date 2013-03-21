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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
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

int mainboard_io_trap_handler(int smif)
{
	printk(BIOS_DEBUG, "mainboard_io_trap_handler: %x\n", smif);
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		smm_get_gnvs()->smif = 0;
		break;
	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1;
}

void mainboard_smi_gpi(u16 gpi_sts)
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

#define APMC_FINALIZE 0xcb
#define APMC_ACPI_EN  0xe1
#define APMC_ACPI_DIS 0x1e

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	printk(BIOS_DEBUG, "mainboard_smi_apmc: %x\n", apmc);
	switch (apmc) {
	case APMC_FINALIZE:
		printk(BIOS_DEBUG, "APMC: FINALIZE\n");
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "APMC#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();

		mainboard_finalized = 1;
		break;

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
