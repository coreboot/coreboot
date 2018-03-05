/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __ARCH_ARM64_MMU_H__
#define __ARCH_ARM64_MMU_H__

#include <types.h>

/* Memory attributes for mmap regions
 * These attributes act as tag values for memrange regions
 */

/* Normal memory / device */
#define MA_MEM                     (1 << 0)
#define MA_DEV                     (0 << 0)

/* Secure / non-secure */
#define MA_NS                      (1 << 1)
#define MA_S                       (0 << 1)

/* Read only / Read-write */
#define MA_RO                      (1 << 2)
#define MA_RW                      (0 << 2)

/* Non-cacheable memory. */
#define MA_MEM_NC                  (1 << 3)

/* Descriptor attributes */

#define INVALID_DESC               0x0
#define BLOCK_DESC                 0x1
#define TABLE_DESC                 0x3
#define PAGE_DESC                  0x3
#define DESC_MASK                  0x3

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

/* Sentinel descriptor to mark first PTE of an unused table. It must be a value
 * that cannot occur naturally as part of a page table. (Bits [1:0] = 0b00 makes
 * this an unmapped page, but some page attribute bits are still set.) */
#define UNUSED_DESC                0x6EbAAD0BBADbA6E0

/* XLAT Table Init Attributes */

#define VA_START                   0x0
#define BITS_PER_VA                48
/* Granule size of 4KB is being used */
#define GRANULE_SIZE_SHIFT         12
#define GRANULE_SIZE               (1 << GRANULE_SIZE_SHIFT)
#define XLAT_ADDR_MASK             ((1UL << BITS_PER_VA) - GRANULE_SIZE)
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

#define BLOCK_INDEX_MASK           0x7
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

/* Initialize MMU registers and page table memory region. */
void mmu_init(void);
/* Change a memory type for a range of bytes at runtime. */
void mmu_config_range(void *start, size_t size, uint64_t tag);
/* Enable the MMU (need previous mmu_init() and configured ranges!). */
void mmu_enable(void);
/* Disable the MMU (which also disables dcache but not icache). */
void mmu_disable(void);

#endif /* __ARCH_ARM64_MMU_H__ */
