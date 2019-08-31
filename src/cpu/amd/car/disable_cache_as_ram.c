/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * WARNING: this file will be used by both any AP cores and core 0 / node 0
 */

#include <arch/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>

static __always_inline uint32_t amd_fam1x_cpu_family(void)
{
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	return family;
}

static __always_inline
void disable_cache_as_ram_real(uint8_t skip_sharedc_config)
{
	msr_t msr;
	uint32_t family;

	if (!skip_sharedc_config) {
		/* disable cache */
		write_cr0(read_cr0() | CR0_CacheDisable);

		msr.lo = 0;
		msr.hi = 0;
		wrmsr(MTRR_FIX_4K_C8000, msr);
		if (CONFIG_DCACHE_RAM_SIZE > 0x8000)
			wrmsr(MTRR_FIX_4K_C0000, msr);
		if (CONFIG_DCACHE_RAM_SIZE > 0x10000)
			wrmsr(MTRR_FIX_4K_D0000, msr);
		if (CONFIG_DCACHE_RAM_SIZE > 0x18000)
			wrmsr(MTRR_FIX_4K_D8000, msr);

		/* disable fixed mtrr from now on,
		 * it will be enabled by ramstage again
		 */
		msr = rdmsr(SYSCFG_MSR);
		msr.lo &= ~(SYSCFG_MSR_MtrrFixDramEn
			| SYSCFG_MSR_MtrrFixDramModEn);
		wrmsr(SYSCFG_MSR, msr);

		/* Set the default memory type and
		 * disable fixed and enable variable MTRRs
		 */
		msr.hi = 0;
		msr.lo = (1 << 11);

		wrmsr(MTRR_DEF_TYPE_MSR, msr);

		enable_cache();
	}

	/* INVDWBINVD = 1 */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (0x1 << 4);
	wrmsr(HWCR_MSR, msr);

	family = amd_fam1x_cpu_family();

#if CONFIG(CPU_AMD_MODEL_10XXX)
	if (family >= 0x6f) {
		/* Family 15h or later */

		/* DisSS = 0 */
		msr = rdmsr(LS_CFG_MSR);
		msr.lo &= ~(0x1 << 28);
		wrmsr(LS_CFG_MSR, msr);

		if (!skip_sharedc_config) {
			/* DisSpecTlbRld = 0 */
			msr = rdmsr(IC_CFG_MSR);
			msr.lo &= ~(0x1 << 9);
			wrmsr(IC_CFG_MSR, msr);

			/* Erratum 714: SpecNbReqDis = 0 */
			msr = rdmsr(BU_CFG2_MSR);
			msr.lo &= ~(0x1 << 8);
			wrmsr(BU_CFG2_MSR, msr);
		}

		/* DisSpecTlbRld = 0 */
		/* DisHwPf = 0 */
		msr = rdmsr(DC_CFG_MSR);
		msr.lo &= ~(0x1 << 4);
		msr.lo &= ~(0x1 << 13);
		wrmsr(DC_CFG_MSR, msr);
	}
#endif
}
