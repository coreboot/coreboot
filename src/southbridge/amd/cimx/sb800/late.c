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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <device/device.h>	/* device_t */
#include <device/pci.h>		/* device_operations */
#include <device/pci_ids.h>
#include <arch/ioapic.h>
#include <device/smbus.h>	/* smbus_bus_operations */
#include <pc80/mc146818rtc.h>
#include <console/console.h>	/* printk */
#include <arch/acpi.h>
#include <usbdebug.h>
#include "lpc.h"		/* lpc_read_resources */
#include "SBPLATFORM.h" 	/* Platfrom Specific Definitions */
#include "cfg.h"		/* sb800 Cimx configuration */
#include "chip.h"		/* struct southbridge_amd_cimx_sb800_config */
#include "sb_cimx.h"		/* AMD CIMX wrapper entries */
#include "smbus.h"
#include "fan.h"

/*implement in mainboard.c*/
void set_pcie_reset(void);
void set_pcie_dereset(void);


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
	printk(BIOS_DEBUG, "SB800 - Late.c - %s - Start.\n", __func__);
	switch (func) {
	case CB_SBGPP_RESET_ASSERT:
		set_pcie_reset();
		break;

	case CB_SBGPP_RESET_DEASSERT:
		set_pcie_dereset();
		break;

	case IMC_FIRMWARE_FAIL:
		break;

	default:
		break;
	}

	printk(BIOS_DEBUG, "SB800 - Late.c - %s - End.\n", __func__);
	return ret;
}

#define HOST_CAP                  0x00 /* host capabilities */
#define HOST_CTL                  0x04 /* global host control */
#define HOST_IRQ_STAT             0x08 /* interrupt status */
#define HOST_PORTS_IMPL           0x0c /* bitmap of implemented ports */

#define HOST_CTL_AHCI_EN          (1 << 31) /* AHCI enabled */
static void ahci_raid_init(struct device *dev)
{
	u8 irq = 0;
	u32 bar5, caps, ports, val;

	val = pci_read_config16(dev, PCI_CLASS_DEVICE);
	if (val == PCI_CLASS_STORAGE_SATA) {
		printk(BIOS_DEBUG, "AHCI controller ");
	} else if (val == PCI_CLASS_STORAGE_RAID) {
		printk(BIOS_DEBUG, "RAID controller ");
	} else {
		printk(BIOS_WARNING, "device class:%x, neither in ahci or raid mode\n", val);
		return;
	}

	irq = pci_read_config8(dev, PCI_INTERRUPT_LINE);
	bar5 = pci_read_config32(dev, PCI_BASE_ADDRESS_5);
	printk(BIOS_DEBUG, "IOMEM base: 0x%X, IRQ: 0x%X\n", bar5, irq);

	caps = *(volatile u32 *)(bar5 + HOST_CAP);
	caps = (caps & 0x1F) + 1;
	ports= *(volatile u32 *)(bar5 + HOST_PORTS_IMPL);
	printk(BIOS_DEBUG, "Number of Ports: 0x%x, Port implemented(bit map): 0x%x\n", caps, ports);

	/* make sure ahci is enabled */
	val = *(volatile u32 *)(bar5 + HOST_CTL);
	if (!(val & HOST_CTL_AHCI_EN)) {
		*(volatile u32 *)(bar5 + HOST_CTL) = val | HOST_CTL_AHCI_EN;
	}

	dev->command |= PCI_COMMAND_MASTER;
	pci_write_config8(dev, PCI_COMMAND, dev->command);
	printk(BIOS_DEBUG, "AHCI/RAID controller initialized\n");
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void lpc_init(device_t dev)
{
	printk(BIOS_DEBUG, "SB800 - Late.c - lpc_init - Start.\n");

	rtc_check_update_cmos_date(RTC_HAS_ALTCENTURY);

	/* Initialize the real time clock.
	 * The 0 argument tells rtc_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells rtc_init to always initialize the CMOS.
	 */
	rtc_init(0);

	printk(BIOS_DEBUG, "SB800 - Late.c - lpc_init - End.\n");
}

static struct device_operations lpc_ops = {
        .read_resources = lpc_read_resources,
        .set_resources = lpc_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init = lpc_init,
        .scan_bus = scan_static_bus,
        .ops_pci = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
        .ops = &lpc_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_LPC,
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = ahci_raid_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver ahci_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_AHCI,
};

static const struct pci_driver raid_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_RAID,
};
static const struct pci_driver raid5_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB800_SATA_RAID5,
};

static struct device_operations usb_ops = {
	.read_resources = pci_ehci_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = 0,
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


static struct device_operations azalia_ops = {
        .read_resources = pci_dev_read_resources,
        .set_resources = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init = 0,
        .scan_bus = 0,
        .ops_pci = &lops_pci,
};

static const struct pci_driver azalia_driver __pci_driver = {
        .ops = &azalia_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_HDA,
};


static struct device_operations gec_ops = {
        .read_resources = pci_dev_read_resources,
        .set_resources = pci_dev_set_resources,
        .enable_resources = pci_dev_enable_resources,
        .init = 0,
        .scan_bus = 0,
        .ops_pci = &lops_pci,
};

static const struct pci_driver gec_driver __pci_driver = {
        .ops = &gec_ops,
        .vendor = PCI_VENDOR_ID_ATI,
        .device = PCI_DEVICE_ID_ATI_SB800_GEC,
};

/**
 * @brief Enable PCI Bridge
 *
 * PcibConfig [PM_Reg: EAh], PCIDisable [Bit0]
 * 'PCIDisable' set to 0 to enable P2P bridge.
 * 'PCIDisable' set to 1 to disable P2P bridge and enable PCI interface pins
 *              to function as GPIO {GPIO 35:0}.
 */
static void pci_init(device_t dev)
{
	/* PCI Bridge SHOULD be enabled by default according to SB800 rrg,
	 * but actually was disabled in some platform, so I have to enabled it.
	 */
	RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEA, AccWidthUint8, ~BIT0, 0);
}


static struct device_operations pci_ops = {
        .read_resources = pci_bus_read_resources,
        .set_resources = pci_dev_set_resources,
        .enable_resources = pci_bus_enable_resources,
        .init = pci_init,
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
	.init             = 0,
	.scan_bus         = pci_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &lops_pci,
};

/**
 * South Bridge CIMx ramstage entry point wrapper.
 */
void sb_Before_Pci_Init(void)
{
	sb_config->StdHeader.Func = SB_BEFORE_PCI_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_After_Pci_Init(void)
{
	sb_config->StdHeader.Func = SB_AFTER_PCI_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_Mid_Post_Init(void)
{
	sb_config->StdHeader.Func = SB_MID_POST_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_Late_Post(void)
{
	sb_config->StdHeader.Func = SB_LATE_POST_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_Before_Pci_Restore_Init(void)
{
	sb_config->StdHeader.Func = SB_BEFORE_PCI_RESTORE_INIT;
	AmdSbDispatcher(sb_config);
}

void sb_After_Pci_Restore_Init(void)
{
	sb_config->StdHeader.Func = SB_AFTER_PCI_RESTORE_INIT;
	AmdSbDispatcher(sb_config);
}

/**
 * @brief SB Cimx entry point sbBeforePciInit wrapper
 */
static void sb800_enable(device_t dev)
{
	struct southbridge_amd_cimx_sb800_config *sb_chip =
		(struct southbridge_amd_cimx_sb800_config *)(dev->chip_info);

	printk(BIOS_DEBUG, "sb800_enable() ");

	switch (dev->path.pci.devfn) {
	case (0x11 << 3) | 0: /* 0:11.0  SATA */
		/* the first sb800 device */
		switch (GPP_CFGMODE) { /* config the GPP PCIe ports */
		case GPP_CFGMODE_X2200:
			abcfg_reg(0xc0, 0x01FF, 0x032); /* x2 Port_0, x2 Port_1 */
			break;
		case GPP_CFGMODE_X2110:
			abcfg_reg(0xc0, 0x01FF, 0x073); /* x2 Port_0, x1 Port_1&2 */
			break;
		case GPP_CFGMODE_X1111:
			abcfg_reg(0xc0, 0x01FF, 0x0F4); /* x1 Port_0&1&2&3 */
			break;
		case GPP_CFGMODE_X4000:
		default:
			abcfg_reg(0xc0, 0x01FF, 0x010); /* x4 Port_0 */
			break;
		}
		sb800_cimx_config(sb_config);

		if (dev->enabled) {
  			sb_config->SATAMODE.SataMode.SataController = CIMX_OPTION_ENABLED;
			if (1 == sb_chip->boot_switch_sata_ide)
				sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary.
			else if (0 == sb_chip->boot_switch_sata_ide)
				sb_config->SATAMODE.SataMode.SataIdeCombMdPriSecOpt = 1; //1 -IDE as secondary.
		} else {
  			sb_config->SATAMODE.SataMode.SataController = CIMX_OPTION_DISABLED;
		}
		break;

	case (0x14 << 3) | 0: /* 0:14:0 SMBUS */
		printk(BIOS_INFO, "sm_init().\n");
		clear_ioapic(IO_APIC_ADDR);
		/* I/O APIC IDs are normally limited to 4-bits. Enforce this limit. */
#if (CONFIG_APIC_ID_OFFSET == 0 && CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS < 16)
		/* Assign the ioapic ID the next available number after the processor core local APIC IDs */
		setup_ioapic(IO_APIC_ADDR, CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS);
#elif (CONFIG_APIC_ID_OFFSET > 0)
		/* Assign the ioapic ID the value 0. Processor APIC IDs follow. */
		setup_ioapic(IO_APIC_ADDR, 0);
#else
#error "The processor APIC IDs must be lifted to make room for the I/O APIC ID"
#endif
		break;

	case (0x14 << 3) | 1: /* 0:14:1 IDE */
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
		break;


	case (0x14 << 3) | 3: /* 0:14:3 LPC */
		/* Initialize the fans */
#if CONFIG_SB800_IMC_FAN_CONTROL
		init_sb800_IMC_fans(dev);
#elif CONFIG_SB800_MANUAL_FAN_CONTROL
		init_sb800_MANUAL_fans(dev);
#endif
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
		break;

	case (0x15 << 3) | 0: /* 0:15:0 PCIe PortA */
		{
			device_t device;
			for (device = dev; device; device = device->next) {
				if (dev->path.type != DEVICE_PATH_PCI) continue;
				if ((device->path.pci.devfn & ~7) != PCI_DEVFN(0x15,0)) break;
				sb_config->PORTCONFIG[device->path.pci.devfn & 3].PortCfg.PortPresent = device->enabled;
			}

			/*
			 * GPP_CFGMODE_X4000: PortA Lanes[3:0]
			 * GPP_CFGMODE_X2200: PortA Lanes[1:0], PortB Lanes[3:2]
			 * GPP_CFGMODE_X2110: PortA Lanes[1:0], PortB Lane2, PortC Lane3
			 * GPP_CFGMODE_X1111: PortA Lanes0, PortB Lane1, PortC Lane2, PortD Lane3
			 */
			sb_config->GppLinkConfig = sb_chip->gpp_configuration;
		}
		break;

	case (0x12 << 3) | 0: /* 0:12:0 OHCI-USB1 */
		sb_config->USBMODE.UsbMode.Ohci1 = dev->enabled;
		break;
	case (0x12 << 3) | 2: /* 0:12:2 EHCI-USB1 */
		sb_config->USBMODE.UsbMode.Ehci1 = dev->enabled;
		break;
	case (0x13 << 3) | 0: /* 0:13:0 OHCI-USB2 */
		sb_config->USBMODE.UsbMode.Ohci2 = dev->enabled;
		break;
	case (0x13 << 3) | 2: /* 0:13:2 EHCI-USB2 */
		sb_config->USBMODE.UsbMode.Ehci2 = dev->enabled;
		break;
	case (0x14 << 3) | 5: /* 0:14:5 OHCI-USB4 */
		sb_config->USBMODE.UsbMode.Ohci4 = dev->enabled;
		break;
	case (0x16 << 3) | 0: /* 0:16:0 OHCI-USB3 */
		sb_config->USBMODE.UsbMode.Ohci3 = dev->enabled;
		break;
	case (0x16 << 3) | 2: /* 0:16:2 EHCI-USB3 */
		sb_config->USBMODE.UsbMode.Ehci3 = dev->enabled;

		/* call the CIMX entry at the last sb800 device,
		 * so make sure the mainboard devicetree is complete
		 */
#if CONFIG_HAVE_ACPI_RESUME
		if (acpi_slp_type != 3)
			sb_Before_Pci_Init();
		else
			sb_Before_Pci_Restore_Init();
#else
		sb_Before_Pci_Init();
#endif
		break;

	default:
		break;
	}
}

struct chip_operations southbridge_amd_cimx_sb800_ops = {
	CHIP_NAME("ATI SB800")
	.enable_dev = sb800_enable,
};
