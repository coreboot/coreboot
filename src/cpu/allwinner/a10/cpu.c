/*
 * Ramstage initialization for Allwinner CPUs
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cbmem.h>


static void cpu_enable_resources(device_t dev)
{
	ram_resource(dev, 0, CONFIG_SYS_SDRAM_BASE >> 10,
		     CONFIG_DRAM_SIZE_MB << 10);
	/* TODO: Declare CBFS cache as reserved? There's no guarantee we won't
	 * overwrite it. It seems to stay intact, being so high in RAM
	 */
}

static void cpu_init(device_t dev)
{
	/* TODO: Check if anything else needs to be explicitly initialized */
}

static void cpu_noop(device_t dev)
{
}

static struct device_operations cpu_ops = {
	.read_resources   = cpu_noop,
	.set_resources    = cpu_noop,
	.enable_resources = cpu_enable_resources,
	.init             = cpu_init,
	.scan_bus         = NULL,
};

static void a1x_cpu_enable_dev(device_t dev)
{
	dev->ops = &cpu_ops;
}

struct chip_operations cpu_allwinner_a10_ops = {
	CHIP_NAME("CPU Allwinner A10")
	.enable_dev = a1x_cpu_enable_dev,
};
