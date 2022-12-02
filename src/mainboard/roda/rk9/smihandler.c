/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/i82801ix/i82801ix.h>
#include <ec/acpi/ec.h>

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << 1)) {
		printk(BIOS_DEBUG, "EC/SMI\n");
		/* TODO */
	}
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		send_ec_command(0x05); /* Set_SMI_Disable */
		send_ec_command(0xaa); /* Set_ACPI_Enable */
		break;

	case APM_CNT_ACPI_DISABLE:
		send_ec_command(0x04); /* Set_SMI_Enable */
		send_ec_command(0xab); /* Set_ACPI_Disable */
		break;
	}
	return 0;
}
