/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/pmbase.h>
#include <northbridge/intel/sandybridge/sandybridge.h>

/* Include EC functions */
#include <ec/quanta/it8518/ec.h>
#include "ec.h"

static u8 mainboard_smi_ec(void)
{
	u8 cmd = ec_it8518_get_event();

	switch (cmd) {
	case EC_SMI_LID_CLOSED:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");
		/* Go to S5 */
		write_pmbase32(PM1_CNT, read_pmbase32(PM1_CNT) | (0xf << 10));
		break;
	}

	return cmd;
}

void mainboard_smi_gpi(u32 gpi_sts)
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
	 * 1/0 USB on, all USB port didn't support wake up
	 * 0/1 USB on, yellow port support wake up charge, but may not support
	 *             charge smart phone.
	 * 1/1 USB on, yellow port in AUTO mode and didn't support wake up system.
	 */
	if (gnvs->s3u0 != 0 || gnvs->s3u1 != 0) {
		ec_write(EC_PERIPH_CNTL_3, ec_read(EC_PERIPH_CNTL_3) | 0x00);
		ec_write(EC_USB_S3_EN, ec_read(EC_USB_S3_EN) | 0x01);
		printk(BIOS_DEBUG, "USB wake from S3 enabled.\n");
	} else {
		/*
		 * If USB charging in suspend is disabled then also disable
		 * the XHCI PME to prevent wake when the port power is cut
		 * after the transition into suspend.
		 */
		if (gnvs->xhci) {
			pci_update_config32(PCH_XHCI_DEV, 0x74, ~(1 << 8), 1 << 15);
		}
	}

	ec_kbc_write_cmd(EC_KBD_CMD_MUTE);
	ec_it8518_enable_wake_events();
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_FINALIZE:
		stout_ec_finalize_smm();
		break;
	case APM_CNT_ACPI_ENABLE:
		/*
		 * TODO(kimarie) Clear all pending events and enable SCI.
		 */
		ec_write_cmd(EC_CMD_NOTIFY_ACPI_ENTER);
		break;
	case APM_CNT_ACPI_DISABLE:
		/*
		 * TODO(kimarie) Clear all pending events and enable SMI.
		 */
		ec_write_cmd(EC_CMD_NOTIFY_ACPI_EXIT);
		break;
	}
	return 0;
}
