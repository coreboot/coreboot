/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <console/console.h>
#include <commonlib/bsd/helpers.h>

void var_mtrr_context_init(struct var_mtrr_context *ctx, void *arg)
{
	ctx->upper_mask = (1U << (cpu_phys_address_size() - 32)) - 1;
	ctx->max_var_mtrrs = get_var_mtrr_count();
	ctx->used_var_mtrrs = 0;
	ctx->arg = arg;
}

int var_mtrr_set_with_cb(struct var_mtrr_context *ctx, uintptr_t addr, size_t size,
			int type, void (*callback)(const struct var_mtrr_context *ctx,
						uintptr_t base_addr, size_t size,
						msr_t base, msr_t mask))
{
	/* Utilize additional MTRRs if the specified size is greater than the
	   base address alignment. */
	while (size != 0) {
		uint32_t addr_lsb;
		uint32_t size_msb;
		uint32_t mtrr_size;
		msr_t base;
		msr_t mask;

		if (ctx->used_var_mtrrs >= ctx->max_var_mtrrs) {
			printk(BIOS_ERR, "No more variable MTRRs: %d\n",
					ctx->max_var_mtrrs);
			return -1;
		}

		addr_lsb = fls(addr);
		size_msb = fms(size);

		/* All MTRR entries need to have their base aligned to the mask
		   size. The maximum size is calculated by a function of the
		   min base bit set and maximum size bit set. */
		if (addr_lsb > size_msb)
			mtrr_size = 1 << size_msb;
		else
			mtrr_size = 1 << addr_lsb;

		base.hi = (uint64_t)addr >> 32;
		base.lo = addr | type;
		mask.hi = ctx->upper_mask;
		mask.lo = ~(mtrr_size - 1) | MTRR_PHYS_MASK_VALID;
		callback(ctx, addr, mtrr_size, base, mask);
		ctx->used_var_mtrrs++;

		size -= mtrr_size;
		addr += mtrr_size;
	}

	return 0;
}

static void set_mtrr(const struct var_mtrr_context *ctx, uintptr_t base_addr, size_t size,
			msr_t base, msr_t mask)
{
	int i = var_mtrr_context_current_mtrr(ctx);

	wrmsr(MTRR_PHYS_BASE(i), base);
	wrmsr(MTRR_PHYS_MASK(i), mask);
}

int var_mtrr_set(struct var_mtrr_context *ctx, uintptr_t addr, size_t size, int type)
{
	return var_mtrr_set_with_cb(ctx, addr, size, type, set_mtrr);
}
