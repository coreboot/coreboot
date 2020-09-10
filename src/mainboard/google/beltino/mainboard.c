/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "onboard.h"

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	apm_control(APM_CNT_FINALIZE);
}

static void mainboard_init(struct device *dev)
{
	lan_init();
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
