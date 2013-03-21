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
#include <ec/smsc/mec1308/ec.h>
#include "ec.h"

int mainboard_io_trap_handler(int smif)
{
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

static u8 mainboard_smi_ec(void)
{
	u8 cmd;
	u32 pm1_cnt;

	cmd = read_ec_command_byte(EC_GET_SMI_CAUSE);

	switch (cmd) {
	case EC_LID_CLOSE:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		pm1_cnt = inl(smm_get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, smm_get_pmbase() + PM1_CNT);
		break;
	}

	return cmd;
}

void mainboard_smi_gpi(u16 gpi_sts)
{
	if (gpi_sts & (1 << EC_SMI_GPI)) {
		/* Process all pending EC requests */
		ec_set_ports(EC_MAILBOX_PORT, EC_MAILBOX_PORT+1);
		while (mainboard_smi_ec() != 0xff);

		/* The EC may keep asserting SMI# for some
		 * period unless we kick it here.
		 */
		send_ec_command(EC_SMI_DISABLE);
		send_ec_command(EC_SMI_ENABLE);
	}
}

#define APMC_FINALIZE 0xcb

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	ec_set_ports(EC_MAILBOX_PORT, EC_MAILBOX_PORT+1);

	switch (apmc) {
	case 0xe1: /* ACPI ENABLE */
		send_ec_command(EC_SMI_DISABLE);
		send_ec_command(EC_ACPI_ENABLE);
		break;

	case 0x1e: /* ACPI DISABLE */
		send_ec_command(EC_SMI_ENABLE);
		send_ec_command(EC_ACPI_DISABLE);
		break;

	case APMC_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();

		mainboard_finalized = 1;
		break;
	}
	return 0;
}
