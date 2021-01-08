/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
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
	if (gnvs->s3u0 != 0 || gnvs->s3u1 != 0)
		ec_mem_write(EC_EC_PSW, ec_mem_read(EC_EC_PSW) | EC_PSW_USB);

	/* Disable wake on USB, LAN & RTC */
	/* Enable Wake from Keyboard */
	if ((slp_typ == 4) || (slp_typ == 5)) {
		printk(BIOS_DEBUG, "Disabling wake on RTC\n");
		ec_mem_write(EC_EC_PSW, EC_PSW_IKB);
	}
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		/* Clear all pending events and enable SCI */
		ec_write_cmd(EC_CMD_ENABLE_ACPI_MODE);
		break;

	case APM_CNT_ACPI_DISABLE:
		/* Clear all pending events and tell the EC that ACPI is disabled */
		ec_write_cmd(EC_CMD_DISABLE_ACPI_MODE);
		break;
	}
	return 0;
}
