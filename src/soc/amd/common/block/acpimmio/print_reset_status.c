/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <amdblocks/acpimmio.h>

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

void fch_print_pmxc0_status(void)
{
	/* PMxC0 S5/Reset Status shows the source of previous reset. */
	uint32_t pmxc0_status = pm_read32(PM_RST_STATUS);

	static const char *const pmxc0_status_bits[32] = {
		[0] = "ThermalTrip",
		[1] = "FourSecondPwrBtn",
		[2] = "Shutdown",
		[3] = "ThermalTripFromTemp",
		[4] = "RemotePowerDownFromASF",
		[5] = "ShutDownFan0",
		[16] = "UserRst",
		[17] = "SoftPciRst",
		[18] = "DoInit",
		[19] = "DoReset",
		[20] = "DoFullReset",
		[21] = "SleepReset",
		[22] = "KbReset",
		[23] = "LtReset",
		[24] = "FailBootRst",
		[25] = "WatchdogIssueReset",
		[26] = "RemoteResetFromASF",
		[27] = "SyncFlood",
		[28] = "HangReset",
		[29] = "EcWatchdogRst",
	};

	printk(BIOS_DEBUG, "PMxC0 STATUS: 0x%x ", pmxc0_status);
	print_num_status_bits(ARRAY_SIZE(pmxc0_status_bits), pmxc0_status, pmxc0_status_bits);
	printk(BIOS_DEBUG, "\n");
}
