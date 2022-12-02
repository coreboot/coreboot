/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/pmbase.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <ec/smsc/mec1308/ec.h>
#include "ec.h"

static u8 mainboard_smi_ec(void)
{
	u8 cmd;

	cmd = read_ec_command_byte(EC_GET_SMI_CAUSE);

	switch (cmd) {
	case EC_LID_CLOSE:
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
	}
	return 0;
}
