/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <soc/mtrr.h>

/* Picasso defines 8 Variable MTRRs */
#define MAX_VARIABLE_MTRRS 8
#define SYS_CFG_MTRR_BITS ( \
SYSCFG_MSR_TOM2WB | \
SYSCFG_MSR_TOM2En | \
SYSCFG_MSR_MtrrVarDramEn | \
SYSCFG_MSR_MtrrFixDramModEn | \
SYSCFG_MSR_MtrrFixDramEn \
)

static const unsigned int fixed_mtrr_offsets[] = {
	MTRR_FIX_64K_00000,
	MTRR_FIX_16K_80000,
	MTRR_FIX_16K_A0000,
	MTRR_FIX_4K_C0000,
	MTRR_FIX_4K_C8000,
	MTRR_FIX_4K_D0000,
	MTRR_FIX_4K_D8000,
	MTRR_FIX_4K_E0000,
	MTRR_FIX_4K_E8000,
	MTRR_FIX_4K_F0000,
	MTRR_FIX_4K_F8000,
};

static int mtrrs_saved;
static msr_t sys_cfg;
static msr_t mtrr_def;
static msr_t mtrr_base[MAX_VARIABLE_MTRRS];
static msr_t mtrr_mask[MAX_VARIABLE_MTRRS];
static msr_t fixed_mtrrs[ARRAY_SIZE(fixed_mtrr_offsets)];

void picasso_save_mtrrs(void)
{
	unsigned int i;
	int mtrrs;

	mtrrs = get_var_mtrr_count();

	ASSERT_MSG(mtrrs == MAX_VARIABLE_MTRRS, "Unexpected number of MTRRs\n");

	for (i = 0; i < MAX_VARIABLE_MTRRS; ++i) {
		mtrr_base[i] = rdmsr(MTRR_PHYS_BASE(i));
		mtrr_mask[i] = rdmsr(MTRR_PHYS_MASK(i));
		printk(BIOS_DEBUG,
		       "Saving Variable MTRR %d: Base: 0x%08x 0x%08x, Mask: 0x%08x 0x%08x\n", i,
		       mtrr_base[i].hi, mtrr_base[i].lo, mtrr_mask[i].hi, mtrr_mask[i].lo);
	}

	for (i = 0; i < ARRAY_SIZE(fixed_mtrr_offsets); ++i) {
		fixed_mtrrs[i] = rdmsr(fixed_mtrr_offsets[i]);
		printk(BIOS_DEBUG, "Saving Fixed MTRR %u: 0x%08x 0x%08x\n", i,
		       fixed_mtrrs[i].hi, fixed_mtrrs[i].lo);
	}

	mtrr_def = rdmsr(MTRR_DEF_TYPE_MSR);
	printk(BIOS_DEBUG, "Saving Default Type MTRR: 0x%08x 0x%08x\n", mtrr_def.hi,
	       mtrr_def.lo);

	sys_cfg = rdmsr(SYSCFG_MSR);
	printk(BIOS_DEBUG, "Saving SYS_CFG: 0x%08x 0x%08x\n", mtrr_def.hi, mtrr_def.lo);

	mtrrs_saved = 1;
}

static void update_if_changed(unsigned int offset, msr_t expected)
{
	msr_t tmp = rdmsr(offset);
	if (tmp.lo == expected.lo && tmp.hi == expected.hi)
		return;

	printk(BIOS_INFO, "MSR %#x was modified: 0x%08x 0x%08x\n", offset, tmp.hi, tmp.lo);
	wrmsr(offset, expected);
}

void picasso_restore_mtrrs(void)
{
	unsigned int i;
	msr_t tmp_sys_cfg;

	ASSERT_MSG(mtrrs_saved, "Must save MTRRs before restoring.\n");

	for (i = 0; i < MAX_VARIABLE_MTRRS; ++i) {
		update_if_changed(MTRR_PHYS_BASE(i), mtrr_base[i]);
		update_if_changed(MTRR_PHYS_MASK(i), mtrr_mask[i]);
	}

	for (i = 0; i < ARRAY_SIZE(fixed_mtrr_offsets); ++i)
		update_if_changed(fixed_mtrr_offsets[i], fixed_mtrrs[i]);

	update_if_changed(MTRR_DEF_TYPE_MSR, mtrr_def);

	tmp_sys_cfg = rdmsr(SYSCFG_MSR);

	/* We only care about the MTRR bits in the SYSCFG register */
	if ((tmp_sys_cfg.lo & SYS_CFG_MTRR_BITS) != (sys_cfg.lo & SYS_CFG_MTRR_BITS)) {
		printk(BIOS_INFO, "SYS_CFG was modified: 0x%08x 0x%08x\n", tmp_sys_cfg.hi,
		       tmp_sys_cfg.lo);
		tmp_sys_cfg.lo &= ~SYS_CFG_MTRR_BITS;
		tmp_sys_cfg.lo |= (sys_cfg.lo & SYS_CFG_MTRR_BITS);
		wrmsr(SYSCFG_MSR, tmp_sys_cfg);
	}
}
