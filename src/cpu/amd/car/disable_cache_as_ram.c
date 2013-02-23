/*
 * This file is part of the coreboot project.
 *
 * original idea yhlu 6.2005 (assembler code)
 *
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * be warned, this file will be used other cores and core 0 / node 0
 */

#include <cpu/x86/cache.h>

static inline __attribute__((always_inline)) void disable_cache_as_ram(void)
{
	msr_t msr;

	/* disable cache */
	write_cr0(read_cr0() | CR0_CacheDisable);

	msr.lo = 0;
	msr.hi = 0;
	wrmsr(MTRRfix4K_C8000_MSR, msr);
#if CONFIG_DCACHE_RAM_SIZE > 0x8000
	wrmsr(MTRRfix4K_C0000_MSR, msr);
#endif
	/* disable fixed mtrr from now on, it will be enabled by coreboot_ram again*/

	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~(SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrFixDramModEn);
	wrmsr(SYSCFG_MSR, msr);

	/* Set the default memory type and disable fixed and enable variable MTRRs */
	msr.hi = 0;
	msr.lo = (1 << 11);

	wrmsr(MTRRdefType_MSR, msr);

	enable_cache();
}

static void disable_cache_as_ram_bsp(void)
{
	disable_cache_as_ram();
}
