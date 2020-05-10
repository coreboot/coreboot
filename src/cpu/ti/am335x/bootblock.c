/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#include <arch/cache.h>
#include <bootblock_common.h>

void bootblock_soc_init(void)
{
	uint32_t sctlr;

	/* enable dcache */
	sctlr = read_sctlr();
	sctlr |= SCTLR_C;
	write_sctlr(sctlr);
}
