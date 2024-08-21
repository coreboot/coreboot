/*
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload.h>
#include <assert.h>
#include <die.h>
#include <stdlib.h>
#include <unistd.h>
#include <arch/cache.h>
#include <arch/virtual.h>
#include <arch/io.h>

unsigned long virtual_offset = 0;
extern char _end[];

/*
 * MAIR Index
 * (Originally defined in src/arch/arm/include/armv7/arch/cache.h)
 */
#define MAIR_INDX_NC	0
#define MAIR_INDX_WT	1
#define MAIR_INDX_WB	2

/*
 * Translation Table Attribute
 * (Originally defined in src/arch/arm/include/armv7/arch/cache.h)
 */
#define ATTR_BASE (\
	0ULL << 54 |	/* PN. 0:Not restricted */ \
	0ULL << 53 |	/* PXN. 0:Not restricted */ \
	1 << 10 |	/* AF. 1:Accessed. This is to prevent access \
	 	 	 * fault when accessed for the first time */ \
	0 << 6 |	/* AP[2:1]. 0b00:full access from PL1 */ \
	0 << 5 |	/* NS. 0:Output address is in Secure space */ \
	0 << 1 | 	/* block/table. 0:block entry */ \
	1 << 0		/* validity. 1:valid */ \
	)
#define ATTR_NC		(ATTR_BASE | (MAIR_INDX_NC << 2))
#define ATTR_WT		(ATTR_BASE | (MAIR_INDX_WT << 2))
#define ATTR_WB		(ATTR_BASE | (MAIR_INDX_WB << 2))

/* Translation Table Entry */
typedef uint64_t pmd_t;
typedef uint64_t pgd_t;

#define SECTION_SHIFT	30
#define BLOCK_SHIFT	21
#define PAGE_SHIFT	12
#define PGD_MASK	(~0u << PAGE_SHIFT)

static pmd_t *ttb_buff = 0;
static uintptr_t work_block;
static pmd_t original_map;

int getpagesize(void)
{
	return 1 << PAGE_SHIFT;
}

static void lpae_map_init(void)
{
	pgd_t *pgd;

	die_if(!(read_ttbcr() >> 31), "LPAE is not enabled\n");

	/* get work block address */
	work_block = ALIGN_UP((uintptr_t)_end, 2*MiB);
	assert(work_block);
	printf("Work block for LPAE mapping is @ %p\n", (void *)work_block);

	/* get the address of the 1st pmd from pgd[0] */
	pgd = (pgd_t *)((uintptr_t)read_ttbr0() & PGD_MASK);
	ttb_buff = (pmd_t *)((uintptr_t)pgd[0] & PGD_MASK);
	assert(ttb_buff);

	original_map = ttb_buff[work_block >> BLOCK_SHIFT];
}

static void lpae_flush_work_block(void)
{
	dccmvac((uintptr_t)&ttb_buff[work_block >> BLOCK_SHIFT]);
	dsb();
	tlbimvaa(work_block);
	dsb();
	isb();
}

/**
 * Maps a 2MB designated block to a requested physical address, and returns
 * the address to the block or NULL on error.
 *
 * pa_mb: Physical address in MB. Has to be on a 2MB boundary.
 * policy: Data chache policy
 */
void *lpae_map_phys_addr(unsigned long pa_mb, enum dcache_policy policy)
{
	pmd_t attr;

	if (!ttb_buff)
		lpae_map_init();

	switch(policy) {
	case DCACHE_OFF:
		attr = ATTR_NC;
		break;
	case DCACHE_WRITEBACK:
		attr = ATTR_WB;
		break;
	case DCACHE_WRITETHROUGH:
		attr = ATTR_WT;
		break;
	default:
		return NULL;
	}

	ttb_buff[work_block >> BLOCK_SHIFT] =
			((pmd_t)pa_mb/2 << BLOCK_SHIFT) | attr;

	lpae_flush_work_block();

	return (void *)work_block;
}

void lpae_restore_map(void)
{
	ttb_buff[work_block >> BLOCK_SHIFT] = original_map;
	lpae_flush_work_block();
}
