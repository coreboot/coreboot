/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <symbols.h>
#include <soc/ipq_uart.h>

#define RESERVED_SIZE	0x01500000 /* 21 MiB */

static void soc_read_resources(struct device *dev)
{
	/* Reserve bottom 21 MiB for NSS, SMEM, etc. */
	uintptr_t reserve_ram_end = (uintptr_t)_dram + RESERVED_SIZE;
	uint64_t ram_end = CONFIG_DRAM_SIZE_MB * (uint64_t)MiB;

	reserved_ram_from_to(dev, 0, (uintptr_t)_dram, reserve_ram_end);
	ram_from_to(dev, 1, reserve_ram_end, ram_end);
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
