/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/cpu.h>
#include <console/console.h>
#include <cpu/amd/cpuid.h>
#include <cpu/amd/msr.h>
#include <cpu/cpu.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <device/device.h>

int get_cpu_count(void)
{
	return 1 + (cpuid_ecx(0x80000008) & 0xff);
}

unsigned int get_threads_per_core(void)
{
	return 1 + ((cpuid_ebx(CPUID_EBX_CORE_ID) & CPUID_EBX_THREADS_MASK)
		    >> CPUID_EBX_THREADS_SHIFT);
}

/* Being called via mp_run_on_all_cpus() ensures this will run on the BSP first, then APs */
static void sync_psp_addr_msr(void *unused)
{
	static msr_t psp_addr_base;
	msr_t msr_temp;

	if (psp_addr_base.raw == 0ul) {
		msr_temp = rdmsr(PSP_ADDR_MSR);
		if (msr_temp.raw == 0ul) {
			printk(BIOS_ERR, "PSP_ADDR_MSR on BSP is 0; cannot program MSR on APs\n");
			return;
		}
		psp_addr_base.lo = msr_temp.lo;
		printk(BIOS_SPEW, "Read PSP_ADDR_MSR 0x%x from BSP\n", psp_addr_base.lo);
	} else {
		msr_temp = rdmsr(PSP_ADDR_MSR);
		if (msr_temp.raw == 0ul) {
			wrmsr(PSP_ADDR_MSR, psp_addr_base);
			printk(BIOS_SPEW, "Wrote PSP_ADDR_MSR 0x%x to AP\n", psp_addr_base.lo);
		}
	}
}

static void post_mp_init(struct device *unused)
{
	if (CONFIG(SOC_AMD_COMMON_BLOCK_CPU_SYNC_PSP_ADDR_MSR))
		mp_run_on_all_cpus(sync_psp_addr_msr, NULL);
}

struct device_operations amd_cpu_bus_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.init		= mp_cpu_bus_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt	= generate_cpu_entries,
#endif
	.final = post_mp_init,
};
