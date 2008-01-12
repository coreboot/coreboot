#include <console/console.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
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
	{ X86_VENDOR_CENTAUR, 0x0691 },		// VIA C3 Nehemiah
	{ X86_VENDOR_CENTAUR, 0x0692 },		// VIA C3 Nehemiah
	{ X86_VENDOR_CENTAUR, 0x0693 }, 	// VIA C3 Nehemiah  
	{ X86_VENDOR_CENTAUR, 0x0694 },		// VIA C3 Nehemiah
	{ X86_VENDOR_CENTAUR, 0x0695 }, 	// VIA C3 Nehemiah  
	{ X86_VENDOR_CENTAUR, 0x0696 }, 	// VIA C3 Nehemiah  
	{ X86_VENDOR_CENTAUR, 0x0697 }, 	// VIA C3 Nehemiah  
	{ X86_VENDOR_CENTAUR, 0x0698 },		// VIA C3 Nehemiah
	{ X86_VENDOR_CENTAUR, 0x0699 },		// VIA C3 Nehemiah
	{ X86_VENDOR_CENTAUR, 0x069A },		// VIA C3 Nehemiah
	/* Some of these may not actually exist */
	{ X86_VENDOR_CENTAUR, 0x06A0 },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06A8 },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06A9 },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06AA },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06AB },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06AC },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06AD },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06AE },		// VIA C7 Esther
	{ X86_VENDOR_CENTAUR, 0x06AF },		// VIA C7 Esther
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
