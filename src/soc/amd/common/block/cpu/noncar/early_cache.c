/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/cpu.h>
#include <assert.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <soc/iomap.h>
#include <stdint.h>

/* Allocate a static amount of stack for the MTRR context. */
#define MAX_VAR_MTRR_USE 10

/*
 * PSP performs the memory training and setting up DRAM map prior to x86 cores being released.
 * Honor TOP_MEM and set up caching from 0 til TOP_MEM. Likewise, route lower memory addresses
 * covered by fixed MTRRs to DRAM except for 0xa0000-0xc0000.
 */
void early_cache_setup(void)
{
	msr_t top_mem;
	msr_t sys_cfg;
	msr_t mtrr_def_type;
	msr_t fixed_mtrr_ram;
	msr_t fixed_mtrr_mmio;
	union mtrr_ctx {
		struct var_mtrr_context ctx;
		char buffer[sizeof(struct var_mtrr_context)
			    + 2 * MAX_VAR_MTRR_USE * sizeof(msr_t)];

	} mtrr_ctx;

	var_mtrr_context_init(&mtrr_ctx.ctx);
	mtrr_ctx.ctx.max_var_mtrrs = MIN(MAX_VAR_MTRR_USE, mtrr_ctx.ctx.max_var_mtrrs);
	top_mem = rdmsr(TOP_MEM);
	/* Enable RdDram and WrDram attributes in fixed MTRRs. */
	sys_cfg = rdmsr(SYSCFG_MSR);
	sys_cfg.lo |= SYSCFG_MSR_MtrrFixDramModEn;

	/* Fixed MTRR constants. */
	fixed_mtrr_ram.lo = fixed_mtrr_ram.hi =
		((MTRR_TYPE_WRBACK | MTRR_READ_MEM | MTRR_WRITE_MEM) <<  0) |
		((MTRR_TYPE_WRBACK | MTRR_READ_MEM | MTRR_WRITE_MEM) <<  8) |
		((MTRR_TYPE_WRBACK | MTRR_READ_MEM | MTRR_WRITE_MEM) << 16) |
		((MTRR_TYPE_WRBACK | MTRR_READ_MEM | MTRR_WRITE_MEM) << 24);
	fixed_mtrr_mmio.lo = fixed_mtrr_mmio.hi =
		((MTRR_TYPE_UNCACHEABLE) <<  0) |
		((MTRR_TYPE_UNCACHEABLE) <<  8) |
		((MTRR_TYPE_UNCACHEABLE) << 16) |
		((MTRR_TYPE_UNCACHEABLE) << 24);

	/* Prep default MTRR type. */
	mtrr_def_type = rdmsr(MTRR_DEF_TYPE_MSR);
	mtrr_def_type.lo &= ~MTRR_DEF_TYPE_MASK;
	mtrr_def_type.lo |= MTRR_TYPE_UNCACHEABLE;
	mtrr_def_type.lo |= MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN;

	disable_cache();

	wrmsr(SYSCFG_MSR, sys_cfg);

	var_mtrr_set(&mtrr_ctx.ctx, 0, ALIGN_DOWN(top_mem.lo, 8*MiB), MTRR_TYPE_WRBACK);
	/* TODO: check if we should always mark 16 MByte below 4 GByte as WRPROT */
	var_mtrr_set(&mtrr_ctx.ctx, FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	commit_mtrr_setup(&mtrr_ctx.ctx);

	/* Set up RAM caching for everything below 1MiB except for 0xa0000-0xc0000 . */
	wrmsr(MTRR_FIX_64K_00000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_16K_80000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_16K_A0000, fixed_mtrr_mmio);
	wrmsr(MTRR_FIX_4K_C0000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_C8000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_D0000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_D8000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_E0000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_E8000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_F0000, fixed_mtrr_ram);
	wrmsr(MTRR_FIX_4K_F8000, fixed_mtrr_ram);

	wrmsr(MTRR_DEF_TYPE_MSR, mtrr_def_type);

	/* Enable Fixed and Variable MTRRs. */
	sys_cfg.lo |= SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_MtrrVarDramEn;
	sys_cfg.lo |= SYSCFG_MSR_TOM2En | SYSCFG_MSR_TOM2WB;
	/* AGESA currently expects SYSCFG_MSR_MtrrFixDramModEn to be set. Once
	   MP init happens in coreboot proper it can be knocked down. */
	wrmsr(SYSCFG_MSR, sys_cfg);

	enable_cache();
}
