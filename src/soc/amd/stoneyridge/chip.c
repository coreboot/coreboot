/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <chip.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>

struct device_operations cpu_bus_ops = {
	.read_resources	  = DEVICE_NOOP,
	.set_resources	  = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init		  = stoney_init_cpus,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};

struct device_operations pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = domain_set_resources,
	.enable_resources = domain_enable_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
};

static void enable_dev(device_t dev)
{
	static int done = 0;

	if (!done) {
		setup_bsp_ramtop();
		done = 1;
	}

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_PCI)
		sb_enable(dev);
}

static void soc_init(void *chip_info)
{
	southbridge_init(chip_info);
}

static void soc_final(void *chip_info)
{
	southbridge_final(chip_info);
	fam15_finalize(chip_info);
}

struct chip_operations soc_amd_stoneyridge_ops = {
	CHIP_NAME("AMD StoneyRidge SOC")
	.enable_dev = &enable_dev,
	.init = &soc_init,
	.final = &soc_final
};

static void earliest_ramstage(void *unused)
{
	post_code(0x46);
	if (IS_ENABLED(CONFIG_SOC_AMD_PSP_SELECTABLE_SMU_FW))
		psp_load_named_blob(MBOX_BIOS_CMD_SMU_FW2, "smu_fw2");

	post_code(0x47);
	AGESAWRAPPER(amdinitenv);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, earliest_ramstage, NULL);
