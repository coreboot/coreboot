/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <symbols.h>
#include <soc/ipq_uart.h>

/* CONFIG_DRAM_SIZE_MB effectively extends hlos2. */
typedef struct {
	uint8_t	hlos1[112 * MiB],	/* <-- 0x80000000 */
		appsbl[4 * MiB],	/* <-- 0x87000000 */
		sbl[1 * MiB],		/* <-- 0x87400000 */
		rsvd[11 * MiB],		/* <-- 0x87500000 */
		hlos2[128 * MiB];	/* <-- 0x88000000 */
} ipq_mem_map_t;

static void soc_read_resources(struct device *dev)
{
	ipq_mem_map_t *ipq_mem_map = ((ipq_mem_map_t *)_dram);
	uint64_t ram_end = (uintptr_t)_dram + CONFIG_DRAM_SIZE_MB * (uint64_t)MiB;

	ram_from_to(dev, 0, (uintptr_t)ipq_mem_map->hlos1, (uintptr_t)ipq_mem_map->rsvd);
	reserved_ram_from_to(dev, 1, (uintptr_t)ipq_mem_map->rsvd,
			     (uintptr_t)ipq_mem_map->hlos2);

	/* 0x88000000 to end, is the second region for Linux */
	ram_from_to(dev, 2, (uintptr_t)ipq_mem_map->hlos2, ram_end);
}

static void soc_init(struct device *dev)
{
	/*
	 * Do this in case console is not enabled: kernel's earlyprintk()
	 * should work no matter what the firmware console configuration is.
	 */
	ipq40xx_uart_init();

	printk(BIOS_INFO, "CPU: QCA 40xx\n");
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init		= soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_ipq40xx_ops = {
	CHIP_NAME("SOC QCA 40xx")
	.enable_dev = enable_soc_dev,
};
