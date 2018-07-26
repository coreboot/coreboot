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
#include <drivers/i2c/designware/dw_i2c.h>
#include <romstage_handoff.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>

/* Supplied by i2c.c */
extern struct device_operations stoneyridge_i2c_mmio_ops;
extern const char *i2c_acpi_name(const struct device *dev);

struct device_operations cpu_bus_ops = {
	.read_resources	  = DEVICE_NOOP,
	.set_resources	  = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init		  = stoney_init_cpus,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};

const char *soc_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type == DEVICE_PATH_USB) {
		switch (dev->path.usb.port_type) {
		case 0:
			/* Root Hub */
			return "RHUB";
		case 2:
			/* USB2 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "HS01";
			case 1: return "HS02";
			case 2: return "HS03";
			case 3: return "HS04";
			case 4: return "HS05";
			case 5: return "HS06";
			case 6: return "HS07";
			case 7: return "HS08";
			}
			break;
		case 3:
			/* USB3 ports */
			switch (dev->path.usb.port_id) {
			case 0: return "SS01";
			case 1: return "SS02";
			case 2: return "SS03";
			}
			break;
		}
		return NULL;
	}

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case GFX_DEVFN:
		return "IGFX";
	case PCIE0_DEVFN:
		return "PBR4";
	case PCIE1_DEVFN:
		return "PBR5";
	case PCIE2_DEVFN:
		return "PBR6";
	case PCIE3_DEVFN:
		return "PBR7";
	case PCIE4_DEVFN:
		return "PBR8";
	case HDA1_DEVFN:
		return "AZHD";
	case EHCI1_DEVFN:
		return "EHC0";
	case LPC_DEVFN:
		return "LPCB";
	case SATA_DEVFN:
		return "STCR";
	case SD_DEVFN:
		return "SDCN";
	case SMBUS_DEVFN:
		return "SBUS";
	case XHCI_DEVFN:
		return "XHC0";
	default:
		return NULL;
	}
};

struct device_operations pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = domain_set_resources,
	.enable_resources = domain_enable_resources,
	.scan_bus	  = pci_domain_scan_bus,
	.acpi_name	  = soc_acpi_name,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
	else if (dev->path.type == DEVICE_PATH_PCI)
		sb_enable(dev);
	else if (dev->path.type == DEVICE_PATH_MMIO)
		if (i2c_acpi_name(dev) != NULL)
			dev->ops = &stoneyridge_i2c_mmio_ops;
}

static void soc_init(void *chip_info)
{
	southbridge_init(chip_info);
	setup_bsp_ramtop();
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
	if (!romstage_handoff_is_resume()) {
		post_code(0x46);
		if (IS_ENABLED(CONFIG_SOC_AMD_PSP_SELECTABLE_SMU_FW))
			psp_load_named_blob(MBOX_BIOS_CMD_SMU_FW2, "smu_fw2");

		post_code(0x47);
		do_agesawrapper(agesawrapper_amdinitenv, "amdinitenv");
	} else {
		/* Complete the initial system restoration */
		post_code(0x46);
		do_agesawrapper(agesawrapper_amds3laterestore,
						"amds3laterestore");
	}
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, earliest_ramstage, NULL);
