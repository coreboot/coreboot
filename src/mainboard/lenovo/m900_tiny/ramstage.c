/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <gpio.h>
#include <mainboard/gpio.h>
#include <soc/ramstage.h>

static void print_board_id(void)
{
	gpio_t id_gpios[] = {
		GPP_G14,
		GPP_G13,
		GPP_G12,
	};

	printk(BIOS_INFO, "Board ID: ");

	switch (gpio_base2_value(id_gpios, ARRAY_SIZE(id_gpios))) {
	case 0:
		printk(BIOS_INFO, "ThinkCentre M900 Tiny\n");
		break;
	case 2:
	case 3:
	case 4:
		printk(BIOS_INFO, "ThinkCentre M700 Tiny\n");
		break;
	default:
		printk(BIOS_INFO, "Unknown!\n");
		break;
	}

	printk(BIOS_INFO, "Serial header %spopulated\n", !gpio_get(GPP_A22) ? "" : "un");

	printk(BIOS_INFO, "PS/2 header %spopulated\n", !gpio_get(GPP_D14) ? "" : "un");

	printk(BIOS_INFO, "USB header %spopulated\n", !gpio_get(GPP_C19) ? "" : "un");

	printk(BIOS_INFO, "DisplayPort header %spopulated\n", !gpio_get(GPP_B15) ? "" : "un");

	printk(BIOS_INFO, "PCIe / SATA header %spopulated\n", !gpio_get(GPP_B21) ? "" : "un");
}

static void mainboard_enable(struct device *dev)
{
	mainboard_configure_gpios();
	print_board_id();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
