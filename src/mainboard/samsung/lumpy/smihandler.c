/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <halt.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
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
		poweroff();
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
	case APM_CNT_ACPI_ENABLE:
		send_ec_command(EC_SMI_DISABLE);
		send_ec_command(EC_ACPI_ENABLE);
		break;

	case APM_CNT_ACPI_DISABLE:
		send_ec_command(EC_SMI_ENABLE);
		send_ec_command(EC_ACPI_DISABLE);
		break;
	}
	return 0;
}
