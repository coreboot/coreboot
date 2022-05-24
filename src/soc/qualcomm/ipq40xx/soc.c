/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <symbols.h>
#include <soc/ipq_uart.h>

typedef struct {
	uint8_t	hlos1[112 * MiB],	/* <-- 0x80000000 */
		appsbl[4 * MiB],	/* <-- 0x87000000 */
		sbl[1 * MiB],		/* <-- 0x87400000 */
		rsvd[11 * MiB],		/* <-- 0x87500000 */
		hlos2[128 * MiB];	/* <-- 0x88000000 */
} ipq_mem_map_t;

#define LINUX_REGION1_START	((uintptr_t)(ipq_mem_map->hlos1))
#define LINUX_REGION1_START_KB	(LINUX_REGION1_START / KiB)
#define LINUX_REGION1_SIZE	(sizeof(ipq_mem_map->hlos1) +	\
				 sizeof(ipq_mem_map->appsbl) +	\
				 sizeof(ipq_mem_map->sbl))
#define LINUX_REGION1_SIZE_KB	(LINUX_REGION1_SIZE / KiB)

#define RESERVED_START		((uintptr_t)(ipq_mem_map->rsvd))
#define RESERVED_START_KB	(RESERVED_START / KiB)
#define RESERVED_SIZE		(sizeof(ipq_mem_map->rsvd))
#define RESERVED_SIZE_KB	(RESERVED_SIZE / KiB)

/* xxx_SIZE defines not needed since it goes till end of memory */
#define LINUX_REGION2_START	((uintptr_t)(ipq_mem_map->hlos2))
#define LINUX_REGION2_START_KB	(LINUX_REGION2_START / KiB)

static void soc_read_resources(struct device *dev)
{
	ipq_mem_map_t *ipq_mem_map = ((ipq_mem_map_t *)_dram);

	ram_resource_kb(dev, 0, LINUX_REGION1_START_KB, LINUX_REGION1_SIZE_KB);

	reserved_ram_resource_kb(dev, 1, RESERVED_START_KB, RESERVED_SIZE_KB);

	/* 0x88000000 to end, is the second region for Linux */
	ram_resource_kb(dev, 2, LINUX_REGION2_START_KB,
		     (CONFIG_DRAM_SIZE_MB * KiB) -
			LINUX_REGION1_SIZE_KB - RESERVED_SIZE_KB);
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
