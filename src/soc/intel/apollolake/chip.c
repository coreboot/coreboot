/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <bootstate.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <memrange.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>

#include "chip.h"

static void pci_domain_set_resources(device_t dev)
{
       assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = NULL,
	.init = NULL,
	.scan_bus = pci_domain_scan_bus,
	.ops_pci_bus = pci_bus_default_ops,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = apollolake_init_cpus,
	.scan_bus = NULL,
};

static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void soc_init(void *data)
{
	struct range_entry range;

	/* TODO: tigten this resource range */
	/* TODO: fix for S3 resume, as this would corrupt OS memory */
	range_entry_init(&range, 0x200000, 4ULL*GiB, 0);
	fsp_silicon_init(&range);
}

void platform_fsp_silicon_init_params_cb(struct FSPS_UPD *silupd)
{
        struct FSP_S_CONFIG *silconfig = &silupd->FspsConfig;
	static struct soc_intel_apollolake_config *cfg;

	/* Load VBT before devicetree-specific config. */
	silconfig->GraphicsConfigPtr = fsp_load_vbt();

	struct device *dev = NB_DEV_ROOT;
	if (!dev && !dev->chip_info) {
		printk(BIOS_ERR, "BUG! Could not find SOC devicetree config\n");
		return;
	}

	cfg = dev->chip_info;

	silconfig->PcieRpClkReqNumber[0] = cfg->pcie_rp0_clkreq_pin;
	silconfig->PcieRpClkReqNumber[1] = cfg->pcie_rp1_clkreq_pin;
	silconfig->PcieRpClkReqNumber[2] = cfg->pcie_rp2_clkreq_pin;
	silconfig->PcieRpClkReqNumber[3] = cfg->pcie_rp3_clkreq_pin;
	silconfig->PcieRpClkReqNumber[4] = cfg->pcie_rp4_clkreq_pin;
	silconfig->PcieRpClkReqNumber[5] = cfg->pcie_rp5_clkreq_pin;
}

struct chip_operations soc_intel_apollolake_ops = {
	CHIP_NAME("Intel Apollolake SOC")
	.enable_dev = &enable_dev,
	.init = &soc_init
};

static void fsp_notify_dummy(void *arg)
{

	enum fsp_notify_phase ph = (enum fsp_notify_phase) arg;

	if (fsp_notify(ph) != FSP_SUCCESS)
		printk(BIOS_CRIT, "FspNotify failed!\n");
}

BOOT_STATE_INIT_ENTRY(BS_DEV_RESOURCES, BS_ON_EXIT, fsp_notify_dummy,
						(void *) AFTER_PCI_ENUM);
BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, fsp_notify_dummy,
						(void *) READY_TO_BOOT);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, fsp_notify_dummy,
						(void *) READY_TO_BOOT);
