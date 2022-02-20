/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/int15.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <superio/smsc/sch5545/sch5545.h>

#include "sch5545_ec.h"

#define SIO_PORT				0x2e

#define GPIO_CHASSIS_ID0			1
#define GPIO_VGA_CABLE_DET_L			4
#define GPIO_SKU2				7
#define GPIO_CHASSIS_ID1			17
/* Internal USB header on mainboard */
#define FLEXBAY_HEADER_CABLE_DET_L		20
#define GPIO_BOARD_REV0				21
/* Password clear jumper */
#define GPIO_PSWD_CLR				31
#define GPIO_SKU0				32
#define GPIO_SKU1				35
#define GPIO_CHASSIS_ID2			37
/* Front panel presence */
#define GPIO_FRONT_PANEL_PRESENT_L		39
#define GPIO_INTRUDER_CABLE_DET_L		44
#define GPIO_BOARD_REV1				46
#define GPIO_BOARD_REV2				68
/* Front USB 3.0 ports */
#define GPIO_USB_HEADER_DET_L			69
/* Differentiate between MT/DT on the Medium Tower and Desktop variants */
#define GPIO_FRONT_PANEL_CHASSIS_DET_L		70
/*
 * This GPIO is connected to the transistor gate. If high, it will pull the
 * HDA_SDO high. When strapped at PCH_PWROK it will enable the Flash Descriptor
 * Security Override and disable ME after chipset bringup. Alternative method
 * is to use the service jumper on the mainboard.
 */
#define GPIO_ME_MFG_MODE			74

/* These GPIOs are on SCH5545 */

/* Detect if the power switch cable is connected */
#define SIO_GPIO_FP_CBL_DET_L			25
/* Detect internal speaker connected to front cover */
#define SIO_GPIO_PCSPKR_DET_L			31

static void mainboard_enable(struct device *dev)
{
	int pin_sts;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_NONE,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);

	pin_sts = get_gpio(GPIO_CHASSIS_ID0);
	pin_sts |= get_gpio(GPIO_CHASSIS_ID1) << 1;
	pin_sts |= get_gpio(GPIO_CHASSIS_ID2) << 2;
	pin_sts |= get_gpio(GPIO_FRONT_PANEL_CHASSIS_DET_L) << 3;

	printk(BIOS_DEBUG, "Chassis type: ");
	switch (pin_sts) {
	case 0:
		printk(BIOS_DEBUG, "MT\n");
		break;
	case 3:
	case 11:
		printk(BIOS_DEBUG, "USFF\n");
		break;
	case 4:
		/* As per table in schematics, but don't know what this is */
		printk(BIOS_DEBUG, "Comoros\n");
		break;
	case 1:
	case 9:
	case 5:
	case 13:
		printk(BIOS_DEBUG, "SFF\n");
		break;
	case 8:
		printk(BIOS_DEBUG, "DT\n");
		break;
	default:
		printk(BIOS_DEBUG, "Unknown chassis type %u\n", pin_sts);
		break;
	}

	pin_sts = get_gpio(GPIO_BOARD_REV0);
	pin_sts |= get_gpio(GPIO_BOARD_REV1) << 1;
	pin_sts |= get_gpio(GPIO_BOARD_REV2) << 2;

	printk(BIOS_DEBUG, "Board revision: %d\n", pin_sts);

	pin_sts = get_gpio(GPIO_SKU0);
	pin_sts |= get_gpio(GPIO_SKU1) << 1;
	pin_sts |= get_gpio(GPIO_SKU2) << 2;

	printk(BIOS_DEBUG, "SKU ID is %d:", pin_sts);
	switch (pin_sts) {
	case 0:
		printk(BIOS_DEBUG, "TPM\n");
		break;
	case 1:
		printk(BIOS_DEBUG, "TCM\n");
		break;
	case 2:
		printk(BIOS_DEBUG, "Non TPM/TCM\n");
		break;
	default:
		printk(BIOS_DEBUG, "Unknown/reserved\n");
		break;
	}

	printk(BIOS_DEBUG, "VGA cable %sconnected\n",
	       get_gpio(GPIO_VGA_CABLE_DET_L) ? "dis" : "");

	printk(BIOS_DEBUG, "Flexbay %sattached to internal USB 2.0 header\n",
	       get_gpio(FLEXBAY_HEADER_CABLE_DET_L) ? "not " : "");

	printk(BIOS_DEBUG, "Password clear jumper %sactive\n",
	       get_gpio(GPIO_PSWD_CLR) ? "in" : "");

	if (!get_gpio(GPIO_FRONT_PANEL_PRESENT_L)) {
		printk(BIOS_DEBUG, "Front panel cable connected\n");
	} else {
		printk(BIOS_WARNING, "Front panel cable not connected!\n");
		printk(BIOS_WARNING, "Front USB 2.0 ports, SATA LED, microphone"
		       " and speaker jacks will not work!\n");
		printk(BIOS_WARNING, "Check the front panel cable!\n");
	}

	if (!get_gpio(GPIO_INTRUDER_CABLE_DET_L)) {
		printk(BIOS_DEBUG, "Intruder cable connected\n");
	} else {
		printk(BIOS_WARNING, "Intruder cable not connected!\n");
		printk(BIOS_WARNING, "Intrusion detection will not work!\n");
		printk(BIOS_WARNING, "Check the intruder cable!\n");
	}

	if (!get_gpio(GPIO_USB_HEADER_DET_L)) {
		printk(BIOS_DEBUG, "Front USB 3.0 cable connected\n");
	} else {
		printk(BIOS_WARNING, "Front USB 3.0 cable not connected!\n");
		printk(BIOS_WARNING, "Front USB 3.0 ports will not work!\n");
		printk(BIOS_WARNING, "Check the front USB 3.0 cable!\n");
	}
}

static void mainboard_final(void *chip_info)
{
	int pin_sts;
	struct device *dev = pcidev_on_root(0x1f, 0);
	const u8 pirq_routing = 11;

	pci_write_config8(dev, PIRQA_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQB_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQC_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQD_ROUT, pirq_routing);

	pci_write_config8(dev, PIRQE_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQF_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQG_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQH_ROUT, pirq_routing);

	pin_sts = sch5545_get_gpio(SIO_PORT, SIO_GPIO_FP_CBL_DET_L);

	if (pin_sts != -1) {
		if (pin_sts) {
			printk(BIOS_WARNING, "Power switch cable not connected!\n");
			printk(BIOS_WARNING, "Check power switch cable!\n");
		} else {
			printk(BIOS_DEBUG, "Power switch cable connected\n");
		}
	}

	pin_sts = sch5545_get_gpio(SIO_PORT, SIO_GPIO_PCSPKR_DET_L);

	if (pin_sts != -1)
		printk(BIOS_DEBUG, "Internal chassis PC speaker %sconnected\n",
		       pin_sts ? "not " : "");
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};

BOOT_STATE_INIT_ENTRY(BS_POST_DEVICE, BS_ON_EXIT, sch5545_ec_hwm_init, NULL);
