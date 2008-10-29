#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/hyperthreading.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>

static const uint32_t microcode_updates[] = {
	#include "microcode_m206e839.h"
	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static inline void strcpy(char *dst, char *src) 
{
	while (*src) *dst++ = *src++;
}

static void fill_processor_name(char *processor_name)
{
	struct cpuid_result regs;
	char temp_processor_name[49];
	char *processor_name_start;
	unsigned int *name_as_ints = (unsigned int *)temp_processor_name;
	int i;

	for (i=0; i<3; i++) {
		regs = cpuid(0x80000002 + i);
		name_as_ints[i*4 + 0] = regs.eax;
		name_as_ints[i*4 + 1] = regs.ebx;
		name_as_ints[i*4 + 2] = regs.ecx;
		name_as_ints[i*4 + 3] = regs.edx;
	}

	temp_processor_name[48] = 0;

	/* Skip leading spaces */
	processor_name_start = temp_processor_name;
	while (*processor_name_start == ' ') 
		processor_name_start++;

	memset(processor_name, 0, 49);
	strcpy(processor_name, processor_name_start);
}

static void model_6ex_init(device_t cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Print processor name */
	fill_processor_name(processor_name);
	printk_info("CPU: %s.\n", processor_name);

	/* Setup MTRRs */
	x86_setup_mtrrs(36);
	x86_mtrr_check();
	
	/* Enable the local cpu apics */
	setup_lapic();

	/* Start up my cpu siblings */
	intel_sibling_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_6ex_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x06e0 }, /* Intel Core Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06e8 }, /* Intel Core Solo/Core Duo */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

