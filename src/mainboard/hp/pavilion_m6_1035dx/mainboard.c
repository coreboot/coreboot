/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include "ec.h"

#include <arch/acpi.h>
#include <console/console.h>
#include <device/device.h>

#include <southbridge/amd/agesa/hudson/smi.h>

static void pavilion_cold_boot_init(void)
{
	/* Lid SMI is only used in non-ACPI mode; leave it off in S3 resume */
	hudson_configure_gevent_smi(EC_LID_GEVENT, SMI_MODE_SMI, SMI_LVL_LOW);
	/* EC is not powered off during S3 sleep */
	pavilion_m6_1035dx_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	hudson_configure_gevent_smi(EC_SMI_GEVENT, SMI_MODE_SMI, SMI_LVL_HIGH);
	hudson_enable_smi_generation();

	if (!acpi_is_wakeup_s3())
		pavilion_cold_boot_init();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
