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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */


#include <device/device.h>	/* device_t */
#include <device/pci.h>		/* device_operations */
#include <device/pci_ids.h>
#include <device/smbus.h>	/* smbus_bus_operations */
#include <console/console.h>	/* printk */
#include "lpc.h"		/* lpc_read_resources */
#include "SBPLATFORM.h" 	/* Platfrom Specific Definitions */
#include "cfg.h"		/* sb800 Cimx configuration */
#include "chip.h" 		/* struct southbridge_amd_cimx_wrapper_sb800_config */


/*implement in mainboard.c*/
//void set_pcie_assert(void);
//void set_pcie_deassert(void);
void set_pcie_reset(void);
void set_pcie_dereset(void);


#ifndef _RAMSTAGE_
#define _RAMSTAGE_
#endif
static AMDSBCFG sb_late_cfg; //global, init in sb800_cimx_config
static AMDSBCFG *sb_config = &sb_late_cfg;


/**
 * @brief Entry point of Southbridge CIMx callout
 *
 * prototype UINT32 (*SBCIM_HOOK_ENTRY)(UINT32 Param1, UINT32 Param2, void* pConfig)
 *
 * @param[in] func      Southbridge CIMx Function ID.
 * @param[in] data      Southbridge Input Data.
 * @param[in] sb_config Southbridge configuration structure pointer.
 *
 */
u32 sb800_callout_entry(u32 func, u32 data, void* config)
{
	u32 ret = 0;

	switch (func) {
	case CB_SBGPP_RESET_ASSERT:
		//set_pcie_assert();
		set_pcie_reset();
		break;

	case CB_SBGPP_RESET_DEASSERT:
		//set_pcie_deassert();
		set_pcie_dereset();
		break;

	case IMC_FIRMWARE_FAIL:
		break;

	default:
		break;
	}

	return ret;
}


static struct pci_operations lops_pci = {
	.set_subsystem = 0,
};

static void lpc_enable_resources(device_t dev)
{

	pci_dev_enable_resources(dev);
	//lpc_enable_childrens_resources(dev);
}

static void lpc_init(device_t dev)
{
	/* SB Configure HPET base and enable bit */
	hpetInit(sb_config, &(sb_config->BuildParameters));
}

static struct device_operations lpc_ops = {
        .read_resources = lpc_read_resources,
        .set_resources = lpc_set_resources,
        .enable_resources = lpc_enable_resources,
        .init = lpc_init,
        .scan_bus = scan_static_bus,
        .ops_pci = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
        .ops = &lpc_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_LPC,
};


static void sata_enable_resources(struct device *dev)
{
	sataInitAfterPciEnum(sb_config);
	pci_dev_enable_resources(dev);
}

static void sata_init(struct device *dev)
{
	sb_config->StdHeader.Func = SB_MID_POST_INIT;
	AmdSbDispatcher(sb_config); //sataInitMidPost only
	commonInitLateBoot(sb_config);
	sataInitLatePost(sb_config);
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
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_AHCI,
};

#if CONFIG_USBDEBUG
static void usb_set_resources(struct device *dev)
{
	struct resource *res;
	u32 base;
	u32 old_debug;

	old_debug = get_ehci_debug();
	set_ehci_debug(0);

	pci_dev_set_resources(dev);

	res = find_resource(dev, 0x10);
	set_ehci_debug(old_debug);
	if (!res)
		return;
	base = res->base;
	set_ehci_base(base);
	report_resource_stored(dev, res, "");
}
#endif

static void usb_init(struct device *dev)
{
	usbInitAfterPciInit(sb_config);
	commonInitLateBoot(sb_config);
}

static struct device_operations usb_ops = {
	.read_resources = pci_dev_read_resources,
#if CONFIG_USBDEBUG
	.set_resources = usb_set_resources,
#else
	.set_resources = pci_dev_set_resources,
#endif
	.enable_resources = pci_dev_enable_resources,
	.init = usb_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

/*
 * The pci id of usb ctrl 0 and 1 are the same.
 */
static const struct pci_driver usb_ohci123_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_0, /* OHCI-USB1, OHCI-USB2, OHCI-USB3 */
};

static const struct pci_driver usb_ehci123_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_18_2, /* EHCI-USB1, EHCI-USB2, EHCI-USB3 */
};

static const struct pci_driver usb_ohci4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_USB_20_5, /* OHCI-USB4 */
};


static void azalia_init(struct device *dev)
{
	azaliaInitAfterPciEnum(sb_config); //Detect and configure High Definition Audio
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
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_HDA,
};


static void gec_init(struct device *dev)
{
	gecInitAfterPciEnum(sb_config);
	gecInitLatePost(sb_config);
	printk(BIOS_DEBUG, "gec hda enabled\n");
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
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_GEC,
};


static void pcie_init(device_t dev)
{
	sbPcieGppLateInit(sb_config);
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
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_PCI,
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
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_PCIEA,
};

/* 0:15:1 PCIe PortB */
static const struct pci_driver PORTB_driver __pci_driver = {
        .ops = &bridge_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_PCIEB,
};

/* 0:15:2 PCIe PortC */
static const struct pci_driver PORTC_driver __pci_driver = {
        .ops = &bridge_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_PCIEC,
};

/* 0:15:3 PCIe PortD */
static const struct pci_driver PORTD_driver __pci_driver = {
        .ops = &bridge_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_PCIED,
};


/**
 * @brief SB Cimx entry point sbBeforePciInit wrapper
 */
static void sb800_enable(device_t dev)
{
	u8 gpp_port = 0;
	struct southbridge_amd_cimx_wrapper_sb800_config *sb_chip =
		(struct southbridge_amd_cimx_wrapper_sb800_config *)(dev->chip_info);

	sb800_cimx_config(sb_config);
	printk(BIOS_DEBUG, "sb800_enable() ");

	/* Config SouthBridge SMBUS/ACPI/IDE/LPC/PCIB.*/
	commonInitEarlyBoot(sb_config);
	commonInitEarlyPost(sb_config);

	switch (dev->path.pci.devfn) {
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

		sataInitBeforePciEnum(sb_config); // Init SATA class code and PHY
		break;

	case (0x12 << 3) | 0: /* 0:12:0 OHCI-USB1 */
	case (0x12 << 3) | 2: /* 0:12:2 EHCI-USB1 */
	case (0x13 << 3) | 0: /* 0:13:0 OHCI-USB2 */
	case (0x13 << 3) | 2: /* 0:13:2 EHCI-USB2 */
	case (0x14 << 3) | 5: /* 0:14:5 OHCI-USB4 */
	case (0x16 << 3) | 0: /* 0:16:0 OHCI-USB3 */
	case (0x16 << 3) | 2: /* 0:16:2 EHCI-USB3 */
		usbInitBeforePciEnum(sb_config);  // USB POST TIME Only
		break;

	case (0x14 << 3) | 0: /* 0:14:0 SMBUS */
		break;

	case (0x14 << 3) | 1: /* 0:14:1 IDE */
		if (dev->enabled) {
			sb_config->SATAMODE.SataMode.SataIdeCombinedMode = ENABLED;
		} else {
  			sb_config->SATAMODE.SataMode.SataIdeCombinedMode = DISABLED;
		}
		sataInitBeforePciEnum(sb_config); // Init SATA class code and PHY
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
  		azaliaInitBeforePciEnum(sb_config); // Detect and configure High Definition Audio
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
  		gecInitBeforePciEnum(sb_config); // Init GEC
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

		sbPcieGppEarlyInit(sb_config);
		break;

	default:
		break;
	}

	/* Special setting ABCFG registers before PCI emulation. */
	abSpecialSetBeforePciEnum(sb_config);
  	usbDesertPll(sb_config);
	//sb_config->StdHeader.Func = SB_BEFORE_PCI_INIT;
	//AmdSbDispatcher(sb_config);
}

struct chip_operations southbridge_amd_cimx_wrapper_sb800_ops = {
	CHIP_NAME("ATI SB800")
	.enable_dev = sb800_enable,
};
