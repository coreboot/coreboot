/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>

/* Include EC functions */
#include <ec/quanta/ene_kb3940q/ec.h>
#include "ec.h"

void mainboard_smi_sleep(u8 slp_typ)
{
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
