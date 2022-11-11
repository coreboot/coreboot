/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <stage_cache.h>
#include <types.h>
#include <inttypes.h>

/*
 *        Subregions within SMM
 *     +-------------------------+
 *     |          IED            | IED_REGION_SIZE
 *     +-------------------------+
 *     |  External Stage Cache   | SMM_RESERVED_SIZE
 *     +-------------------------+
 *     |      code and data      |
 *     |         (TSEG)          |
 *     +-------------------------+ TSEG
 */
int smm_subregion(int sub, uintptr_t *start, size_t *size)
{
	uintptr_t sub_base;
	size_t sub_size;
	const size_t ied_size = CONFIG_IED_REGION_SIZE;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	if (CONFIG(SMM_TSEG))
		smm_region(&sub_base, &sub_size);
	else if (CONFIG(SMM_ASEG))
		aseg_region(&sub_base, &sub_size);
	else
		return -1;

	ASSERT(IS_ALIGNED(sub_base, sub_size));
	ASSERT(sub_size > (cache_size + ied_size));

	switch (sub) {
	case SMM_SUBREGION_HANDLER:
		/* Handler starts at the base of TSEG. */
		sub_size -= ied_size;
		sub_size -= cache_size;
		break;
	case SMM_SUBREGION_CACHE:
		/* External cache is in the middle of TSEG. */
		sub_base += sub_size - (ied_size + cache_size);
		sub_size = cache_size;
		break;
	case SMM_SUBREGION_CHIPSET:
		/* IED is at the top. */
		sub_base += sub_size - ied_size;
		sub_size = ied_size;
		break;
	default:
		*start = 0;
		*size = 0;
		return -1;
	}

	*start = sub_base;
	*size = sub_size;
	return 0;
}

void stage_cache_external_region(void **base, size_t *size)
{
	if (smm_subregion(SMM_SUBREGION_CACHE, (uintptr_t *)base, size)) {
		printk(BIOS_ERR, "No cache SMM subregion.\n");
		*base = NULL;
		*size = 0;
	}
}

void smm_list_regions(void)
{
	uintptr_t base;
	size_t size;
	int i;

	smm_region(&base, &size);
	if (!size)
		return;

	printk(BIOS_DEBUG, "SMM Memory Map\n");
	printk(BIOS_DEBUG, "SMRAM       : 0x%" PRIxPTR " 0x%zx\n", base, size);

	for (i = 0; i < SMM_SUBREGION_NUM; i++) {
		if (smm_subregion(i, &base, &size))
			continue;
		printk(BIOS_DEBUG, " Subregion %d: 0x%" PRIxPTR " 0x%zx\n", i, base, size);
	}
}
