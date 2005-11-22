#include <console/console.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>

static void model_centaur_init(device_t dev)
{
	/* Turn on caching if we haven't already */
	x86_enable_cache();
	x86_setup_mtrrs(36);
	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_centaur_init,
};

#warning "FIXME - need correct cpu id here for VIA C3"
static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_CENTAUR, 0x0670 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0671 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0672 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0673 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0674 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0675 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0676 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0677 },		// VIA C3 Samual 2
	{ X86_VENDOR_CENTAUR, 0x0678 },		// VIA C3 Ezra
	{ X86_VENDOR_CENTAUR, 0x0680 },		// VIA C3 Ezra-T
	{ X86_VENDOR_CENTAUR, 0x0698 },		// VIA C3 Nehemiah
	{ 0, 0 },
};

static struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
