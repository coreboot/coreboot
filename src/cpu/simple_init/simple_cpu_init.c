#include <linux/console.h>
#include <device/device.h>
#include <device/path.h>
#include <device/cpu.h>

#if CONFIG_SMP
#error "This Configuration does not support SMP"
#endif

void initialize_cpus(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct cpu_info *info;

	/* Find the info struct for this cpu */
	info = cpu_info();
	
	/* Get the device path of the boot cpu */
	cpu_path.type = DEVICE_PATH_BOOT_CPU;

	/* Find the device struct for the boot cpu */
	info->cpu = alloc_find_dev(bus, &cpu_path);
	
	/* Initialize the bootstrap processor */
	cpu_initialize();
}
