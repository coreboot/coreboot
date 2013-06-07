/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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


#include <device/device.h>      /* device_t */
#include <device/pci.h>         /* device_operations */
#include <device/pci_ids.h>
#include <arch/ioapic.h>
#include <device/smbus.h>       /* smbus_bus_operations */
#include <pc80/mc146818rtc.h>
#include <console/console.h>    /* printk */
#include <usbdebug.h>
#include "lpc.h"                /* lpc_read_resources */
#include "Platform.h"   /* Platfrom Specific Definitions */
#include "sb_cimx.h"
#include "sb700_cfg.h"                /* sb700 Cimx configuration */
#include "chip.h"               /* struct southbridge_amd_cimx_sb700_config */


/*implement in mainboard.c*/
void set_pcie_reset(void);
void set_pcie_dereset(void);

static AMDSBCFG sb_late_cfg; //global, init in sb700_cimx_config
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
u32 sb700_callout_entry(u32 func, u32 data, void* config)
{
	u32 ret = 0;

	printk(BIOS_DEBUG, "SB700 - Late.c - sb700_callout_entry - Start.\n");
	printk(BIOS_DEBUG, "SB700 - Late.c - sb700_callout_entry - End.\n");
	return ret;
}


static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static void lpc_enable_resources(device_t dev)
{

	printk(BIOS_SPEW, "SB700 - Late.c - %s - Start.\n", __func__);
	pci_dev_enable_resources(dev);
	lpc_enable_childrens_resources(dev);
	printk(BIOS_SPEW, "SB700 - Late.c - %s - End.\n", __func__);
}

static void lpc_init(device_t dev)
{
	printk(BIOS_DEBUG, "SB700 - Late.c - lpc_init - Start.\n");

	rtc_check_update_cmos_date(RTC_HAS_ALTCENTURY);

	/* Initialize the real time clock.
	 * The 0 argument tells rtc_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells rtc_init to always initialize the CMOS.
	 */
	rtc_init(0);

	printk(BIOS_DEBUG, "SB700 - Late.c - lpc_init - End.\n");
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
	.device = PCI_DEVICE_ID_ATI_SB700_LPC,
};


static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = 0,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver sata_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_SATA, //SATA IDE Mode 4390
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
	.device = PCI_DEVICE_ID_ATI_SB700_USB_18_0, /* OHCI-USB1, OHCI-USB2, OHCI-USB3 */
};

static const struct pci_driver usb_ohci3_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_USB_18_1,
};

static const struct pci_driver usb_ehci123_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_USB_18_2, /* EHCI-USB1, EHCI-USB2, EHCI-USB3 */
};

static const struct pci_driver usb_ohci4_driver __pci_driver = {
	.ops = &usb_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_USB_20_5, /* OHCI-USB4 */
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
	.device = PCI_DEVICE_ID_ATI_SB700_HDA,
};


static struct device_operations pci_ops = {
	.read_resources = pci_bus_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init = 0,
	.scan_bus = pci_scan_bridge,
	.reset_bus = pci_bus_reset,
	.ops_pci = &lops_pci,
};

static const struct pci_driver pci_driver __pci_driver = {
	.ops = &pci_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_PCI,
};


static void sb700_enable(device_t dev)
{
	struct southbridge_amd_cimx_sb700_config *sb_chip =
		(struct southbridge_amd_cimx_sb700_config *)(dev->chip_info);

	printk(BIOS_DEBUG, "sb700_enable() ");
	switch (dev->path.pci.devfn) {
		case (0x11 << 3) | 0: /* 0:11.0  SATA */
			sb700_cimx_config(sb_config);
			if (dev->enabled) {
				sb_config->SataController = CIMX_OPTION_ENABLED;
				if (1 == sb_chip->boot_switch_sata_ide)
					sb_config->SataIdeCombMdPriSecOpt = 0; //0 -IDE as primary.
				else if (0 == sb_chip->boot_switch_sata_ide)
					sb_config->SataIdeCombMdPriSecOpt = 1; //1 -IDE as secondary.
			} else {
				sb_config->SataController = CIMX_OPTION_DISABLED;
			}
			break;

		case (0x12 << 3) | 0: /* 0:12:0 OHCI-USB1 */
		case (0x12 << 3) | 2: /* 0:12:2 EHCI-USB1 */
		case (0x13 << 3) | 0: /* 0:13:0 OHCI-USB2 */
		case (0x13 << 3) | 2: /* 0:13:2 EHCI-USB2 */
			break;

		case (0x14 << 3) | 0: /* 0:14:0 SMBUS */
			{
#if 1
				u32 ioapic_base;
				printk(BIOS_DEBUG, "sm_init().\n");
				ioapic_base = IO_APIC_ADDR;
				clear_ioapic(ioapic_base);
				/* I/O APIC IDs are normally limited to 4-bits. Enforce this limit. */
#if (CONFIG_APIC_ID_OFFSET == 0 && CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS >= 1)
				/* Assign the ioapic ID the next available number after the processor core local APIC IDs */
				setup_ioapic(ioapic_base, (UINT8) (CONFIG_MAX_CPUS * CONFIG_MAX_PHYSICAL_CPUS));
#elif (CONFIG_APIC_ID_OFFSET > 0)
				/* Assign the ioapic ID the value 0. Processor APIC IDs follow. */
				setup_ioapic(ioapic_base, 0);
#else
#error "The processor APIC IDs must be lifted to make room for the I/O APIC ID"
#endif
#endif
			}
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
			break;

		case (0x14 << 3) | 4: /* 0:14:4 PCI */
			break;

		case (0x14 << 3) | 5: /* 0:14:5 OHCI-USB4 */
			/* call CIMX entry after last device enable */
			sb_Before_Pci_Init();
			break;

		default:
			break;
	}
}

struct chip_operations southbridge_amd_cimx_sb700_ops = {
	CHIP_NAME("ATI SB700")
	.enable_dev = sb700_enable,
};

/**
 * @brief SB Cimx entry point sbBeforePciInit wrapper
 */
void sb_Before_Pci_Init(void)
{
	printk(BIOS_SPEW, "sb700 %s Start\n", __func__);
	/* TODO: The sb700 cimx dispatcher not work yet, calling cimx API directly */
	//sb_config->StdHeader.Func = SB_BEFORE_PCI_INIT;
	//AmdSbDispatcher(sb_config);
	sbBeforePciInit(sb_config);
	printk(BIOS_SPEW, "sb700 %s End\n", __func__);
}

void sb_After_Pci_Init(void)
{
	printk(BIOS_SPEW, "sb700 %s Start\n", __func__);
	/* TODO: The sb700 cimx dispatcher not work yet, calling cimx API directly */
	//sb_config->StdHeader.Func = SB_AFTER_PCI_INIT;
	//AmdSbDispatcher(sb_config);
	sbAfterPciInit(sb_config);
	printk(BIOS_SPEW, "sb700 %s End\n", __func__);
}

void sb_Late_Post(void)
{
	printk(BIOS_SPEW, "sb700 %s Start\n", __func__);
	/* TODO: The sb700 cimx dispatcher not work yet, calling cimx API directly */
	//sb_config->StdHeader.Func = SB_LATE_POST_INIT;
	//AmdSbDispatcher(sb_config);
	sbLatePost(sb_config);
	printk(BIOS_SPEW, "sb700 %s End\n", __func__);
}
