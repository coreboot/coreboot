/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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


#include <device/device.h>
#include <device/pci.h>		/* device_operations */
#include <device/pci_ids.h>
#include <device/smbus.h>	/* smbus_bus_operations */
#include <pc80/mc146818rtc.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include <console/console.h>	/* printk */
#include <device/pci_ehci.h>
#include <arch/acpi.h>
#include "lpc.h"		/* lpc_read_resources */
#include "SbPlatform.h" 	/* Platform Specific Definitions */
#include "chip.h" 		/* struct southbridge_amd_cimx_sb900_config */

#ifndef _RAMSTAGE_
#define _RAMSTAGE_
#endif
static AMDSBCFG sb_late_cfg; //global, init in sb900_cimx_config
static AMDSBCFG *sb_config = &sb_late_cfg;


/**
 * @brief Entry point of Southbridge CIMx callout
 *
 * prototype UINT32 (*SBCIM_HOOK_ENTRY)(UINT32 Param1, UINT32 Param2, void* pConfig)
 *
 * @param[in] func      Southbridge CIMx Function ID.
 * @param[in] data      Southbridge Input Data.
 * @param[in] config    Southbridge configuration structure pointer.
 *
 */
u32 sb900_callout_entry(u32 func, u32 data, void* config)
{
	u32 ret = 0;

	printk(BIOS_DEBUG, "SB900 - Late.c - sb900_callout_entry - Start.\n");
	switch (func) {
	case CB_SBGPP_RESET_ASSERT:
		break;

	case CB_SBGPP_RESET_DEASSERT:
		break;

//-	case IMC_FIRMWARE_FAIL:
//-		break;

	default:
		break;
	}

	printk(BIOS_DEBUG, "SB900 - Late.c - sb900_callout_entry - End.\n");
	return ret;
}


static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static void lpc_enable_resources(struct device *dev)
{

	printk(BIOS_DEBUG, "SB900 - Late.c - lpc_enable_resources - Start.\n");
	pci_dev_enable_resources(dev);
	//lpc_enable_childrens_resources(dev);
	printk(BIOS_DEBUG, "SB900 - Late.c - lpc_enable_resources - End.\n");
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - lpc_init - Start.\n");
	/* SB Configure HPET base and enable bit */
//-	hpetInit(sb_config, &(sb_config->BuildParameters));
	cmos_check_update_date();

	/* Initialize the real time clock.
	 * The 0 argument tells cmos_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells cmos_init to always initialize the CMOS.
	 */
	cmos_init(0);

	setup_i8259(); /* Initialize i8259 pic */
	setup_i8254(); /* Initialize i8254 timers */

	printk(BIOS_DEBUG, "SB900 - Late.c - lpc_init - End.\n");
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

static struct device_operations lpc_ops = {
	.read_resources = lpc_read_resources,
	.set_resources = lpc_set_resources,
	.enable_resources = lpc_enable_resources,
	.init = lpc_init,
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	.write_acpi_tables = acpi_write_hpet,
#endif
	.scan_bus = scan_lpc_bus,
	.ops_pci = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_LPC,
};


static void sata_enable_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - sata_enable_resources - Start.\n");
//-	sataInitAfterPciEnum(sb_config);
	pci_dev_enable_resources(dev);
	printk(BIOS_DEBUG, "SB900 - Late.c - sata_enable_resources - End.\n");
}

static void sata_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - sata_init - Start.\n");
	sb_config->StdHeader.Func = SB_MID_POST_INIT;
//-	AmdSbDispatcher(sb_config); //sataInitMidPost only
//-	commonInitLateBoot(sb_config);
//-	sataInitLatePost(sb_config);
	printk(BIOS_DEBUG, "SB900 - Late.c - sata_init - End.\n");
}

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = sata_enable_resources, //pci_dev_enable_resources,
	.init = sata_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver sata_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_AMD,
#if (CONFIG_SATA_CONTROLLER_MODE == 0x0 || CONFIG_SATA_CONTROLLER_MODE == 0x3)
	.device = PCI_DEVICE_ID_AMD_SB900_SATA,	//SATA IDE Mode
#endif
#if (CONFIG_SATA_CONTROLLER_MODE == 0x2 || CONFIG_SATA_CONTROLLER_MODE == 0x4)
	.device = PCI_DEVICE_ID_AMD_SB900_SATA_AHCI,	//SATA AHCI Mode
#endif
#if (CONFIG_SATA_CONTROLLER_MODE == 0x5 || CONFIG_SATA_CONTROLLER_MODE == 0x6)
	.device = PCI_DEVICE_ID_AMD_SB900_SATA_AMDAHCI,	//SATA AMDAHCI Mode
#endif
#if (CONFIG_SATA_CONTROLLER_MODE == 0x1 && INCHIP_SATA_FORCE_RAID5 == 0x0)
	.device = PCI_DEVICE_ID_AMD_SB900_SATA_RAID5,	//SATA RAID5 Mode
#endif
#if (CONFIG_SATA_CONTROLLER_MODE == 0x1 && INCHIP_SATA_FORCE_RAID5 == 0x1)
	.device = PCI_DEVICE_ID_AMD_SB900_SATA_RAID,	//SATA RAID Mode
#endif
};

static void usb_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - usb_init - Start.\n");
//-	usbInitAfterPciInit(sb_config);
//-	commonInitLateBoot(sb_config);
	printk(BIOS_DEBUG, "SB900 - Late.c - usb_init - End.\n");
}

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

/*
 * The pci id of usb ctrl 0 and 1 are the same.
 */
static const struct pci_driver usb_xhci123_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_16_0, /* XHCI-USB1, XHCI-USB2 */
};

static const struct pci_driver usb_ohci123_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_18_0, /* OHCI-USB1, OHCI-USB2, OHCI-USB3 */
};

static const struct pci_driver usb_ehci123_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_18_2, /* EHCI-USB1, EHCI-USB2, EHCI-USB3 */
};

static const struct pci_driver usb_ohci4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_USB_20_5, /* OHCI-USB4 */
};


static void azalia_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - azalia_init - Start.\n");
//-	azaliaInitAfterPciEnum(sb_config); //Detect and configure High Definition Audio
	printk(BIOS_DEBUG, "SB900 - Late.c - azalia_init - End.\n");
}

static struct device_operations azalia_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = azalia_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver azalia_driver __pci_driver = {
	.ops = &azalia_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_HDA,
};


static void gec_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - gec_init - Start.\n");
//-	gecInitAfterPciEnum(sb_config);
//-	gecInitLatePost(sb_config);
	printk(BIOS_DEBUG, "SB900 - Late.c - gec_init - End.\n");
}

static struct device_operations gec_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = gec_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver gec_driver __pci_driver = {
	.ops = &gec_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_GEC,
};


static void pcie_init(struct device *dev)
{
	printk(BIOS_DEBUG, "SB900 - Late.c - pcie_init - Start.\n");
//-	sbPcieGppLateInit(sb_config);
	printk(BIOS_DEBUG, "SB900 - Late.c - pcie_init - End.\n");
}

static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = pcie_init,
	.scan_bus = pci_scan_bridge,
	.reset_bus = pci_bus_reset,
	.ops_pci = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_PCI,
};


struct device_operations bridge_ops = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pcie_init,
	.scan_bus         = pci_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &lops_pci,
};

/* 0:15:0 PCIe PortA */
static const struct pci_driver PORTA_driver __pci_driver = {
	.ops = &bridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_PCIEA,
};

/* 0:15:1 PCIe PortB */
static const struct pci_driver PORTB_driver __pci_driver = {
	.ops = &bridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_PCIEB,
};

/* 0:15:2 PCIe PortC */
static const struct pci_driver PORTC_driver __pci_driver = {
	.ops = &bridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_PCIEC,
};

/* 0:15:3 PCIe PortD */
static const struct pci_driver PORTD_driver __pci_driver = {
	.ops = &bridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_SB900_PCIED,
};


/**
 * @brief SB Cimx entry point sbBeforePciInit wrapper
 */
static void sb900_enable(struct device *dev)
{
	u8 gpp_port = 0;
	struct southbridge_amd_cimx_sb900_config *sb_chip =
		(struct southbridge_amd_cimx_sb900_config *)(dev->chip_info);

	sb900_cimx_config(sb_config);
	printk(BIOS_DEBUG, "sb900_enable() ");

	/* Config SouthBridge SMBUS/ACPI/IDE/LPC/PCIB.*/
//-	commonInitEarlyBoot(sb_config);
//-	commonInitEarlyPost(sb_config);

	switch (dev->path.pci.devfn) {
	case (0x10 << 3) | 0: /* 0:10:0 XHCI-USB */
//-		usbInitBeforePciEnum(sb_config);  // USB POST TIME Only
		break;

	case (0x11 << 3) | 0: /* 0:11.0  SATA */
		if (dev->enabled) {
  			sb_config->SATAMODE.SataMode.SataController = ENABLED;
			if (1 == sb_chip->boot_switch_sata_ide)
				sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary.
			else if (0 == sb_chip->boot_switch_sata_ide)
				sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 1; //1 -IDE as secondary.
		} else {
  			sb_config->SATAMODE.SataMode.SataController = DISABLED;
		}

//-		sataInitBeforePciEnum(sb_config); // Init SATA class code and PHY
		break;

	case (0x12 << 3) | 0: /* 0:12:0 OHCI-USB1 */
	case (0x12 << 3) | 2: /* 0:12:2 EHCI-USB1 */
	case (0x13 << 3) | 0: /* 0:13:0 OHCI-USB2 */
	case (0x13 << 3) | 2: /* 0:13:2 EHCI-USB2 */
	case (0x14 << 3) | 5: /* 0:14:5 OHCI-USB4 */
//-		usbInitBeforePciEnum(sb_config);  // USB POST TIME Only
		break;

	case (0x14 << 3) | 0: /* 0:14:0 SMBUS */
		break;

	case (0x14 << 3) | 1: /* 0:14:1 IDE */
		if (dev->enabled) {
			sb_config->SATAMODE.SataMode.SataIdeCombinedMode = ENABLED;
		} else {
  			sb_config->SATAMODE.SataMode.SataIdeCombinedMode = DISABLED;
		}
//-		sataInitBeforePciEnum(sb_config); // Init SATA class code and PHY
		break;

	case (0x14 << 3) | 2: /* 0:14:2 HDA */
		if (dev->enabled) {
  			if (AZALIA_DISABLE == sb_config->AzaliaController) {
  				sb_config->AzaliaController = AZALIA_AUTO;
			}
			printk(BIOS_DEBUG, "hda enabled\n");
		} else {
  			sb_config->AzaliaController = AZALIA_DISABLE;
			printk(BIOS_DEBUG, "hda disabled\n");
		}
//-		azaliaInitBeforePciEnum(sb_config); // Detect and configure High Definition Audio
		break;


	case (0x14 << 3) | 3: /* 0:14:3 LPC */
		break;

	case (0x14 << 3) | 4: /* 0:14:4 PCI */
		break;

	case (0x14 << 3) | 6: /* 0:14:6 GEC */
		if (dev->enabled) {
			sb_config->GecConfig = 0;
			printk(BIOS_DEBUG, "gec enabled\n");
		} else {
			sb_config->GecConfig = 1;
			printk(BIOS_DEBUG, "gec disabled\n");
		}
//-		gecInitBeforePciEnum(sb_config); // Init GEC
		break;

	case (0x15 << 3) | 0: /* 0:15:0 PCIe PortA */
	case (0x15 << 3) | 1: /* 0:15:1 PCIe PortB */
	case (0x15 << 3) | 2: /* 0:15:2 PCIe PortC */
	case (0x15 << 3) | 3: /* 0:15:3 PCIe PortD */
		gpp_port = (dev->path.pci.devfn) & 0x03;
		if (dev->enabled) {
			sb_config->PORTCONFIG[gpp_port].PortCfg.PortPresent = ENABLED;
		} else {
			sb_config->PORTCONFIG[gpp_port].PortCfg.PortPresent = DISABLED;
		}

		/*
		 * GPP_CFGMODE_X4000: PortA Lanes[3:0]
		 * GPP_CFGMODE_X2200: PortA Lanes[1:0], PortB Lanes[3:2]
		 * GPP_CFGMODE_X2110: PortA Lanes[1:0], PortB Lane2, PortC Lane3
		 * GPP_CFGMODE_X1111: PortA Lanes0, PortB Lane1, PortC Lane2, PortD Lane3
		 */
		if (sb_config->GppLinkConfig != sb_chip->gpp_configuration) {
			sb_config->GppLinkConfig = sb_chip->gpp_configuration;
		}

//-		sbPcieGppEarlyInit(sb_config);
		break;

	default:
		break;
	}

	/* Special setting ABCFG registers before PCI emulation. */
//-	abSpecialSetBeforePciEnum(sb_config);
//-  	usbDesertPll(sb_config);
	//sb_config->StdHeader.Func = SB_BEFORE_PCI_INIT;
	//AmdSbDispatcher(sb_config);
}

struct chip_operations southbridge_amd_cimx_sb900_ops = {
	CHIP_NAME("ATI SB900")
	.enable_dev = sb900_enable,
};
