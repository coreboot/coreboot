#include <console/console.h>
#include <device/device.h>

#define RAM_BASE_KB (CONFIG_SYS_SDRAM_BASE >> 10)
#define RAM_SIZE_KB (CONFIG_DRAM_SIZE_MB << 10UL)

static void domain_read_resources(device_t dev)
{
	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB);
}

static void domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static unsigned int domain_scan_bus(device_t dev, unsigned int max)
{
	return max;
}


static struct device_operations domain_ops = {
	.read_resources   = domain_read_resources,
	.set_resources    = domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = domain_scan_bus,
};

static void cpu_init(device_t dev)
{
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
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_ops;
	}
}

struct chip_operations cpu_samsung_exynos5250_ops = {
	CHIP_NAME("CPU Samsung Exynos 5250")
	.enable_dev = enable_dev,
};
