/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <symbols.h>
#include <soc/ipq_uart.h>

#define RESERVED_SIZE_KB	(0x01500000 / KiB)

static void soc_read_resources(struct device *dev)
{
	/* Reserve bottom 0x150_0000 bytes for NSS, SMEM, etc. */
	reserved_ram_resource_kb(dev, 0, (uintptr_t)_dram / KiB, RESERVED_SIZE_KB);
	ram_resource_kb(dev, 0, (uintptr_t)_dram / KiB + RESERVED_SIZE_KB,
		     (CONFIG_DRAM_SIZE_MB * KiB) - RESERVED_SIZE_KB);
}

static void soc_init(struct device *dev)
{
	/*
	 * Do this in case console is not enabled: kernel's earlyprintk()
	 * should work no matter what the firmware console configuration is.
	 */
	ipq806x_uart_init();

	printk(BIOS_INFO, "CPU: Qualcomm 8064\n");
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init		= soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_ipq806x_ops = {
	CHIP_NAME("SOC Qualcomm 8064")
	.enable_dev = enable_soc_dev,
};
