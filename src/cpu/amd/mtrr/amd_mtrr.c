#include <console/console.h>
#include <device/device.h>
#include <arch/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>

/* These will likely move to some device node or cbmem. */
static uint64_t amd_topmem = 0;
static uint64_t amd_topmem2 = 0;

uint64_t bsp_topmem(void)
{
	return amd_topmem;
}

uint64_t bsp_topmem2(void)
{
	return amd_topmem2;
}

/* Take a copy of BSP CPUs TOP_MEM and TOP_MEM2 registers,
 * so they can be distributed to AP CPUs. Not strictly MTRRs,
 * but this is not that bad a place to have this code.
 */
void setup_bsp_ramtop(void)
{
	msr_t msr, msr2;

	/* TOP_MEM: the top of DRAM below 4G */
	msr = rdmsr(TOP_MEM);
	printk(BIOS_INFO,
	    "%s, TOP MEM: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
	     __func__, msr.lo, msr.hi);

	/* TOP_MEM2: the top of DRAM above 4G */
	msr2 = rdmsr(TOP_MEM2);
	printk(BIOS_INFO,
	    "%s, TOP MEM2: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
	     __func__, msr2.lo, msr2.hi);

	amd_topmem = (uint64_t) msr.hi<<32 | msr.lo;
	amd_topmem2 = (uint64_t) msr2.hi<<32 | msr2.lo;
}

static void setup_ap_ramtop(void)
{
	msr_t msr;
	uint64_t v;

	v = bsp_topmem();
	if (!v)
		return;

	msr.hi = v >> 32;
	msr.lo = (uint32_t) v;
	wrmsr(TOP_MEM, msr);

	v = bsp_topmem2();
	msr.hi = v >> 32;
	msr.lo = (uint32_t) v;
	wrmsr(TOP_MEM2, msr);
}

void amd_setup_mtrrs(void)
{
	unsigned long address_bits;
	unsigned long i;
	msr_t msr, sys_cfg;
	// Test if this CPU is a Fam 0Fh rev. F or later
	const int cpu_id = cpuid_eax(0x80000001);
	printk(BIOS_SPEW, "CPU ID 0x80000001: %x\n", cpu_id);
	const int has_tom2wb =
		 (((cpu_id>>20 )&0xf) > 0) || // ExtendedFamily > 0
		((((cpu_id>>8 )&0xf) == 0xf) && // Family == 0F
		 (((cpu_id>>16)&0xf) >= 0x4));  // Rev>=F deduced from rev tables
	if(has_tom2wb)
		printk(BIOS_DEBUG, "CPU is Fam 0Fh rev.F or later. We can use TOM2WB for any memory above 4GB\n");

	/* Enable the access to AMD RdDram and WrDram extension bits */
	disable_cache();
	sys_cfg = rdmsr(SYSCFG_MSR);
	sys_cfg.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, sys_cfg);
	enable_cache();

	/* Setup fixed MTRRs, but do not enable them just yet. */
	x86_setup_fixed_mtrrs_no_enable();

	disable_cache();

	setup_ap_ramtop();

	/* if DRAM above 4GB: set SYSCFG_MSR_TOM2En and SYSCFG_MSR_TOM2WB */
	sys_cfg.lo &= ~(SYSCFG_MSR_TOM2En | SYSCFG_MSR_TOM2WB);
	if (bsp_topmem2() > (uint64_t)1<<32) {
		sys_cfg.lo |= SYSCFG_MSR_TOM2En;
		if(has_tom2wb)
			sys_cfg.lo |= SYSCFG_MSR_TOM2WB;
	}

	/* zero the IORR's before we enable to prevent
	 * undefined side effects.
	 */
	msr.lo = msr.hi = 0;
	for(i = IORR_FIRST; i <= IORR_LAST; i++) {
		wrmsr(i, msr);
	}

	/* Enable Variable Mtrrs
	 * Enable the RdMem and WrMem bits in the fixed mtrrs.
	 * Disable access to the RdMem and WrMem in the fixed mtrr.
	 */
	sys_cfg.lo |= SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn;
	sys_cfg.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, sys_cfg);

	enable_fixed_mtrr();

	enable_cache();

	address_bits = CONFIG_CPU_ADDR_BITS; //K8 could be 40, and GH could be 48

	/* AMD specific cpuid function to query number of address bits */
	if (cpuid_eax(0x80000000) >= 0x80000008) {
		address_bits = cpuid_eax(0x80000008) & 0xff;
	}

	/* Now that I have mapped what is memory and what is not
	 * Set up the mtrrs so we can cache the memory.
	 */

	// Rev. F K8 supports has SYSCFG_MSR_TOM2WB and doesn't need
	// variable MTRR to span memory above 4GB
	// Lower revisions K8 need variable MTRR over 4GB
	x86_setup_var_mtrrs(address_bits, has_tom2wb ? 0 : 1);
}
