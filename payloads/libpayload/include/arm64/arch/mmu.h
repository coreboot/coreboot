/*
 *
 * Copyright 2014 Google Inc.
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

#ifndef __ARCH_ARM64_MMU_H__
#define __ARCH_ARM64_MMU_H__

#include <libpayload.h>

struct mmu_memrange {
	uint64_t base;
	uint64_t size;
	uint64_t type;
};

struct mmu_ranges {
	struct mmu_memrange entries[SYSINFO_MAX_MEM_RANGES];
	size_t used;
};

/*
 * Symbols taken from linker script
 * They mark the start and end of the region used by payload
 */
extern char _start[], _end[];

/* Memory attributes for mmap regions
 * These attributes act as tag values for memrange regions
 */

#define TYPE_NORMAL_MEM            1
#define TYPE_DEV_MEM               2
#define TYPE_DMA_MEM               3

/* Descriptor attributes */

#define INVALID_DESC               0x0
#define BLOCK_DESC                 0x1
#define TABLE_DESC                 0x3
#define PAGE_DESC                  0x3

/* Block descriptor */
#define BLOCK_NS                   (1 << 5)

#define BLOCK_AP_RW                (0 << 7)
#define BLOCK_AP_RO                (1 << 7)

#define BLOCK_ACCESS               (1 << 10)

#define BLOCK_XN                   (1UL << 54)

#define BLOCK_SH_SHIFT                 (8)
#define BLOCK_SH_NON_SHAREABLE         (0 << BLOCK_SH_SHIFT)
#define BLOCK_SH_UNPREDICTABLE         (1 << BLOCK_SH_SHIFT)
#define BLOCK_SH_OUTER_SHAREABLE       (2 << BLOCK_SH_SHIFT)
#define BLOCK_SH_INNER_SHAREABLE       (3 << BLOCK_SH_SHIFT)

/* XLAT Table Init Attributes */

#define VA_START                   0x0
#define BITS_PER_VA                48
#define MIN_64_BIT_ADDR            (1UL << 32)
/* Granule size of 4KB is being used */
#define GRANULE_SIZE_SHIFT         12
#define GRANULE_SIZE               (1 << GRANULE_SIZE_SHIFT)
#define XLAT_TABLE_MASK            (~(0UL) << GRANULE_SIZE_SHIFT)
#define GRANULE_SIZE_MASK          ((1 << GRANULE_SIZE_SHIFT) - 1)

#define BITS_RESOLVED_PER_LVL   (GRANULE_SIZE_SHIFT - 3)
#define L0_ADDR_SHIFT           (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 3)
#define L1_ADDR_SHIFT           (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 2)
#define L2_ADDR_SHIFT           (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 1)
#define L3_ADDR_SHIFT           (GRANULE_SIZE_SHIFT + BITS_RESOLVED_PER_LVL * 0)

#define L0_ADDR_MASK     (((1UL << BITS_RESOLVED_PER_LVL) - 1) << L0_ADDR_SHIFT)
#define L1_ADDR_MASK     (((1UL << BITS_RESOLVED_PER_LVL) - 1) << L1_ADDR_SHIFT)
#define L2_ADDR_MASK     (((1UL << BITS_RESOLVED_PER_LVL) - 1) << L2_ADDR_SHIFT)
#define L3_ADDR_MASK     (((1UL << BITS_RESOLVED_PER_LVL) - 1) << L3_ADDR_SHIFT)

/* These macros give the size of the region addressed by each entry of a xlat
   table at any given level */
#define L3_XLAT_SIZE               (1UL << L3_ADDR_SHIFT)
#define L2_XLAT_SIZE               (1UL << L2_ADDR_SHIFT)
#define L1_XLAT_SIZE               (1UL << L1_ADDR_SHIFT)
#define L0_XLAT_SIZE               (1UL << L0_ADDR_SHIFT)

/* Block indices required for MAIR */
#define BLOCK_INDEX_MEM_DEV_NGNRNE 0
#define BLOCK_INDEX_MEM_DEV_NGNRE  1
#define BLOCK_INDEX_MEM_DEV_GRE    2
#define BLOCK_INDEX_MEM_NORMAL_NC  3
#define BLOCK_INDEX_MEM_NORMAL     4

#define BLOCK_INDEX_SHIFT          2

/* MAIR attributes */
#define MAIR_ATTRIBUTES            ((0x00 << (BLOCK_INDEX_MEM_DEV_NGNRNE*8)) | \
				    (0x04 << (BLOCK_INDEX_MEM_DEV_NGNRE*8))  | \
				    (0x0c << (BLOCK_INDEX_MEM_DEV_GRE*8))    | \
				    (0x44 << (BLOCK_INDEX_MEM_NORMAL_NC*8))  | \
				    (0xffUL << (BLOCK_INDEX_MEM_NORMAL*8)))

/* TCR attributes */
#define TCR_TOSZ                   (64 - BITS_PER_VA)

#define TCR_IRGN0_SHIFT            8
#define TCR_IRGN0_NM_NC            (0x00 << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NM_WBWAC         (0x01 << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NM_WTC           (0x02 << TCR_IRGN0_SHIFT)
#define TCR_IRGN0_NM_WBNWAC        (0x03 << TCR_IRGN0_SHIFT)

#define TCR_ORGN0_SHIFT            10
#define TCR_ORGN0_NM_NC            (0x00 << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NM_WBWAC         (0x01 << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NM_WTC           (0x02 << TCR_ORGN0_SHIFT)
#define TCR_ORGN0_NM_WBNWAC        (0x03 << TCR_ORGN0_SHIFT)

#define TCR_SH0_SHIFT              12
#define TCR_SH0_NC                 (0x0 << TCR_SH0_SHIFT)
#define TCR_SH0_OS                 (0x2 << TCR_SH0_SHIFT)
#define TCR_SH0_IS                 (0x3 << TCR_SH0_SHIFT)

#define TCR_TG0_SHIFT              14
#define TCR_TG0_4KB                (0x0 << TCR_TG0_SHIFT)
#define TCR_TG0_64KB               (0x1 << TCR_TG0_SHIFT)
#define TCR_TG0_16KB               (0x2 << TCR_TG0_SHIFT)

#define TCR_PS_SHIFT               16
#define TCR_PS_4GB                 (0x0 << TCR_PS_SHIFT)
#define TCR_PS_64GB                (0x1 << TCR_PS_SHIFT)
#define TCR_PS_1TB                 (0x2 << TCR_PS_SHIFT)
#define TCR_PS_4TB                 (0x3 << TCR_PS_SHIFT)
#define TCR_PS_16TB                (0x4 << TCR_PS_SHIFT)
#define TCR_PS_256TB               (0x5 << TCR_PS_SHIFT)

#define TCR_TBI_SHIFT              20
#define TCR_TBI_USED               (0x0 << TCR_TBI_SHIFT)
#define TCR_TBI_IGNORED            (0x1 << TCR_TBI_SHIFT)

#define DMA_DEFAULT_SIZE           (32 * MiB)
#define TTB_DEFAULT_SIZE           0x100000

#define MB_SIZE			   (1UL << 20)

/* Initialize the MMU TTB tables using the mmu_ranges */
uint64_t mmu_init(struct mmu_ranges *mmu_ranges);

/* Enable the mmu based on previous mmu_init(). */
void mmu_enable(void);

/* Disable mmu */
void mmu_disable(void);

/* Change a memory type for a range of bytes at runtime. */
void mmu_config_range(void *start, size_t size, uint64_t tag);

/*
 * Based on the memory ranges provided in coreboot tables,
 * initialize the mmu_memranges used for mmu initialization
 * cb_ranges -> Memory ranges present in cb tables
 * mmu_ranges -> mmu_memranges initialized by this function
 */
struct mmu_memrange* mmu_init_ranges_from_sysinfo(struct memrange *cb_ranges,
						  uint64_t ncb,
						  struct mmu_ranges *mmu_ranges);

/*
 * Functions for handling the initialization of memory ranges and enabling mmu
 * before coreboot tables are parsed
 */
void mmu_presysinfo_memory_used(uint64_t base, uint64_t size);
void mmu_presysinfo_enable(void);

/*
 * Functions for exposing the used memory ranges to payloads. The ranges contain
 * all used memory ranges that are actually used by payload. i.e. _start -> _end
 * in linker script, the coreboot tables and framebuffer/DMA allocated in MMU
 * initialization.
 */
const struct mmu_ranges *mmu_get_used_ranges(void);
#endif // __ARCH_ARM64_MMU_H__
