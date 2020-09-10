/* SPDX-License-Identifier: GPL-2.0-only */

#include "ec.h"

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/device.h>

#include <southbridge/amd/agesa/hudson/smi.h>

static void pavilion_cold_boot_init(void)
{
	/* Lid SMI is only used in non-ACPI mode; leave it off in S3 resume */
	hudson_configure_gevent_smi(EC_LID_GEVENT, SMI_MODE_SMI, SMI_LVL_LOW);
	/* EC is not powered off during S3 sleep */
	lenovo_g505s_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	hudson_configure_gevent_smi(EC_SMI_GEVENT, SMI_MODE_SMI, SMI_LVL_HIGH);
	global_smi_enable();

	if (!acpi_is_wakeup_s3())
		pavilion_cold_boot_init();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
