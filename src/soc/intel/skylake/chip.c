/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/acpi.h>
#include <chip.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <string.h>

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = &pci_domain_read_resources,
	.set_resources    = &pci_domain_set_resources,
	.scan_bus         = &pci_domain_scan_bus,
	.ops_pci_bus      = &pci_bus_default_ops,
};

static struct device_operations cpu_bus_ops = {
	.init             = &soc_init_cpus,
	.acpi_fill_ssdt_generator = generate_cpu_entries,
};

static void soc_enable(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle PCH device enable */
		if (PCI_SLOT(dev->path.pci.devfn) > SA_DEV_SLOT_IGD &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			pch_enable_dev(dev);
		}
	}
}

struct chip_operations soc_intel_skylake_ops = {
	CHIP_NAME("Intel Skylake")
	.enable_dev = &soc_enable,
	.init       = &soc_init_pre_device,
};

/* UPD parameters to be initialized before SiliconInit */
void soc_silicon_init_params(SILICON_INIT_UPD *params)
{
	const struct device *dev = dev_find_slot(0, PCH_DEVFN_LPC);
	const struct soc_intel_skylake_config *config = dev->chip_info;

	memcpy(params->SerialIoDevMode, config->SerialIoDevMode,
	       sizeof(params->SerialIoDevMode));
	memcpy(params->PortUsb20Enable, config->PortUsb20Enable,
	       sizeof(params->PortUsb20Enable));
	memcpy(params->PortUsb30Enable, config->PortUsb30Enable,
	       sizeof(params->PortUsb30Enable));

	params->SataSalpSupport = config->SataSalpSupport;
	params->SataPortsEnable[0] = config->SataPortsEnable[0];
	params->SsicPortEnable = config->SsicPortEnable;
	params->SmbusEnable = config->SmbusEnable;
	params->Cio2Enable = config->Cio2Enable;
	params->ScsEmmcEnabled = config->ScsEmmcEnabled;
	params->ScsEmmcHs400Enabled = config->ScsEmmcHs400Enabled;
	params->ScsSdCardEnabled = config->ScsSdCardEnabled;
	params->IshEnable = 0;
	params->EnableAzalia = config->EnableAzalia;
	params->IoBufferOwnership = config->IoBufferOwnership;
	params->DspEnable = config->DspEnable;
	params->XdciEnable = config->XdciEnable;
	params->Device4Enable = config->Device4Enable;
	params->RtcLock = config->RtcLock;

	/* Show SPI controller if enabled in devicetree.cb */
	dev = dev_find_slot(0, PCH_DEVFN_SPI);
	params->ShowSpiController = dev->enabled;
}

void soc_display_silicon_init_params(const SILICON_INIT_UPD *original,
	SILICON_INIT_UPD *params)
{
	/* Display the parameters for SiliconInit */
	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");
	soc_display_upd_value("LogoPtr", 4,
			(uint32_t)original->LogoPtr,
			(uint32_t)params->LogoPtr);
	soc_display_upd_value("LogoSize", 4,
		(uint32_t)original->LogoSize,
		(uint32_t)params->LogoSize);
	soc_display_upd_value("GraphicsConfigPtr", 4,
		(uint32_t)original->GraphicsConfigPtr,
		(uint32_t)params->GraphicsConfigPtr);
	soc_display_upd_value("MicrocodeRegionBase", 4,
		(uint32_t)original->MicrocodeRegionBase,
		(uint32_t)params->MicrocodeRegionBase);
	soc_display_upd_value("MicrocodeRegionSize", 4,
		(uint32_t)original->MicrocodeRegionSize,
		(uint32_t)params->MicrocodeRegionSize);
	soc_display_upd_value("TurboMode", 1,
		(uint32_t)original->TurboMode,
		(uint32_t)params->TurboMode);
	soc_display_upd_value("GpioTablePtr", 4,
		(uint32_t)original->GpioTablePtr,
		(uint32_t)params->GpioTablePtr);
	soc_display_upd_value("Device4Enable", 1,
		original->Device4Enable,
		params->Device4Enable);
	soc_display_upd_value("SataSalpSupport", 1, original->SataSalpSupport,
		params->SataSalpSupport);
	soc_display_upd_value("SataPortsEnable[0]", 1,
		original->SataPortsEnable[0], params->SataPortsEnable[0]);
	soc_display_upd_value("SataPortsEnable[1]", 1,
		original->SataPortsEnable[1], params->SataPortsEnable[1]);
	soc_display_upd_value("SataPortsEnable[2]", 1,
		original->SataPortsEnable[2], params->SataPortsEnable[2]);
	soc_display_upd_value("SataPortsEnable[3]", 1,
		original->SataPortsEnable[3], params->SataPortsEnable[3]);
	soc_display_upd_value("SataPortsEnable[4]", 1,
		original->SataPortsEnable[4], params->SataPortsEnable[4]);
	soc_display_upd_value("SataPortsEnable[5]", 1,
		original->SataPortsEnable[5], params->SataPortsEnable[5]);
	soc_display_upd_value("SataPortsEnable[6]", 1,
		original->SataPortsEnable[6], params->SataPortsEnable[6]);
	soc_display_upd_value("SataPortsEnable[7]", 1,
		original->SataPortsEnable[7], params->SataPortsEnable[7]);
	soc_display_upd_value("SataPortsDevSlp[0]", 1,
		original->SataPortsDevSlp[0], params->SataPortsDevSlp[0]);
	soc_display_upd_value("SataPortsDevSlp[1]", 1,
		original->SataPortsDevSlp[1], params->SataPortsDevSlp[1]);
	soc_display_upd_value("SataPortsDevSlp[2]", 1,
		original->SataPortsDevSlp[2], params->SataPortsDevSlp[2]);
	soc_display_upd_value("SataPortsDevSlp[3]", 1,
		original->SataPortsDevSlp[3], params->SataPortsDevSlp[3]);
	soc_display_upd_value("SataPortsDevSlp[4]", 1,
		original->SataPortsDevSlp[4], params->SataPortsDevSlp[4]);
	soc_display_upd_value("SataPortsDevSlp[5]", 1,
		original->SataPortsDevSlp[5], params->SataPortsDevSlp[5]);
	soc_display_upd_value("SataPortsDevSlp[6]", 1,
		original->SataPortsDevSlp[6], params->SataPortsDevSlp[6]);
	soc_display_upd_value("SataPortsDevSlp[7]", 1,
		original->SataPortsDevSlp[7], params->SataPortsDevSlp[7]);
	soc_display_upd_value("EnableAzalia", 1,
		original->EnableAzalia,	params->EnableAzalia);
	soc_display_upd_value("DspEnable", 1, original->DspEnable,
		params->DspEnable);
	soc_display_upd_value("IoBufferOwnership", 1,
		original->IoBufferOwnership, params->IoBufferOwnership);
	soc_display_upd_value("PortUsb20Enable[0]", 1,
		original->PortUsb20Enable[0], params->PortUsb20Enable[0]);
	soc_display_upd_value("PortUsb20Enable[1]", 1,
		original->PortUsb20Enable[1], params->PortUsb20Enable[1]);
	soc_display_upd_value("PortUsb20Enable[2]", 1,
		original->PortUsb20Enable[2], params->PortUsb20Enable[2]);
	soc_display_upd_value("PortUsb20Enable[3]", 1,
		original->PortUsb20Enable[3], params->PortUsb20Enable[3]);
	soc_display_upd_value("PortUsb20Enable[4]", 1,
		original->PortUsb20Enable[4], params->PortUsb20Enable[4]);
	soc_display_upd_value("PortUsb20Enable[5]", 1,
		original->PortUsb20Enable[5], params->PortUsb20Enable[5]);
	soc_display_upd_value("PortUsb20Enable[6]", 1,
		original->PortUsb20Enable[6], params->PortUsb20Enable[6]);
	soc_display_upd_value("PortUsb20Enable[7]", 1,
		original->PortUsb20Enable[7], params->PortUsb20Enable[7]);
	soc_display_upd_value("PortUsb20Enable[8]", 1,
		original->PortUsb20Enable[8], params->PortUsb20Enable[8]);
	soc_display_upd_value("PortUsb20Enable[9]", 1,
		original->PortUsb20Enable[9], params->PortUsb20Enable[9]);
	soc_display_upd_value("PortUsb20Enable[10]", 1,
		original->PortUsb20Enable[10], params->PortUsb20Enable[10]);
	soc_display_upd_value("PortUsb20Enable[11]", 1,
		original->PortUsb20Enable[11], params->PortUsb20Enable[11]);
	soc_display_upd_value("PortUsb20Enable[12]", 1,
		original->PortUsb20Enable[12], params->PortUsb20Enable[12]);
	soc_display_upd_value("PortUsb20Enable[13]", 1,
		original->PortUsb20Enable[13], params->PortUsb20Enable[13]);
	soc_display_upd_value("PortUsb20Enable[14]", 1,
		original->PortUsb20Enable[14], params->PortUsb20Enable[14]);
	soc_display_upd_value("PortUsb20Enable[15]", 1,
		original->PortUsb20Enable[15], params->PortUsb20Enable[15]);
	soc_display_upd_value("PortUsb30Enable[0]", 1,
		original->PortUsb30Enable[0], params->PortUsb30Enable[0]);
	soc_display_upd_value("PortUsb30Enable[1]", 1,
		original->PortUsb30Enable[1], params->PortUsb30Enable[1]);
	soc_display_upd_value("PortUsb30Enable[2]", 1,
		original->PortUsb30Enable[2], params->PortUsb30Enable[2]);
	soc_display_upd_value("PortUsb30Enable[3]", 1,
		original->PortUsb30Enable[3], params->PortUsb30Enable[3]);
	soc_display_upd_value("PortUsb30Enable[4]", 1,
		original->PortUsb30Enable[4], params->PortUsb30Enable[4]);
	soc_display_upd_value("PortUsb30Enable[5]", 1,
		original->PortUsb30Enable[5], params->PortUsb30Enable[5]);
	soc_display_upd_value("PortUsb30Enable[6]", 1,
		original->PortUsb30Enable[6], params->PortUsb30Enable[6]);
	soc_display_upd_value("PortUsb30Enable[7]", 1,
		original->PortUsb30Enable[7], params->PortUsb30Enable[7]);
	soc_display_upd_value("PortUsb30Enable[8]", 1,
		original->PortUsb30Enable[8], params->PortUsb30Enable[8]);
	soc_display_upd_value("PortUsb30Enable[9]", 1,
		original->PortUsb30Enable[9], params->PortUsb30Enable[9]);
	soc_display_upd_value("XdciEnable", 1, original->XdciEnable,
		params->XdciEnable);
	soc_display_upd_value("SsicPortEnable", 1, original->SsicPortEnable,
		params->SsicPortEnable);
	soc_display_upd_value("SmbusEnable", 1, original->SmbusEnable,
		params->SmbusEnable);
	soc_display_upd_value("SerialIoDevMode[0]", 1,
		original->SerialIoDevMode[0], params->SerialIoDevMode[0]);
	soc_display_upd_value("SerialIoDevMode[1]", 1,
		original->SerialIoDevMode[1], params->SerialIoDevMode[1]);
	soc_display_upd_value("SerialIoDevMode[2]", 1,
		original->SerialIoDevMode[2], params->SerialIoDevMode[2]);
	soc_display_upd_value("SerialIoDevMode[3]", 1,
		original->SerialIoDevMode[3], params->SerialIoDevMode[3]);
	soc_display_upd_value("SerialIoDevMode[4]", 1,
		original->SerialIoDevMode[4], params->SerialIoDevMode[4]);
	soc_display_upd_value("SerialIoDevMode[5]", 1,
		original->SerialIoDevMode[5], params->SerialIoDevMode[5]);
	soc_display_upd_value("SerialIoDevMode[6]", 1,
		original->SerialIoDevMode[6], params->SerialIoDevMode[6]);
	soc_display_upd_value("SerialIoDevMode[7]", 1,
		original->SerialIoDevMode[7], params->SerialIoDevMode[7]);
	soc_display_upd_value("SerialIoDevMode[8]", 1,
		original->SerialIoDevMode[8], params->SerialIoDevMode[8]);
	soc_display_upd_value("SerialIoDevMode[9]", 1,
		original->SerialIoDevMode[9], params->SerialIoDevMode[9]);
	soc_display_upd_value("SerialIoDevMode[10]", 1,
		original->SerialIoDevMode[10], params->SerialIoDevMode[10]);
	soc_display_upd_value("Cio2Enable", 1, original->Cio2Enable,
		params->Cio2Enable);
	soc_display_upd_value("ScsEmmcEnabled", 1, original->ScsEmmcEnabled,
		params->ScsEmmcEnabled);
	soc_display_upd_value("ScsEmmcHs400Enabled", 1,
		original->ScsEmmcHs400Enabled, params->ScsEmmcHs400Enabled);
	soc_display_upd_value("ScsSdCardEnabled", 1, original->ScsSdCardEnabled,
		params->ScsSdCardEnabled);
	soc_display_upd_value("IshEnable", 1, original->IshEnable,
		params->IshEnable);
	soc_display_upd_value("ShowSpiController", 1,
		original->ShowSpiController, params->ShowSpiController);
	soc_display_upd_value("HsioMessaging", 1, original->HsioMessaging,
		params->HsioMessaging);
	soc_display_upd_value("Heci3Enabled", 1, original->Heci3Enabled,
		params->Heci3Enabled);
	soc_display_upd_value("RtcLock", 1, original->RtcLock,
		params->RtcLock);
}

static void pci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device)
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	else
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   (device << 16) | vendor);
}

struct pci_operations soc_pci_ops = {
	.set_subsystem = &pci_set_subsystem
};
