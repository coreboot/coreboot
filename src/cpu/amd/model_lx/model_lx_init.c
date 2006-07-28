#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/cache.h>


static void vsm_end_post_smi(void)
{
	__asm__ volatile (
			  "push %ax\n"
			  "mov $0x5000, %ax\n"
			  ".byte 0x0f, 0x38\n"
			  "pop %ax\n"
			  );
}

static void model_lx_init(device_t dev)
{
	printk_debug("model_lx_init\n");

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Enable the local cpu apics */
	//setup_lapic();

	vsm_end_post_smi();

	printk_debug("model_lx_init DONE\n");
};

static struct device_operations cpu_dev_ops = {
	.init	= model_lx_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x05A2 },
	{ 0, 0 },
};

static struct cpu_driver driver __cpu_driver = {
	.ops	  = &cpu_dev_ops,
	.id_table = cpu_table,
};
