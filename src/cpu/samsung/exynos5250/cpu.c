#include <console/console.h>
#include <device/device.h>
#include <arch/cache.h>
#include <cpu/samsung/exynos5250/cpu.h>

#define RAM_BASE_KB (CONFIG_SYS_SDRAM_BASE >> 10)
#define RAM_SIZE_KB (CONFIG_DRAM_SIZE_MB << 10UL)

static void cpu_init(device_t dev)
{
	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB);
}

static void cpu_noop(device_t dev)
{
}

static struct device_operations cpu_ops = {
	.read_resources   = cpu_noop,
	.set_resources    = cpu_noop,
	.enable_resources = cpu_noop,
	.init             = cpu_init,
	.scan_bus         = 0,
};

static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_ops;
	}
}

struct chip_operations cpu_samsung_exynos5250_ops = {
	CHIP_NAME("CPU Samsung Exynos 5250")
	.enable_dev = enable_dev,
};

void exynos5250_config_l2_cache(void)
{
	uint32_t val;

	/*
	 * Bit    9 - L2 tag RAM setup (1 cycle)
	 * Bits 8:6 - L2 tag RAM latency (3 cycles)
	 * Bit    5 - L2 data RAM setup (1 cycle)
	 * Bits 2:0 - L2 data RAM latency (3 cycles)
	 */
	val = (1 << 9) | (0x2 << 6) | (1 << 5) | (0x2);
	write_l2ctlr(val);
}
