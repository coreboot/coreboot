/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <drivers/intel/gma/int15.h>
#include <gpio.h>
#include <intelblocks/pcr.h>
#include <mainboard/gpio.h>
#include <option.h>
#include <soc/pcr_ids.h>
#include <soc/ramstage.h>
#include <static.h>
#include <superio/nuvoton/nct6687d/nct6687d.h>

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

static void devtree_update(void)
{
	config_t *cfg = config_of_soc();
	struct device *wifi_dev = DEV_PTR(pcie_rp7);
	struct device *ssd_dev = DEV_PTR(pcie_rp17);
	struct device *ps2_dev = dev_find_slot_pnp(0x2e, NCT6687D_KBC);

	if (get_uint_option("wifi_slot_enable", 1) == 0) {
		cfg->usb2_ports[8].enable = 0;
		wifi_dev->enabled = 0;
	}

	if (get_uint_option("ssd_slot_enable", 1) == 0) {
		cfg->SataPortsEnable[4] = 0;
		ssd_dev->enabled = 0;
	}

	if (get_uint_option("hdd_slot_enable", 1) == 0) {
		cfg->SataPortsEnable[0] = 0;
		cfg->SataPortsEnable[1] = 0;
	}

	if (get_uint_option("ps2_enable", 1) == 0)
		ps2_dev->enabled = 0;
}

static void mainboard_enable(struct device *dev)
{
	mainboard_configure_gpios();
	devtree_update();
	print_board_id();
	/* Configure GPIO community 1 PWM frequency to 0.5Hz, 0% duty cycle */
	pcr_write32(PID_GPIOCOM1, 0x204, (1 << 14));
	/* Set the software update flag */
	pcr_or32(PID_GPIOCOM1, 0x204, (1 << 30));
	/* Enable PWM */
	pcr_or32(PID_GPIOCOM1, 0x204, (1 << 31));
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
