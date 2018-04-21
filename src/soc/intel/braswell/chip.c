/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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
#include <compiler.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <fsp/util.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

static void pci_domain_set_resources(device_t dev)
{
	printk(BIOS_SPEW, "%s/%s ( %s )\n",
			__FILE__, __func__, dev_name(dev));
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus      = pci_bus_default_ops,
};

static void cpu_bus_noop(device_t dev) { }

static struct device_operations cpu_bus_ops = {
	.read_resources   = cpu_bus_noop,
	.set_resources    = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init             = soc_init_cpus
};


static void enable_dev(device_t dev)
{
	printk(BIOS_SPEW, "----------\n%s/%s ( %s ), type: %d\n",
			__FILE__, __func__,
			dev_name(dev), dev->path.type);
	printk(BIOS_SPEW, "vendor: 0x%04x. device: 0x%04x\n",
			pci_read_config16(dev, PCI_VENDOR_ID),
			pci_read_config16(dev, PCI_DEVICE_ID));
	printk(BIOS_SPEW, "class: 0x%02x %s\n"
			"subclass: 0x%02x %s\n"
			"prog: 0x%02x\n"
			"revision: 0x%02x\n",
			pci_read_config16(dev, PCI_CLASS_DEVICE) >> 8,
			get_pci_class_name(dev),
			pci_read_config16(dev, PCI_CLASS_DEVICE) & 0xff,
			get_pci_subclass_name(dev),
			pci_read_config8(dev, PCI_CLASS_PROG),
			pci_read_config8(dev, PCI_REVISION_ID));

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		/* Handle south cluster enablement. */
		if (PCI_SLOT(dev->path.pci.devfn) > GFX_DEV &&
		    (dev->ops == NULL || dev->ops->enable == NULL)) {
			southcluster_enable_dev(dev);
		}
	}
}

__weak void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
}

void soc_silicon_init_params(SILICON_INIT_UPD *params)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));
	struct soc_intel_braswell_config *config;

	if (!dev) {
		printk(BIOS_ERR,
			"Error! Device (%s) not found, "
			"soc_silicon_init_params!\n", dev_path(dev));
		return;
	}

	config = dev->chip_info;

	/* Set the parameters for SiliconInit */
	printk(BIOS_DEBUG, "Updating UPD values for SiliconInit\n");
	params->PcdSdcardMode = config->PcdSdcardMode;
	params->PcdEnableHsuart0 = config->PcdEnableHsuart0;
	params->PcdEnableHsuart1 = config->PcdEnableHsuart1;
	params->PcdEnableAzalia = config->PcdEnableAzalia;
	params->PcdEnableSata = config->PcdEnableSata;
	params->PcdEnableXhci = config->PcdEnableXhci;
	params->PcdEnableLpe = config->PcdEnableLpe;
	params->PcdEnableDma0 = config->PcdEnableDma0;
	params->PcdEnableDma1 = config->PcdEnableDma1;
	params->PcdEnableI2C0 = config->PcdEnableI2C0;
	params->PcdEnableI2C1 = config->PcdEnableI2C1;
	params->PcdEnableI2C2 = config->PcdEnableI2C2;
	params->PcdEnableI2C3 = config->PcdEnableI2C3;
	params->PcdEnableI2C4 = config->PcdEnableI2C4;
	params->PcdEnableI2C5 = config->PcdEnableI2C5;
	params->PcdEnableI2C6 = config->PcdEnableI2C6;
	params->GraphicsConfigPtr = 0;
	params->AzaliaConfigPtr = 0;
	params->PunitPwrConfigDisable = config->PunitPwrConfigDisable;
	params->ChvSvidConfig = config->ChvSvidConfig;
	params->DptfDisable = config->DptfDisable;
	params->PcdEmmcMode = config->PcdEmmcMode;
	params->PcdUsb3ClkSsc = config->PcdUsb3ClkSsc;
	params->PcdDispClkSsc = config->PcdDispClkSsc;
	params->PcdSataClkSsc = config->PcdSataClkSsc;
	params->Usb2Port0PerPortPeTxiSet = config->Usb2Port0PerPortPeTxiSet;
	params->Usb2Port0PerPortTxiSet = config->Usb2Port0PerPortTxiSet;
	params->Usb2Port0IUsbTxEmphasisEn = config->Usb2Port0IUsbTxEmphasisEn;
	params->Usb2Port0PerPortTxPeHalf = config->Usb2Port0PerPortTxPeHalf;
	if (config->D0Usb2Port0PerPortRXISet != 0)
		params->D0Usb2Port0PerPortRXISet = config->D0Usb2Port0PerPortRXISet;

	params->Usb2Port1PerPortPeTxiSet = config->Usb2Port1PerPortPeTxiSet;
	params->Usb2Port1PerPortTxiSet = config->Usb2Port1PerPortTxiSet;
	params->Usb2Port1IUsbTxEmphasisEn = config->Usb2Port1IUsbTxEmphasisEn;
	params->Usb2Port1PerPortTxPeHalf = config->Usb2Port1PerPortTxPeHalf;
	if (config->D0Usb2Port1PerPortRXISet != 0)
		params->D0Usb2Port1PerPortRXISet = config->D0Usb2Port1PerPortRXISet;

	params->Usb2Port2PerPortPeTxiSet = config->Usb2Port2PerPortPeTxiSet;
	params->Usb2Port2PerPortTxiSet = config->Usb2Port2PerPortTxiSet;
	params->Usb2Port2IUsbTxEmphasisEn = config->Usb2Port2IUsbTxEmphasisEn;
	params->Usb2Port2PerPortTxPeHalf = config->Usb2Port2PerPortTxPeHalf;
	if (config->D0Usb2Port2PerPortRXISet != 0)
		params->D0Usb2Port2PerPortRXISet = config->D0Usb2Port2PerPortRXISet;

	params->Usb2Port3PerPortPeTxiSet = config->Usb2Port3PerPortPeTxiSet;
	params->Usb2Port3PerPortTxiSet = config->Usb2Port3PerPortTxiSet;
	params->Usb2Port3IUsbTxEmphasisEn = config->Usb2Port3IUsbTxEmphasisEn;
	params->Usb2Port3PerPortTxPeHalf = config->Usb2Port3PerPortTxPeHalf;
	if (config->D0Usb2Port3PerPortRXISet != 0)
		params->D0Usb2Port3PerPortRXISet = config->D0Usb2Port3PerPortRXISet;

	params->Usb2Port4PerPortPeTxiSet = config->Usb2Port4PerPortPeTxiSet;
	params->Usb2Port4PerPortTxiSet = config->Usb2Port4PerPortTxiSet;
	params->Usb2Port4IUsbTxEmphasisEn = config->Usb2Port4IUsbTxEmphasisEn;
	params->Usb2Port4PerPortTxPeHalf = config->Usb2Port4PerPortTxPeHalf;
	if (config->D0Usb2Port4PerPortRXISet != 0)
		params->D0Usb2Port4PerPortRXISet = config->D0Usb2Port4PerPortRXISet;

	params->Usb3Lane0Ow2tapgen2deemph3p5 =
		config->Usb3Lane0Ow2tapgen2deemph3p5;
	params->Usb3Lane1Ow2tapgen2deemph3p5 =
		config->Usb3Lane1Ow2tapgen2deemph3p5;
	params->Usb3Lane2Ow2tapgen2deemph3p5 =
		config->Usb3Lane2Ow2tapgen2deemph3p5;
	params->Usb3Lane3Ow2tapgen2deemph3p5 =
		config->Usb3Lane3Ow2tapgen2deemph3p5;
	params->PcdSataInterfaceSpeed = config->PcdSataInterfaceSpeed;
	params->PcdPchUsbSsicPort = config->PcdPchUsbSsicPort;
	params->PcdPchUsbHsicPort = config->PcdPchUsbHsicPort;
	params->PcdPcieRootPortSpeed = config->PcdPcieRootPortSpeed;
	params->PcdPchSsicEnable = config->PcdPchSsicEnable;
	params->PcdLogoPtr = config->PcdLogoPtr;
	params->PcdLogoSize = config->PcdLogoSize;
	params->PcdRtcLock = config->PcdRtcLock;
	params->PMIC_I2CBus = config->PMIC_I2CBus;
	params->ISPEnable = config->ISPEnable;
	params->ISPPciDevConfig = config->ISPPciDevConfig;
	params->PcdSdDetectChk = config->PcdSdDetectChk;
	params->I2C0Frequency = config->I2C0Frequency;
	params->I2C1Frequency = config->I2C1Frequency;
	params->I2C2Frequency = config->I2C2Frequency;
	params->I2C3Frequency = config->I2C3Frequency;
	params->I2C4Frequency = config->I2C4Frequency;
	params->I2C5Frequency = config->I2C5Frequency;
	params->I2C6Frequency = config->I2C6Frequency;

	board_silicon_USB2_override(params);
}

void soc_display_silicon_init_params(const SILICON_INIT_UPD *old,
	SILICON_INIT_UPD *new)
{
	/* Display the parameters for SiliconInit */
	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");
	fsp_display_upd_value("PcdSdcardMode", 1, old->PcdSdcardMode,
		new->PcdSdcardMode);
	fsp_display_upd_value("PcdEnableHsuart0", 1, old->PcdEnableHsuart0,
		new->PcdEnableHsuart0);
	fsp_display_upd_value("PcdEnableHsuart1", 1, old->PcdEnableHsuart1,
		new->PcdEnableHsuart1);
	fsp_display_upd_value("PcdEnableAzalia", 1, old->PcdEnableAzalia,
		new->PcdEnableAzalia);
	fsp_display_upd_value("AzaliaConfigPtr", 4,
			(uint32_t)old->AzaliaConfigPtr,
			(uint32_t)new->AzaliaConfigPtr);
	fsp_display_upd_value("PcdEnableSata", 1, old->PcdEnableSata,
		new->PcdEnableSata);
	fsp_display_upd_value("PcdEnableXhci", 1, old->PcdEnableXhci,
		new->PcdEnableXhci);
	fsp_display_upd_value("PcdEnableLpe", 1, old->PcdEnableLpe,
		new->PcdEnableLpe);
	fsp_display_upd_value("PcdEnableDma0", 1, old->PcdEnableDma0,
		new->PcdEnableDma0);
	fsp_display_upd_value("PcdEnableDma1", 1, old->PcdEnableDma1,
		new->PcdEnableDma1);
	fsp_display_upd_value("PcdEnableI2C0", 1, old->PcdEnableI2C0,
		new->PcdEnableI2C0);
	fsp_display_upd_value("PcdEnableI2C1", 1, old->PcdEnableI2C1,
		new->PcdEnableI2C1);
	fsp_display_upd_value("PcdEnableI2C2", 1, old->PcdEnableI2C2,
		new->PcdEnableI2C2);
	fsp_display_upd_value("PcdEnableI2C3", 1, old->PcdEnableI2C3,
		new->PcdEnableI2C3);
	fsp_display_upd_value("PcdEnableI2C4", 1, old->PcdEnableI2C4,
		new->PcdEnableI2C4);
	fsp_display_upd_value("PcdEnableI2C5", 1, old->PcdEnableI2C5,
		new->PcdEnableI2C5);
	fsp_display_upd_value("PcdEnableI2C6", 1, old->PcdEnableI2C6,
		new->PcdEnableI2C6);
	fsp_display_upd_value("PcdGraphicsConfigPtr", 4,
		old->GraphicsConfigPtr, new->GraphicsConfigPtr);
	fsp_display_upd_value("GpioFamilyInitTablePtr", 4,
		(uint32_t)old->GpioFamilyInitTablePtr,
		(uint32_t)new->GpioFamilyInitTablePtr);
	fsp_display_upd_value("GpioPadInitTablePtr", 4,
		(uint32_t)old->GpioPadInitTablePtr,
		(uint32_t)new->GpioPadInitTablePtr);
	fsp_display_upd_value("PunitPwrConfigDisable", 1,
		old->PunitPwrConfigDisable,
		new->PunitPwrConfigDisable);
	fsp_display_upd_value("ChvSvidConfig", 1, old->ChvSvidConfig,
		new->ChvSvidConfig);
	fsp_display_upd_value("DptfDisable", 1, old->DptfDisable,
		new->DptfDisable);
	fsp_display_upd_value("PcdEmmcMode", 1, old->PcdEmmcMode,
		new->PcdEmmcMode);
	fsp_display_upd_value("PcdUsb3ClkSsc", 1, old->PcdUsb3ClkSsc,
		new->PcdUsb3ClkSsc);
	fsp_display_upd_value("PcdDispClkSsc", 1, old->PcdDispClkSsc,
		new->PcdDispClkSsc);
	fsp_display_upd_value("PcdSataClkSsc", 1, old->PcdSataClkSsc,
		new->PcdSataClkSsc);
	fsp_display_upd_value("Usb2Port0PerPortPeTxiSet", 1,
		old->Usb2Port0PerPortPeTxiSet,
		new->Usb2Port0PerPortPeTxiSet);
	fsp_display_upd_value("Usb2Port0PerPortTxiSet", 1,
		old->Usb2Port0PerPortTxiSet,
		new->Usb2Port0PerPortTxiSet);
	fsp_display_upd_value("Usb2Port0IUsbTxEmphasisEn", 1,
		old->Usb2Port0IUsbTxEmphasisEn,
		new->Usb2Port0IUsbTxEmphasisEn);
	fsp_display_upd_value("Usb2Port0PerPortTxPeHalf", 1,
		old->Usb2Port0PerPortTxPeHalf,
		new->Usb2Port0PerPortTxPeHalf);
	fsp_display_upd_value("D0Usb2Port0PerPortRXISet", 1,
		old->D0Usb2Port0PerPortRXISet,
		new->D0Usb2Port0PerPortRXISet);
	fsp_display_upd_value("Usb2Port1PerPortPeTxiSet", 1,
		old->Usb2Port1PerPortPeTxiSet,
		new->Usb2Port1PerPortPeTxiSet);
	fsp_display_upd_value("Usb2Port1PerPortTxiSet", 1,
		old->Usb2Port1PerPortTxiSet,
		new->Usb2Port1PerPortTxiSet);
	fsp_display_upd_value("Usb2Port1IUsbTxEmphasisEn", 1,
		old->Usb2Port1IUsbTxEmphasisEn,
		new->Usb2Port1IUsbTxEmphasisEn);
	fsp_display_upd_value("Usb2Port1PerPortTxPeHalf", 1,
		old->Usb2Port1PerPortTxPeHalf,
		new->Usb2Port1PerPortTxPeHalf);
	fsp_display_upd_value("D0Usb2Port1PerPortRXISet", 1,
		old->D0Usb2Port1PerPortRXISet,
		new->D0Usb2Port1PerPortRXISet);
	fsp_display_upd_value("Usb2Port2PerPortPeTxiSet", 1,
		old->Usb2Port2PerPortPeTxiSet,
		new->Usb2Port2PerPortPeTxiSet);
	fsp_display_upd_value("Usb2Port2PerPortTxiSet", 1,
		old->Usb2Port2PerPortTxiSet,
		new->Usb2Port2PerPortTxiSet);
	fsp_display_upd_value("Usb2Port2IUsbTxEmphasisEn", 1,
		old->Usb2Port2IUsbTxEmphasisEn,
		new->Usb2Port2IUsbTxEmphasisEn);
	fsp_display_upd_value("Usb2Port2PerPortTxPeHalf", 1,
		old->Usb2Port2PerPortTxPeHalf,
		new->Usb2Port2PerPortTxPeHalf);
	fsp_display_upd_value("D0Usb2Port2PerPortRXISet", 1,
		old->D0Usb2Port2PerPortRXISet,
		new->D0Usb2Port2PerPortRXISet);
	fsp_display_upd_value("Usb2Port3PerPortPeTxiSet", 1,
		old->Usb2Port3PerPortPeTxiSet,
		new->Usb2Port3PerPortPeTxiSet);
	fsp_display_upd_value("Usb2Port3PerPortTxiSet", 1,
		old->Usb2Port3PerPortTxiSet,
		new->Usb2Port3PerPortTxiSet);
	fsp_display_upd_value("Usb2Port3IUsbTxEmphasisEn", 1,
		old->Usb2Port3IUsbTxEmphasisEn,
		new->Usb2Port3IUsbTxEmphasisEn);
	fsp_display_upd_value("Usb2Port3PerPortTxPeHalf", 1,
		old->Usb2Port3PerPortTxPeHalf,
		new->Usb2Port3PerPortTxPeHalf);
	fsp_display_upd_value("D0Usb2Port3PerPortRXISet", 1,
		old->D0Usb2Port3PerPortRXISet,
		new->D0Usb2Port3PerPortRXISet);
	fsp_display_upd_value("Usb2Port4PerPortPeTxiSet", 1,
		old->Usb2Port4PerPortPeTxiSet,
		new->Usb2Port4PerPortPeTxiSet);
	fsp_display_upd_value("Usb2Port4PerPortTxiSet", 1,
		old->Usb2Port4PerPortTxiSet,
		new->Usb2Port4PerPortTxiSet);
	fsp_display_upd_value("Usb2Port4IUsbTxEmphasisEn", 1,
		old->Usb2Port4IUsbTxEmphasisEn,
		new->Usb2Port4IUsbTxEmphasisEn);
	fsp_display_upd_value("Usb2Port4PerPortTxPeHalf", 1,
		old->Usb2Port4PerPortTxPeHalf,
		new->Usb2Port4PerPortTxPeHalf);
	fsp_display_upd_value("D0Usb2Port4PerPortRXISet", 1,
		old->D0Usb2Port4PerPortRXISet,
		new->D0Usb2Port4PerPortRXISet);
	fsp_display_upd_value("Usb3Lane0Ow2tapgen2deemph3p5", 1,
		old->Usb3Lane0Ow2tapgen2deemph3p5,
		new->Usb3Lane0Ow2tapgen2deemph3p5);
	fsp_display_upd_value("Usb3Lane1Ow2tapgen2deemph3p5", 1,
		old->Usb3Lane1Ow2tapgen2deemph3p5,
		new->Usb3Lane1Ow2tapgen2deemph3p5);
	fsp_display_upd_value("Usb3Lane2Ow2tapgen2deemph3p5", 1,
		old->Usb3Lane2Ow2tapgen2deemph3p5,
		new->Usb3Lane2Ow2tapgen2deemph3p5);
	fsp_display_upd_value("Usb3Lane3Ow2tapgen2deemph3p5", 1,
		old->Usb3Lane3Ow2tapgen2deemph3p5,
		new->Usb3Lane3Ow2tapgen2deemph3p5);
	fsp_display_upd_value("PcdSataInterfaceSpeed", 1,
		old->PcdSataInterfaceSpeed,
		new->PcdSataInterfaceSpeed);
	fsp_display_upd_value("PcdPchUsbSsicPort", 1,
		old->PcdPchUsbSsicPort, new->PcdPchUsbSsicPort);
	fsp_display_upd_value("PcdPchUsbHsicPort", 1,
		old->PcdPchUsbHsicPort, new->PcdPchUsbHsicPort);
	fsp_display_upd_value("PcdPcieRootPortSpeed", 1,
		old->PcdPcieRootPortSpeed, new->PcdPcieRootPortSpeed);
	fsp_display_upd_value("PcdPchSsicEnable", 1, old->PcdPchSsicEnable,
		new->PcdPchSsicEnable);
	fsp_display_upd_value("PcdLogoPtr", 4, old->PcdLogoPtr,
		new->PcdLogoPtr);
	fsp_display_upd_value("PcdLogoSize", 4, old->PcdLogoSize,
		new->PcdLogoSize);
	fsp_display_upd_value("PcdRtcLock", 1, old->PcdRtcLock,
		new->PcdRtcLock);
	fsp_display_upd_value("PMIC_I2CBus", 1,
		old->PMIC_I2CBus, new->PMIC_I2CBus);
	fsp_display_upd_value("ISPEnable", 1,
		old->ISPEnable, new->ISPEnable);
	fsp_display_upd_value("ISPPciDevConfig", 1,
		old->ISPPciDevConfig, new->ISPPciDevConfig);
	fsp_display_upd_value("PcdSdDetectChk", 1,
		old->PcdSdDetectChk, new->PcdSdDetectChk);
}

/* Called at BS_DEV_INIT_CHIPS time -- very early. Just after BS_PRE_DEVICE. */
static void soc_init(void *chip_info)
{
	printk(BIOS_SPEW, "%s/%s\n", __FILE__, __func__);
	soc_init_pre_device(chip_info);
}

struct chip_operations soc_intel_braswell_ops = {
	CHIP_NAME("Intel Braswell SoC")
	.enable_dev = enable_dev,
	.init = soc_init,
};

static void pci_set_subsystem(device_t dev, unsigned int vendor,
	unsigned int device)
{
	printk(BIOS_SPEW, "%s/%s ( %s, 0x%04x, 0x%04x )\n",
			__FILE__, __func__, dev_name(dev), vendor, device);
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

struct pci_operations soc_pci_ops = {
	.set_subsystem = &pci_set_subsystem,
};

/**
  Return SoC stepping type

  @retval SOC_STEPPING            SoC stepping type
**/
int SocStepping(void)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));
	u8 revid = pci_read_config8(dev, 0x8);

	switch (revid & B_PCH_LPC_RID_STEPPING_MASK) {
	case V_PCH_LPC_RID_A0:
	  return SocA0;
	case V_PCH_LPC_RID_A1:
	  return SocA1;
	case V_PCH_LPC_RID_A2:
	  return SocA2;
	case V_PCH_LPC_RID_A3:
	  return SocA3;
	case V_PCH_LPC_RID_A4:
	  return SocA4;
	case V_PCH_LPC_RID_A5:
	  return SocA5;
	case V_PCH_LPC_RID_A6:
	  return SocA6;
	case V_PCH_LPC_RID_A7:
	  return SocA7;
	case V_PCH_LPC_RID_B0:
	  return SocB0;
	case V_PCH_LPC_RID_B1:
	  return SocB1;
	case V_PCH_LPC_RID_B2:
	  return SocB2;
	case V_PCH_LPC_RID_B3:
	  return SocB3;
	case V_PCH_LPC_RID_B4:
	  return SocB4;
	case V_PCH_LPC_RID_B5:
	  return SocB5;
	case V_PCH_LPC_RID_B6:
	  return SocB6;
	case V_PCH_LPC_RID_B7:
	  return SocB7;
	case V_PCH_LPC_RID_C0:
	  return SocC0;
	case V_PCH_LPC_RID_C1:
	  return SocC1;
	case V_PCH_LPC_RID_C2:
	  return SocC2;
	case V_PCH_LPC_RID_C3:
	  return SocC3;
	case V_PCH_LPC_RID_C4:
	  return SocC4;
	case V_PCH_LPC_RID_C5:
	  return SocC5;
	case V_PCH_LPC_RID_C6:
	  return SocC6;
	case V_PCH_LPC_RID_C7:
	  return SocC7;
	case V_PCH_LPC_RID_D0:
	  return SocD0;
	case V_PCH_LPC_RID_D1:
	  return SocD1;
	case V_PCH_LPC_RID_D2:
	  return SocD2;
	case V_PCH_LPC_RID_D3:
	  return SocD3;
	case V_PCH_LPC_RID_D4:
	  return SocD4;
	case V_PCH_LPC_RID_D5:
	  return SocD5;
	case V_PCH_LPC_RID_D6:
	  return SocD6;
	case V_PCH_LPC_RID_D7:
	  return SocD7;
	default:
	  return SocSteppingMax;
	}
}
