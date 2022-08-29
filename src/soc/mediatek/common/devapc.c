/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <soc/devapc.h>
#include <soc/devapc_common.h>

void *getreg_domain(uintptr_t base, unsigned int offset,
		    enum domain_id domain_id, unsigned int index)
{
	return (void *)(base + offset + domain_id * DOMAIN_OFT + index * IDX_OFT);
}

void *getreg(uintptr_t base, unsigned int offset)
{
	return getreg_domain(base, offset, 0, 0);
}

void set_module_apc(uintptr_t base, uint32_t module, enum domain_id domain_id,
		    enum devapc_perm_type perm)
{
	uint32_t apc_register_index;
	uint32_t apc_set_index;

	apc_register_index = module / MOD_NO_IN_1_DEVAPC;
	apc_set_index = module % MOD_NO_IN_1_DEVAPC;

	clrsetbits32(getreg_domain(base, 0, domain_id, apc_register_index),
		     0x3 << (apc_set_index * 2),
		     perm << (apc_set_index * 2));
}
