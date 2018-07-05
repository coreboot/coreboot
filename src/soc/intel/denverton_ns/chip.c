/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Intel Corp.
 * Copyright (C) 2017 Online SAS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <bootstate.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/fast_spi.h>
#include <soc/iomap.h>
#include <soc/intel/common/vbt.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/fiamux.h>
#include <spi-generic.h>
#include <soc/hob_mem.h>

static void pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources = &pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_domain_scan_bus,
};

static struct device_operations cpu_bus_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init = denverton_init_cpus,
	.scan_bus = NULL,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.acpi_fill_ssdt_generator = generate_cpu_entries,
#endif
};

static void soc_enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

static void soc_init(void *data)
{
	fsp_silicon_init(false);
	soc_save_dimm_info();
}

static void soc_final(void *data) {}

static void soc_silicon_init_params(FSPS_UPD *silupd)
{
	size_t num;
	uint16_t supported_hsio_lanes;
	BL_HSIO_INFORMATION *hsio_config;
	BL_FIA_MUX_CONFIG_HOB *fiamux_hob_data = get_fiamux_hob_data();

	/* Configure FIA MUX PCD */
	supported_hsio_lanes =
		(uint16_t)fiamux_hob_data->FiaMuxConfig.SkuNumLanesAllowed;

	num = mainboard_get_hsio_config(&hsio_config);

	if (get_fiamux_hsio_info(supported_hsio_lanes, num, &hsio_config))
		die("HSIO Configuration is invalid, please correct it!");

	/* Check the requested FIA MUX Configuration */
	if (!(&hsio_config->FiaConfig)) {
		die("Requested FIA MUX Configuration is invalid,"
		    " please correct it!");
	}

	/* Initialize PCIE Bifurcation & HSIO configuration */
	silupd->FspsConfig.PcdBifurcationPcie0 = hsio_config->PcieBifCtr[0];
	silupd->FspsConfig.PcdBifurcationPcie1 = hsio_config->PcieBifCtr[1];

	silupd->FspsConfig.PcdFiaMuxConfigRequestPtr =
		(uint32_t)&hsio_config->FiaConfig;
}

void platform_fsp_silicon_init_params_cb(FSPS_UPD *silupd)
{
	const struct microcode *microcode_file;
	size_t microcode_len;

	microcode_file = cbfs_boot_map_with_leak("cpu_microcode_blob.bin",
		CBFS_TYPE_MICROCODE, &microcode_len);

	if ((microcode_file != NULL) && (microcode_len != 0)) {
		/* Update CPU Microcode patch base address/size */
		silupd->FspsConfig.PcdCpuMicrocodePatchBase =
		       (uint32_t)microcode_file;
		silupd->FspsConfig.PcdCpuMicrocodePatchSize =
		       (uint32_t)microcode_len;
	}

	soc_silicon_init_params(silupd);
	mainboard_silicon_init_params(silupd);
}

struct chip_operations soc_intel_denverton_ns_ops = {
	CHIP_NAME("Intel Denverton-NS SOC")
	.enable_dev = &soc_enable_dev,
	.init = &soc_init,
	.final = &soc_final
};

static void soc_set_subsystem(struct device *dev, uint32_t vendor,
			      uint32_t device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   ((device & 0xffff) << 16) |
					   (vendor & 0xffff));
	}
}

struct pci_operations soc_pci_ops = {
	.set_subsystem = soc_set_subsystem,
};

/*
 * spi_flash init() needs to run unconditionally on every boot (including
 * resume) to allow write protect to be disabled for eventlog and nvram
 * updates. This needs to be done as early as possible in ramstage. Thus, add a
 * callback for entry into BS_PRE_DEVICE.
 */
static void spi_flash_init_cb(void *unused)
{
	fast_spi_init();
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, spi_flash_init_cb, NULL);
