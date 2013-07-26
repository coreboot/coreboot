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

/* Include romstage serial for SIO helper functions */
#include <superio/ite/it8772f/early_serial.c>

/* Include EC functions */
#include <ec/quanta/it8518/ec.h>
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
	u8 cmd = ec_it8518_get_event();
	u32 pm1_cnt;

	switch (cmd) {
	case EC_SMI_LID_CLOSED:
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
		/* Process all pending events */
		while (mainboard_smi_ec() != 0);
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/*
	 * Tell the EC to Enable USB power for S3 if requested.
	 * Bit0 of 0x0D/Bit0 of 0x26
	 * 0/0 All USB port off
	 * 1/0 USB on, all USB port didn’t support wake up
	 * 0/1 USB on, yellow port support wake up charge, but may not support
	 *             charge smart phone.
	 * 1/1 USB on, yellow port in AUTO mode and didn’t support wake up system.
	 */
	if (smm_get_gnvs()->s3u0 != 0 || smm_get_gnvs()->s3u1 != 0) {
		ec_write(EC_PERIPH_CNTL_3, ec_read(EC_PERIPH_CNTL_3) | 0x00);
		ec_write(EC_USB_S3_EN, ec_read(EC_USB_S3_EN) | 0x01);
		printk(BIOS_DEBUG, "USB wake from S3 enabled.\n");
	} else {
		/*
		 * If USB charging in suspend is disabled then also disable
		 * the XHCI PME to prevent wake when the port power is cut
		 * after the transition into suspend.
		 */
		if (smm_get_gnvs()->xhci) {
			u32 reg32 = pci_read_config32(PCH_XHCI_DEV, 0x74);
			reg32 &= ~(1 << 8); /* disable PME */
			reg32 |= (1 << 15); /* clear PME status */
			pci_write_config32(PCH_XHCI_DEV, 0x74, reg32);
		}
	}

	ec_kbc_write_cmd(EC_KBD_CMD_MUTE);
	ec_it8518_enable_wake_events();
}

#define APMC_FINALIZE 0xcb
#define APMC_ACPI_EN  0xe1
#define APMC_ACPI_DIS 0x1e

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APMC_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}
	printk(BIOS_DEBUG, "SMI#: finalize\n");
		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();
		stout_ec_finalize_smm();

		mainboard_finalized = 1;
		break;
	case APMC_ACPI_EN:
		/*
		 * TODO(kimarie) Clear all pending events and enable SCI.
		 */
		ec_write_cmd(EC_CMD_NOTIFY_ACPI_ENTER);
		break;
	case APMC_ACPI_DIS:
		/*
		 * TODO(kimarie) Clear all pending events and enable SMI.
		 */
		ec_write_cmd(EC_CMD_NOTIFY_ACPI_EXIT);
		break;
	}
	return 0;
}
